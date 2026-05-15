using System;
using System.Windows;
using Microsoft.Extensions.DependencyInjection;
using XProxyClient.Models;
using XProxyClient.Services;

namespace XProxyClient.Views
{
    public partial class ServerEditWindow : Window
    {
        private readonly IConfigService _configService;
        private readonly ISubscriptionService _subscriptionService;
        private ServerConfig _server;
        private bool _isEditMode;

        public ServerEditWindow()
        {
            InitializeComponent();

            _configService = App.ServiceProvider.GetRequiredService<IConfigService>();
            _subscriptionService = App.ServiceProvider.GetRequiredService<ISubscriptionService>();

            _server = new ServerConfig();
            _isEditMode = false;
            Title = "添加服务器";

            InitializeProxyTypes();
        }

        public ServerEditWindow(ServerConfig server)
        {
            InitializeComponent();

            _configService = App.ServiceProvider.GetRequiredService<IConfigService>();
            _subscriptionService = App.ServiceProvider.GetRequiredService<ISubscriptionService>();

            _server = server;
            _isEditMode = true;
            Title = "编辑服务器";

            InitializeProxyTypes();
            LoadServerData();
        }

        private void InitializeProxyTypes()
        {
            ProxyTypeComboBox.Items.Add(ProxyType.VMess);
            ProxyTypeComboBox.Items.Add(ProxyType.VLESS);
            ProxyTypeComboBox.Items.Add(ProxyType.Shadowsocks);
            ProxyTypeComboBox.Items.Add(ProxyType.Trojan);
            ProxyTypeComboBox.Items.Add(ProxyType.Hysteria2);
            ProxyTypeComboBox.Items.Add(ProxyType.TUIC);
            ProxyTypeComboBox.Items.Add(ProxyType.WireGuard);
            ProxyTypeComboBox.Items.Add(ProxyType.Socks5);
            ProxyTypeComboBox.Items.Add(ProxyType.HTTP);

            ProxyTypeComboBox.SelectedItem = _server.Type;
        }

        private void LoadServerData()
        {
            NameTextBox.Text = _server.Name;
            AddressTextBox.Text = _server.Address;
            PortTextBox.Text = _server.Port.ToString();
            UuidTextBox.Text = _server.Uuid;
            PasswordTextBox.Text = _server.Password;
            EncryptionTextBox.Text = _server.Encryption;
            NetworkTextBox.Text = _server.Network;
            TlsTextBox.Text = _server.Tls;
        }

        private void SaveButton_Click(object sender, RoutedEventArgs e)
        {
            if (string.IsNullOrWhiteSpace(NameTextBox.Text))
            {
                MessageBox.Show("请输入服务器名称", "提示", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }

            if (string.IsNullOrWhiteSpace(AddressTextBox.Text))
            {
                MessageBox.Show("请输入服务器地址", "提示", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }

            if (!int.TryParse(PortTextBox.Text, out int port) || port <= 0 || port > 65535)
            {
                MessageBox.Show("请输入有效的端口号", "提示", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }

            _server.Name = NameTextBox.Text;
            _server.Type = (ProxyType)ProxyTypeComboBox.SelectedItem;
            _server.Address = AddressTextBox.Text;
            _server.Port = port;
            _server.Uuid = UuidTextBox.Text;
            _server.Password = PasswordTextBox.Text;
            _server.Encryption = EncryptionTextBox.Text;
            _server.Network = NetworkTextBox.Text;
            _server.Tls = TlsTextBox.Text;

            if (_isEditMode)
            {
                _configService.UpdateServer(_server);
            }
            else
            {
                _configService.AddServer(_server);
            }

            DialogResult = true;
            Close();
        }

        private void CancelButton_Click(object sender, RoutedEventArgs e)
        {
            DialogResult = false;
            Close();
        }

        private void ParseLinkButton_Click(object sender, RoutedEventArgs e)
        {
            string link = LinkTextBox.Text.Trim();
            if (string.IsNullOrWhiteSpace(link))
            {
                MessageBox.Show("请输入分享链接", "提示", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }

            string result = _subscriptionService.ParseShareLink(link);
            if (result != null)
            {
                MessageBox.Show("解析成功: " + result, "提示", MessageBoxButton.OK, MessageBoxImage.Information);
            }
            else
            {
                MessageBox.Show("解析失败，不支持的链接格式", "错误", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }
    }
}
