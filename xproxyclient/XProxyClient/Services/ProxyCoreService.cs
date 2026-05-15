using XProxyClient.Models;
using System;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace XProxyClient.Services
{
    public class ProxyCoreService : IProxyCoreService
    {
        private readonly IConfigService _configService;
        private Process _coreProcess;
        private ConnectionStatus _status;
        private ServerConfig _currentServer;

        public ProxyCoreService(IConfigService configService)
        {
            _configService = configService;
            _status = ConnectionStatus.Disconnected;
        }

        public async Task StartCore(ServerConfig server)
        {
            try
            {
                _status = ConnectionStatus.Connecting;
                _currentServer = server;

                AppConfig config = _configService.GetConfig();
                string configJson = GenerateConfigJson(server, config);

                string tempConfigPath = Path.GetTempFileName();
                File.WriteAllText(tempConfigPath, configJson);

                string corePath = GetCorePath(config);
                if (string.IsNullOrEmpty(corePath) || !File.Exists(corePath))
                {
                    throw new FileNotFoundException("代理核心未找到，请在设置中配置路径");
                }

                ProcessStartInfo startInfo = new ProcessStartInfo
                {
                    FileName = corePath,
                    Arguments = config.CoreType == CoreType.Xray ? $"-c \"{tempConfigPath}\"" : $"run -c \"{tempConfigPath}\"",
                    UseShellExecute = false,
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    CreateNoWindow = true
                };

                _coreProcess = new Process { StartInfo = startInfo };
                _coreProcess.Start();

                await Task.Delay(1000);

                if (_coreProcess.HasExited)
                {
                    string error = _coreProcess.StandardError.ReadToEnd();
                    _status = ConnectionStatus.Failed;
                    throw new Exception($"启动失败: {error}");
                }

                _status = ConnectionStatus.Connected;
            }
            catch (Exception ex)
            {
                _status = ConnectionStatus.Failed;
                throw new Exception($"启动核心失败: {ex.Message}", ex);
            }
        }

        public void StopCore()
        {
            try
            {
                if (_coreProcess != null && !_coreProcess.HasExited)
                {
                    _coreProcess.Kill();
                    _coreProcess.WaitForExit(5000);
                    _coreProcess.Dispose();
                    _coreProcess = null;
                }
            }
            catch
            {
            }
            finally
            {
                _status = ConnectionStatus.Disconnected;
                _currentServer = null;
            }
        }

        public bool IsConnected()
        {
            return _status == ConnectionStatus.Connected && _coreProcess != null && !_coreProcess.HasExited;
        }

        public string GetConnectionStatus()
        {
            switch (_status)
            {
                case ConnectionStatus.Connected:
                    return $"已连接 - {_currentServer?.Name}";
                case ConnectionStatus.Connecting:
                    return "正在连接...";
                case ConnectionStatus.Failed:
                    return "连接失败";
                default:
                    return "未连接";
            }
        }

        public async Task<long> TestLatency(ServerConfig server)
        {
            try
            {
                Stopwatch stopwatch = Stopwatch.StartNew();

                using (TcpClient client = new TcpClient())
                {
                    Task connectTask = client.ConnectAsync(server.Address, server.Port);
                    Task timeoutTask = Task.Delay(5000);

                    Task completedTask = await Task.WhenAny(connectTask, timeoutTask);
                    if (completedTask == timeoutTask)
                    {
                        return -1;
                    }

                    stopwatch.Stop();
                    return stopwatch.ElapsedMilliseconds;
                }
            }
            catch
            {
                return -1;
            }
        }

        private string GetCorePath(AppConfig config)
        {
            if (config.CoreType == CoreType.Xray)
            {
                return config.XrayPath;
            }
            return config.SingBoxPath;
        }

        private string GenerateConfigJson(ServerConfig server, AppConfig appConfig)
        {
            if (appConfig.CoreType == CoreType.Xray)
            {
                return GenerateXrayConfig(server, appConfig);
            }
            return GenerateSingBoxConfig(server, appConfig);
        }

        private string GenerateXrayConfig(ServerConfig server, AppConfig appConfig)
        {
            JObject config = new JObject();
            JArray inbounds = new JArray();

            JObject socksInbound = new JObject
            {
                ["port"] = appConfig.SocksPort,
                ["listen"] = "127.0.0.1",
                ["protocol"] = "socks",
                ["settings"] = new JObject
                {
                    ["udp"] = true,
                    ["auth"] = "noauth"
                }
            };
            inbounds.Add(socksInbound);

            JObject httpInbound = new JObject
            {
                ["port"] = appConfig.HttpPort,
                ["listen"] = "127.0.0.1",
                ["protocol"] = "http"
            };
            inbounds.Add(httpInbound);

            config["inbounds"] = inbounds;

            JArray outbounds = new JArray();
            JObject outbound = new JObject
            {
                ["protocol"] = GetXrayProtocol(server.Type),
                ["settings"] = GenerateXrayOutboundSettings(server),
                ["streamSettings"] = GenerateXrayStreamSettings(server)
            };
            outbounds.Add(outbound);
            config["outbounds"] = outbounds;

            config["log"] = new JObject
            {
                ["loglevel"] = appConfig.LogLevel
            };

            return JsonConvert.SerializeObject(config, Formatting.Indented);
        }

        private string GenerateSingBoxConfig(ServerConfig server, AppConfig appConfig)
        {
            JObject config = new JObject
            {
                ["log"] = new JObject
                {
                    ["level"] = appConfig.LogLevel
                },
                ["inbounds"] = new JArray
                {
                    new JObject
                    {
                        ["type"] = "socks",
                        ["listen"] = "127.0.0.1",
                        ["listen_port"] = appConfig.SocksPort,
                        ["udp"] = true
                    },
                    new JObject
                    {
                        ["type"] = "http",
                        ["listen"] = "127.0.0.1",
                        ["listen_port"] = appConfig.HttpPort
                    }
                },
                ["outbounds"] = new JArray
                {
                    GenerateSingBoxOutbound(server)
                }
            };

            return JsonConvert.SerializeObject(config, Formatting.Indented);
        }

        private string GetXrayProtocol(ProxyType type)
        {
            switch (type)
            {
                case ProxyType.VMess:
                    return "vmess";
                case ProxyType.VLESS:
                    return "vless";
                case ProxyType.Shadowsocks:
                    return "shadowsocks";
                case ProxyType.Trojan:
                    return "trojan";
                case ProxyType.Socks5:
                    return "socks";
                case ProxyType.HTTP:
                    return "http";
                default:
                    return "vmess";
            }
        }

        private JObject GenerateXrayOutboundSettings(ServerConfig server)
        {
            JObject settings = new JObject();
            JArray vnext = new JArray();

            switch (server.Type)
            {
                case ProxyType.VMess:
                    vnext.Add(new JObject
                    {
                        ["address"] = server.Address,
                        ["port"] = server.Port,
                        ["users"] = new JArray
                        {
                            new JObject
                            {
                                ["id"] = server.Uuid,
                                ["alterId"] = int.TryParse(server.AlterId, out int alterId) ? alterId : 0,
                                ["security"] = string.IsNullOrEmpty(server.Security) ? "auto" : server.Security
                            }
                        }
                    });
                    settings["vnext"] = vnext;
                    break;

                case ProxyType.VLESS:
                    vnext.Add(new JObject
                    {
                        ["address"] = server.Address,
                        ["port"] = server.Port,
                        ["users"] = new JArray
                        {
                            new JObject
                            {
                                ["id"] = server.Uuid,
                                ["flow"] = server.Flow,
                                ["encryption"] = string.IsNullOrEmpty(server.Encryption) ? "none" : server.Encryption
                            }
                        }
                    });
                    settings["vnext"] = vnext;
                    break;

                case ProxyType.Shadowsocks:
                    JArray servers = new JArray
                    {
                        new JObject
                        {
                            ["address"] = server.Address,
                            ["port"] = server.Port,
                            ["method"] = server.Encryption,
                            ["password"] = server.Password
                        }
                    };
                    settings["servers"] = servers;
                    break;

                case ProxyType.Trojan:
                    JArray trojanServers = new JArray
                    {
                        new JObject
                        {
                            ["address"] = server.Address,
                            ["port"] = server.Port,
                            ["password"] = server.Password
                        }
                    };
                    settings["servers"] = trojanServers;
                    break;

                case ProxyType.Socks5:
                    JArray socksServers = new JArray
                    {
                        new JObject
                        {
                            ["address"] = server.Address,
                            ["port"] = server.Port,
                            ["users"] = new JArray
                            {
                                new JObject
                                {
                                    ["user"] = server.Username,
                                    ["pass"] = server.Password
                                }
                            }
                        }
                    };
                    settings["servers"] = socksServers;
                    break;

                case ProxyType.HTTP:
                    JArray httpServers = new JArray
                    {
                        new JObject
                        {
                            ["address"] = server.Address,
                            ["port"] = server.Port,
                            ["users"] = new JArray
                            {
                                new JObject
                                {
                                    ["user"] = server.Username,
                                    ["pass"] = server.Password
                                }
                            }
                        }
                    };
                    settings["servers"] = httpServers;
                    break;
            }

            return settings;
        }

        private JObject GenerateXrayStreamSettings(ServerConfig server)
        {
            JObject streamSettings = new JObject
            {
                ["network"] = string.IsNullOrEmpty(server.Network) ? "tcp" : server.Network,
                ["security"] = server.Tls
            };

            if (server.Tls == "tls")
            {
                JObject tlsSettings = new JObject
                {
                    ["serverName"] = server.Sni,
                    ["allowInsecure"] = server.AllowInsecure
                };
                if (!string.IsNullOrEmpty(server.Alpn))
                {
                    JArray alpnArray = new JArray();
                    foreach (string alpn in server.Alpn.Split(','))
                    {
                        alpnArray.Add(alpn.Trim());
                    }
                    tlsSettings["alpn"] = alpnArray;
                }
                streamSettings["tlsSettings"] = tlsSettings;
            }

            string network = string.IsNullOrEmpty(server.Network) ? "tcp" : server.Network;
            switch (network)
            {
                case "tcp":
                    streamSettings["tcpSettings"] = new JObject
                    {
                        ["header"] = new JObject
                        {
                            ["type"] = string.IsNullOrEmpty(server.HeaderType) ? "none" : server.HeaderType
                        }
                    };
                    break;

                case "ws":
                    streamSettings["wsSettings"] = new JObject
                    {
                        ["path"] = server.Path,
                        ["headers"] = new JObject
                        {
                            ["Host"] = server.Host
                        }
                    };
                    break;

                case "http":
                case "h2":
                    streamSettings["httpSettings"] = new JObject
                    {
                        ["path"] = server.Path,
                        ["host"] = new JArray { server.Host }
                    };
                    break;

                case "grpc":
                    streamSettings["grpcSettings"] = new JObject
                    {
                        ["serviceName"] = server.Path
                    };
                    break;

                case "kcp":
                    streamSettings["kcpSettings"] = new JObject
                    {
                        ["header"] = new JObject
                        {
                            ["type"] = string.IsNullOrEmpty(server.HeaderType) ? "none" : server.HeaderType
                        }
                    };
                    break;

                case "quic":
                    streamSettings["quicSettings"] = new JObject
                    {
                        ["security"] = server.Security,
                        ["key"] = server.Password,
                        ["header"] = new JObject
                        {
                            ["type"] = string.IsNullOrEmpty(server.HeaderType) ? "none" : server.HeaderType
                        }
                    };
                    break;
            }

            return streamSettings;
        }

        private JObject GenerateSingBoxOutbound(ServerConfig server)
        {
            JObject outbound = new JObject
            {
                ["type"] = GetSingBoxProtocol(server.Type),
                ["server"] = server.Address,
                ["server_port"] = server.Port
            };

            switch (server.Type)
            {
                case ProxyType.VMess:
                    outbound["uuid"] = server.Uuid;
                    outbound["alter_id"] = int.TryParse(server.AlterId, out int alterId) ? alterId : 0;
                    outbound["security"] = string.IsNullOrEmpty(server.Security) ? "auto" : server.Security;
                    break;

                case ProxyType.VLESS:
                    outbound["uuid"] = server.Uuid;
                    outbound["flow"] = server.Flow;
                    outbound["packet_encoding"] = "xudp";
                    break;

                case ProxyType.Shadowsocks:
                    outbound["method"] = server.Encryption;
                    outbound["password"] = server.Password;
                    break;

                case ProxyType.Trojan:
                    outbound["password"] = server.Password;
                    break;

                case ProxyType.Socks5:
                    outbound["username"] = server.Username;
                    outbound["password"] = server.Password;
                    break;

                case ProxyType.HTTP:
                    outbound["username"] = server.Username;
                    outbound["password"] = server.Password;
                    break;
            }

            if (!string.IsNullOrEmpty(server.Network))
            {
                outbound["transport"] = new JObject
                {
                    ["type"] = server.Network
                };

                if (server.Network == "ws")
                {
                    outbound["transport"]["path"] = server.Path;
                    outbound["transport"]["headers"] = new JObject
                    {
                        ["Host"] = server.Host
                    };
                }
                else if (server.Network == "http")
                {
                    outbound["transport"]["path"] = server.Path;
                    outbound["transport"]["host"] = new JArray { server.Host };
                }
                else if (server.Network == "grpc")
                {
                    outbound["transport"]["service_name"] = server.Path;
                }
            }

            if (server.Tls == "tls")
            {
                outbound["tls"] = new JObject
                {
                    ["enabled"] = true,
                    ["server_name"] = server.Sni,
                    ["insecure"] = server.AllowInsecure
                };
                if (!string.IsNullOrEmpty(server.Alpn))
                {
                    JArray alpnArray = new JArray();
                    foreach (string alpn in server.Alpn.Split(','))
                    {
                        alpnArray.Add(alpn.Trim());
                    }
                    outbound["tls"]["alpn"] = alpnArray;
                }
            }

            return outbound;
        }

        private string GetSingBoxProtocol(ProxyType type)
        {
            switch (type)
            {
                case ProxyType.VMess:
                    return "vmess";
                case ProxyType.VLESS:
                    return "vless";
                case ProxyType.Shadowsocks:
                    return "shadowsocks";
                case ProxyType.Trojan:
                    return "trojan";
                case ProxyType.Hysteria2:
                    return "hysteria2";
                case ProxyType.TUIC:
                    return "tuic";
                case ProxyType.WireGuard:
                    return "wireguard";
                case ProxyType.Socks5:
                    return "socks";
                case ProxyType.HTTP:
                    return "http";
                default:
                    return "vmess";
            }
        }
    }
}
