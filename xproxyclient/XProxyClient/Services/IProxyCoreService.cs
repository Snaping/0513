using XProxyClient.Models;
using System.Threading.Tasks;

namespace XProxyClient.Services
{
    public interface IProxyCoreService
    {
        Task StartCore(ServerConfig server);
        void StopCore();
        bool IsConnected();
        string GetConnectionStatus();
        Task<long> TestLatency(ServerConfig server);
    }
}
