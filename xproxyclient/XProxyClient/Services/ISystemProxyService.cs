using XProxyClient.Models;

namespace XProxyClient.Services
{
    public interface ISystemProxyService
    {
        void EnableSystemProxy(int socksPort, int httpPort, SystemProxyMode mode);
        void DisableSystemProxy();
        SystemProxyMode GetCurrentMode();
    }
}
