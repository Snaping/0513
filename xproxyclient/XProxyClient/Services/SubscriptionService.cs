using XProxyClient.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json.Linq;

namespace XProxyClient.Services
{
    public class SubscriptionService : ISubscriptionService
    {
        private readonly IConfigService _configService;
        private readonly HttpClient _httpClient;

        public SubscriptionService(IConfigService configService)
        {
            _configService = configService;
            _httpClient = new HttpClient();
            _httpClient.Timeout = TimeSpan.FromSeconds(30);
        }

        public async Task<List<ServerConfig>> FetchSubscription(string url)
        {
            try
            {
                HttpResponseMessage response = await _httpClient.GetAsync(url);
                response.EnsureSuccessStatusCode();
                string content = await response.Content.ReadAsStringAsync();

                string decodedContent;
                try
                {
                    string base64 = content.Trim().Replace("-", "+").Replace("_", "/");
                    switch (base64.Length % 4)
                    {
                        case 2:
                            base64 += "==";
                            break;
                        case 3:
                            base64 += "=";
                            break;
                    }
                    byte[] bytes = Convert.FromBase64String(base64);
                    decodedContent = Encoding.UTF8.GetString(bytes);
                }
                catch
                {
                    decodedContent = content;
                }

                List<ServerConfig> servers = new List<ServerConfig>();
                string[] lines = decodedContent.Split(new char[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries);

                foreach (string line in lines)
                {
                    ServerConfig server = ParseShareLinkToServer(line);
                    if (server != null)
                    {
                        servers.Add(server);
                    }
                }

                return servers;
            }
            catch
            {
                return new List<ServerConfig>();
            }
        }

        public async Task UpdateSubscription(SubscriptionConfig subscription)
        {
            List<ServerConfig> servers = await FetchSubscription(subscription.Url);

            _configService.GetConfig().Servers.RemoveAll(s => s.SubscriptionId == subscription.Id);

            foreach (ServerConfig server in servers)
            {
                server.SubscriptionId = subscription.Id;
                _configService.AddServer(server);
            }

            subscription.LastUpdate = DateTime.Now;
            subscription.ServerCount = servers.Count;
            _configService.UpdateSubscription(subscription);
        }

        public async Task UpdateAllSubscriptions()
        {
            foreach (SubscriptionConfig subscription in _configService.GetAllSubscriptions())
            {
                await UpdateSubscription(subscription);
            }
        }

        public string ParseShareLink(string link)
        {
            ServerConfig server = ParseShareLinkToServer(link);
            if (server != null)
            {
                return string.Format("{0} ({1} - {2}:{3})", server.Name, server.Type, server.Address, server.Port);
            }
            return null;
        }

        private ServerConfig ParseShareLinkToServer(string link)
        {
            try
            {
                if (link.StartsWith("vmess://", StringComparison.OrdinalIgnoreCase))
                {
                    return ParseVMess(link);
                }
                else if (link.StartsWith("vless://", StringComparison.OrdinalIgnoreCase))
                {
                    return ParseVLESS(link);
                }
                else if (link.StartsWith("ss://", StringComparison.OrdinalIgnoreCase))
                {
                    return ParseShadowsocks(link);
                }
                else if (link.StartsWith("trojan://", StringComparison.OrdinalIgnoreCase))
                {
                    return ParseTrojan(link);
                }
            }
            catch
            {
            }
            return null;
        }

        private ServerConfig ParseVMess(string link)
        {
            string base64 = link.Substring(8);
            byte[] bytes = Convert.FromBase64String(base64);
            string json = Encoding.UTF8.GetString(bytes);
            JObject obj = JObject.Parse(json);

            ServerConfig server = new ServerConfig();
            server.Type = ProxyType.VMess;
            server.Name = obj.GetValue("ps") != null ? obj.GetValue("ps").ToString() : "VMess Server";
            server.Address = obj.GetValue("add") != null ? obj.GetValue("add").ToString() : string.Empty;
            server.Port = obj.GetValue("port") != null ? int.Parse(obj.GetValue("port").ToString()) : 0;
            server.Uuid = obj.GetValue("id") != null ? obj.GetValue("id").ToString() : string.Empty;
            server.AlterId = obj.GetValue("aid") != null ? obj.GetValue("aid").ToString() : "0";
            server.Security = obj.GetValue("scy") != null ? obj.GetValue("scy").ToString() : "auto";
            server.Network = obj.GetValue("net") != null ? obj.GetValue("net").ToString() : "tcp";
            server.HeaderType = obj.GetValue("type") != null ? obj.GetValue("type").ToString() : "none";
            server.Host = obj.GetValue("host") != null ? obj.GetValue("host").ToString() : string.Empty;
            server.Path = obj.GetValue("path") != null ? obj.GetValue("path").ToString() : string.Empty;
            server.Tls = obj.GetValue("tls") != null ? obj.GetValue("tls").ToString() : string.Empty;
            server.Sni = obj.GetValue("sni") != null ? obj.GetValue("sni").ToString() : string.Empty;

            return server;
        }

        private ServerConfig ParseVLESS(string link)
        {
            Uri uri = new Uri(link);
            System.Collections.Specialized.NameValueCollection query = System.Web.HttpUtility.ParseQueryString(uri.Query);

            ServerConfig server = new ServerConfig();
            server.Type = ProxyType.VLESS;
            server.Name = !string.IsNullOrEmpty(uri.Fragment) ? Uri.UnescapeDataString(uri.Fragment.TrimStart('#')) : "VLESS Server";
            server.Address = uri.Host;
            server.Port = uri.Port;
            server.Uuid = uri.UserInfo;
            server.Flow = query["flow"] ?? string.Empty;
            server.Encryption = query["encryption"] ?? "none";
            server.Network = query["type"] ?? "tcp";
            server.Host = query["host"] ?? string.Empty;
            server.Path = query["path"] ?? string.Empty;
            server.Tls = query["security"] ?? string.Empty;
            server.Sni = query["sni"] ?? string.Empty;
            server.Alpn = query["alpn"] ?? string.Empty;
            server.AllowInsecure = query["allowInsecure"] == "1";

            return server;
        }

        private ServerConfig ParseShadowsocks(string link)
        {
            string[] parts = link.Substring(5).Split('#');
            string base64 = parts[0];
            string name = parts.Length > 1 ? System.Web.HttpUtility.UrlDecode(parts[1]) : "Shadowsocks Server";

            byte[] bytes = Convert.FromBase64String(base64);
            string decoded = Encoding.UTF8.GetString(bytes);
            string[] userInfoParts = decoded.Split('@');
            string[] methodPass = userInfoParts[0].Split(':');
            string[] addrPort = userInfoParts[1].Split(':');

            ServerConfig server = new ServerConfig();
            server.Type = ProxyType.Shadowsocks;
            server.Name = name;
            server.Address = addrPort[0];
            server.Port = int.Parse(addrPort[1]);
            server.Encryption = methodPass[0];
            server.Password = methodPass[1];

            return server;
        }

        private ServerConfig ParseTrojan(string link)
        {
            Uri uri = new Uri(link);
            System.Collections.Specialized.NameValueCollection query = System.Web.HttpUtility.ParseQueryString(uri.Query);

            ServerConfig server = new ServerConfig();
            server.Type = ProxyType.Trojan;
            server.Name = !string.IsNullOrEmpty(uri.Fragment) ? Uri.UnescapeDataString(uri.Fragment.TrimStart('#')) : "Trojan Server";
            server.Address = uri.Host;
            server.Port = uri.Port;
            server.Password = uri.UserInfo;
            server.Network = query["type"] ?? "tcp";
            server.Host = query["host"] ?? string.Empty;
            server.Path = query["path"] ?? string.Empty;
            server.Tls = "tls";
            server.Sni = query["sni"] ?? string.Empty;
            server.Alpn = query["alpn"] ?? string.Empty;
            server.AllowInsecure = query["allowInsecure"] == "1";

            return server;
        }
    }
}
