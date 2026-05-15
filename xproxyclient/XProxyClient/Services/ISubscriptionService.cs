using XProxyClient.Models;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace XProxyClient.Services
{
    public interface ISubscriptionService
    {
        Task<List<ServerConfig>> FetchSubscription(string url);
        Task UpdateSubscription(SubscriptionConfig subscription);
        Task UpdateAllSubscriptions();
        string ParseShareLink(string link);
    }
}
