using System;
using System.Windows;
using System.Windows.Controls;
using Microsoft.Extensions.DependencyInjection;
using XProxyClient.Models;
using XProxyClient.Services;

namespace XProxyClient.Views
{
    public partial class SettingsWindow : Window
    {
        private readonly IConfigService _configService;

        public SettingsWindow()
        {
            InitializeComponent();

            _configService = App.ServiceProvider.GetRequiredService<IConfigService>();

            InitializeCoreTypes();
            InitializeSystemProxyModes();
            LoadSettings();
        }

        private void InitializeCoreTypes()
        {
            CoreTypeComboBox.Items.Add(CoreType.Xray);
            CoreTypeComboBox.Items.Add(CoreType.SingBox);
        }

        private void InitializeSystemProxyModes()
        {
            SystemProxyModeComboBox.Items.Add(SystemProxyMode.Off);
            SystemProxyModeComboBox.Items.Add(SystemProxyMode.Global);
            SystemProxyModeComboBox.Items.Add(SystemProxyMode.PAC);
        }

        private void LoadSettings()
        {
            AppConfig config = _configService.GetConfig();

            CoreTypeComboBox.SelectedItem = config.CoreType;
            SocksPortTextBox.Text = config.SocksPort.ToString();
            HttpPortTextBox.Text = config.HttpPort.ToString();
            SystemProxyModeComboBox.SelectedItem = config.SystemProxyMode;
            AutoConnectCheckBox.IsChecked = config.AutoConnect;
            AllowInsecureCheckBox.IsChecked = config.AllowInsecure;
            LogLevelTextBox.Text = config.LogLevel;
            XrayPathTextBox.Text = config.XrayPath;
            SingBoxPathTextBox.Text = config.SingBoxPath;
        }

        private void SaveButton_Click(object sender, RoutedEventArgs e)
        {
            if (!int.TryParse(SocksPortTextBox.Text, out int socksPort) || socksPort <= 0 || socksPort > 65535)
            {
                MessageBox.Show("请输入有效的Socks端口号", "提示", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }

            if (!int.TryParse(HttpPortTextBox.Text, out int httpPort) || httpPort <= 0 || httpPort > 65535)
            {
                MessageBox.Show("请输入有效的HTTP端口号", "提示", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }

            AppConfig config = _configService.GetConfig();

            config.CoreType = (CoreType)CoreTypeComboBox.SelectedItem;
            config.SocksPort = socksPort;
            config.HttpPort = httpPort;
            config.SystemProxyMode = (SystemProxyMode)SystemProxyModeComboBox.SelectedItem;
            config.AutoConnect = AutoConnectCheckBox.IsChecked ?? false;
            config.AllowInsecure = AllowInsecureCheckBox.IsChecked ?? false;
            config.LogLevel = LogLevelTextBox.Text;
            config.XrayPath = XrayPathTextBox.Text;
            config.SingBoxPath = SingBoxPathTextBox.Text;

            _configService.SaveConfig();

            MessageBox.Show("设置已保存", "提示", MessageBoxButton.OK, MessageBoxImage.Information);
            DialogResult = true;
            Close();
        }

        private void CancelButton_Click(object sender, RoutedEventArgs e)
        {
            DialogResult = false;
            Close();
        }

        private void ResetButton_Click(object sender, RoutedEventArgs e)
        {
            MessageBoxResult result = MessageBox.Show("确定要重置所有设置吗?", "确认", MessageBoxButton.YesNo, MessageBoxImage.Question);
            if (result == MessageBoxResult.Yes)
            {
                _configService.ResetConfig();
                LoadSettings();
                MessageBox.Show("设置已重置", "提示", MessageBoxButton.OK, MessageBoxImage.Information);
            }
        }
    }
}
