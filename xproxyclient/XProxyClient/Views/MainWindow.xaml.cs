using System;
using System.Windows;
using System.Windows.Controls;
using Microsoft.Extensions.DependencyInjection;
using XProxyClient.Models;
using XProxyClient.Services;
using System.Threading.Tasks;

namespace XProxyClient.Views
{
    public partial class MainWindow : Window
    {
        private readonly IConfigService _configService;
        private readonly IProxyCoreService _proxyCoreService;
        private readonly ISystemProxyService _systemProxyService;
        private readonly ISubscriptionService _subscriptionService;

        public MainWindow()
        {
            InitializeComponent();

            _configService = App.ServiceProvider.GetRequiredService<IConfigService>();
            _proxyCoreService = App.ServiceProvider.GetRequiredService<IProxyCoreService>();
            _systemProxyService = App.ServiceProvider.GetRequiredService<ISystemProxyService>();
            _subscriptionService = App.ServiceProvider.GetRequiredService<ISubscriptionService>();

            LoadServers();
            UpdateConnectionStatus();
        }

        private void LoadServers()
        {
            ServerListBox.Items.Clear();
            foreach (ServerConfig server in _configService.GetAllServers())
            {
                ServerListBox.Items.Add(server);
            }
        }

        private void UpdateConnectionStatus()
        {
            ConnectionStatusText.Text = _proxyCoreService.GetConnectionStatus();
            ConnectButton.IsEnabled = !_proxyCoreService.IsConnected();
            DisconnectButton.IsEnabled = _proxyCoreService.IsConnected();
        }

        private async void ConnectButton_Click(object sender, RoutedEventArgs e)
        {
            if (ServerListBox.SelectedItem == null)
            {
                MessageBox.Show("请先选择一个服务器", "提示", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }

            ServerConfig server = (ServerConfig)ServerListBox.SelectedItem;

            try
            {
                ConnectButton.IsEnabled = false;
                ConnectionStatusText.Text = "正在连接...";

                await _proxyCoreService.StartCore(server);
                _systemProxyService.EnableSystemProxy(_configService.GetConfig().SocksPort, _configService.GetConfig().HttpPort, _configService.GetConfig().SystemProxyMode);

                UpdateConnectionStatus();
            }
            catch (Exception ex)
            {
                MessageBox.Show("连接失败: " + ex.Message, "错误", MessageBoxButton.OK, MessageBoxImage.Error);
                ConnectionStatusText.Text = "连接失败";
                ConnectButton.IsEnabled = true;
            }
        }

        private void DisconnectButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                _proxyCoreService.StopCore();
                _systemProxyService.DisableSystemProxy();
                UpdateConnectionStatus();
            }
            catch (Exception ex)
            {
                MessageBox.Show("断开连接失败: " + ex.Message, "错误", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private async void TestLatencyButton_Click(object sender, RoutedEventArgs e)
        {
            if (ServerListBox.SelectedItem == null)
            {
                MessageBox.Show("请先选择一个服务器", "提示", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }

            ServerConfig server = (ServerConfig)ServerListBox.SelectedItem;

            try
            {
                TestLatencyButton.IsEnabled = false;
                long latency = await _proxyCoreService.TestLatency(server);
                if (latency > 0)
                {
                    MessageBox.Show(string.Format("延迟: {0} ms", latency), "延迟测试", MessageBoxButton.OK, MessageBoxImage.Information);
                }
                else
                {
                    MessageBox.Show("延迟测试失败", "错误", MessageBoxButton.OK, MessageBoxImage.Error);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("延迟测试失败: " + ex.Message, "错误", MessageBoxButton.OK, MessageBoxImage.Error);
            }
            finally
            {
                TestLatencyButton.IsEnabled = true;
            }
        }

        private void AddServerButton_Click(object sender, RoutedEventArgs e)
        {
            ServerEditWindow window = new ServerEditWindow();
            if (window.ShowDialog() == true)
            {
                LoadServers();
            }
        }

        private void EditServerButton_Click(object sender, RoutedEventArgs e)
        {
            if (ServerListBox.SelectedItem == null)
            {
                MessageBox.Show("请先选择一个服务器", "提示", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }

            ServerConfig server = (ServerConfig)ServerListBox.SelectedItem;

            ServerEditWindow window = new ServerEditWindow(server);
            if (window.ShowDialog() == true)
            {
                LoadServers();
            }
        }

        private void DeleteServerButton_Click(object sender, RoutedEventArgs e)
        {
            if (ServerListBox.SelectedItem == null)
            {
                MessageBox.Show("请先选择一个服务器", "提示", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }

            MessageBoxResult result = MessageBox.Show("确定要删除这个服务器吗?", "确认", MessageBoxButton.YesNo, MessageBoxImage.Question);
            if (result == MessageBoxResult.Yes)
            {
                ServerConfig server = (ServerConfig)ServerListBox.SelectedItem;
                _configService.RemoveServer(server.Id);
                LoadServers();
            }
        }

        private void SubscriptionButton_Click(object sender, RoutedEventArgs e)
        {
            SubscriptionWindow window = new SubscriptionWindow();
            window.ShowDialog();
            LoadServers();
        }

        private void SettingsButton_Click(object sender, RoutedEventArgs e)
        {
            SettingsWindow window = new SettingsWindow();
            window.ShowDialog();
        }
    }
}
