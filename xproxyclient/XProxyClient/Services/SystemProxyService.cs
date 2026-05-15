using XProxyClient.Models;
using Microsoft.Win32;
using System;
using System.Runtime.InteropServices;

namespace XProxyClient.Services
{
    public class SystemProxyService : ISystemProxyService
    {
        private const string ProxyKeyPath = @"Software\Microsoft\Windows\CurrentVersion\Internet Settings";

        [DllImport("wininet.dll")]
        private static extern bool InternetSetOption(IntPtr hInternet, int dwOption, IntPtr lpBuffer, int dwBufferLength);
        private const int INTERNET_OPTION_SETTINGS_CHANGED = 39;
        private const int INTERNET_OPTION_REFRESH = 37;

        public void EnableSystemProxy(int socksPort, int httpPort, SystemProxyMode mode)
        {
            try
            {
                using (RegistryKey key = Registry.CurrentUser.OpenSubKey(ProxyKeyPath, true))
                {
                    if (key == null)
                    {
                        return;
                    }

                    if (mode == SystemProxyMode.Global)
                    {
                        string proxyServer = string.Format("127.0.0.1:{0}", httpPort);
                        key.SetValue("ProxyServer", proxyServer);
                        key.SetValue("ProxyEnable", 1);
                        key.SetValue("AutoConfigURL", string.Empty);
                    }
                    else if (mode == SystemProxyMode.PAC)
                    {
                        string pacUrl = string.Format("http://127.0.0.1:{0}/pac", httpPort);
                        key.SetValue("AutoConfigURL", pacUrl);
                        key.SetValue("ProxyEnable", 0);
                        key.SetValue("ProxyServer", string.Empty);
                    }
                }

                RefreshSettings();
            }
            catch
            {
            }
        }

        public void DisableSystemProxy()
        {
            try
            {
                using (RegistryKey key = Registry.CurrentUser.OpenSubKey(ProxyKeyPath, true))
                {
                    if (key == null)
                    {
                        return;
                    }

                    key.SetValue("ProxyEnable", 0);
                    key.SetValue("ProxyServer", string.Empty);
                    key.SetValue("AutoConfigURL", string.Empty);
                }

                RefreshSettings();
            }
            catch
            {
            }
        }

        public SystemProxyMode GetCurrentMode()
        {
            try
            {
                using (RegistryKey key = Registry.CurrentUser.OpenSubKey(ProxyKeyPath, false))
                {
                    if (key == null)
                    {
                        return SystemProxyMode.Off;
                    }

                    int? proxyEnable = key.GetValue("ProxyEnable") as int?;
                    string autoConfigUrl = key.GetValue("AutoConfigURL") as string;

                    if (proxyEnable == 1)
                    {
                        return SystemProxyMode.Global;
                    }
                    if (!string.IsNullOrEmpty(autoConfigUrl))
                    {
                        return SystemProxyMode.PAC;
                    }
                }
            }
            catch
            {
            }
            return SystemProxyMode.Off;
        }

        private void RefreshSettings()
        {
            InternetSetOption(IntPtr.Zero, INTERNET_OPTION_SETTINGS_CHANGED, IntPtr.Zero, 0);
            InternetSetOption(IntPtr.Zero, INTERNET_OPTION_REFRESH, IntPtr.Zero, 0);
        }
    }
}
