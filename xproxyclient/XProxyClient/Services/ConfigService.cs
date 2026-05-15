using XProxyClient.Models;
using System;
using System.Collections.Generic;
using System.IO;
using Newtonsoft.Json;

namespace XProxyClient.Services
{
    public class ConfigService : IConfigService
    {
        private AppConfig _config;
        private readonly string _configPath;

        public ConfigService()
        {
            string appDataPath = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData);
            _configPath = Path.Combine(appDataPath, "XProxyClient", "config.json");
            LoadConfig();
        }

        public AppConfig GetConfig()
        {
            return _config;
        }

        public void SaveConfig()
        {
            SaveConfig(_config);
        }

        public void SaveConfig(AppConfig config)
        {
            try
            {
                string directory = Path.GetDirectoryName(_configPath);
                if (!Directory.Exists(directory))
                {
                    Directory.CreateDirectory(directory);
                }

                string json = JsonConvert.SerializeObject(config, Formatting.Indented);
                File.WriteAllText(_configPath, json);
            }
            catch
            {
            }
        }

        public void ResetConfig()
        {
            _config = new AppConfig();
            SaveConfig();
        }

        private void LoadConfig()
        {
            try
            {
                if (File.Exists(_configPath))
                {
                    string json = File.ReadAllText(_configPath);
                    _config = JsonConvert.DeserializeObject<AppConfig>(json);
                }
            }
            catch
            {
            }

            if (_config == null)
            {
                _config = new AppConfig();
            }
        }

        public void AddServer(ServerConfig server)
        {
            _config.Servers.Add(server);
            SaveConfig();
        }

        public void UpdateServer(ServerConfig server)
        {
            int index = _config.Servers.FindIndex(s => s.Id == server.Id);
            if (index >= 0)
            {
                _config.Servers[index] = server;
                SaveConfig();
            }
        }

        public void RemoveServer(string serverId)
        {
            _config.Servers.RemoveAll(s => s.Id == serverId);
            SaveConfig();
        }

        public List<ServerConfig> GetAllServers()
        {
            return _config.Servers;
        }

        public void AddSubscription(SubscriptionConfig subscription)
        {
            _config.Subscriptions.Add(subscription);
            SaveConfig();
        }

        public void UpdateSubscription(SubscriptionConfig subscription)
        {
            int index = _config.Subscriptions.FindIndex(s => s.Id == subscription.Id);
            if (index >= 0)
            {
                _config.Subscriptions[index] = subscription;
                SaveConfig();
            }
        }

        public void RemoveSubscription(string subscriptionId)
        {
            _config.Subscriptions.RemoveAll(s => s.Id == subscriptionId);
            SaveConfig();
        }

        public List<SubscriptionConfig> GetAllSubscriptions()
        {
            return _config.Subscriptions;
        }
    }
}
