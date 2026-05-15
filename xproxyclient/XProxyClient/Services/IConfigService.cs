using XProxyClient.Models;
using System.Collections.Generic;

namespace XProxyClient.Services
{
    public interface IConfigService
    {
        AppConfig GetConfig();
        void SaveConfig();
        void SaveConfig(AppConfig config);
        void ResetConfig();
        void AddServer(ServerConfig server);
        void UpdateServer(ServerConfig server);
        void RemoveServer(string serverId);
        List<ServerConfig> GetAllServers();
        void AddSubscription(SubscriptionConfig subscription);
        void UpdateSubscription(SubscriptionConfig subscription);
        void RemoveSubscription(string subscriptionId);
        List<SubscriptionConfig> GetAllSubscriptions();
    }
}
