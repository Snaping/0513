using System;
using System.Windows;
using Microsoft.Extensions.DependencyInjection;

namespace XProxyClient
{
    public partial class App : Application
    {
        public static IServiceProvider ServiceProvider { get; private set; }

        public App()
        {
            ConfigureServices();
        }

        private void ConfigureServices()
        {
            ServiceCollection services = new ServiceCollection();

            services.AddSingleton<Services.IConfigService, Services.ConfigService>();
            services.AddSingleton<Services.IProxyCoreService, Services.ProxyCoreService>();
            services.AddSingleton<Services.ISystemProxyService, Services.SystemProxyService>();
            services.AddSingleton<Services.ISubscriptionService, Services.SubscriptionService>();

            ServiceProvider = services.BuildServiceProvider();
        }

        protected override void OnExit(ExitEventArgs e)
        {
            Services.IProxyCoreService proxyService = ServiceProvider.GetService<Services.IProxyCoreService>();
            proxyService.StopCore();
            Services.ISystemProxyService systemProxyService = ServiceProvider.GetService<Services.ISystemProxyService>();
            systemProxyService.DisableSystemProxy();
            base.OnExit(e);
        }
    }
}
