using System;
using System.Windows;
using System.Windows.Controls;
using Microsoft.Extensions.DependencyInjection;
using XProxyClient.Models;
using XProxyClient.Services;

namespace XProxyClient.Views
{
    public partial class SubscriptionWindow : Window
    {
        private readonly IConfigService _configService;
        private readonly ISubscriptionService _subscriptionService;

        public SubscriptionWindow()
        {
            InitializeComponent();

            _configService = App.ServiceProvider.GetRequiredService<IConfigService>();
            _subscriptionService = App.ServiceProvider.GetRequiredService<ISubscriptionService>();

            LoadSubscriptions();
        }

        private void LoadSubscriptions()
        {
            SubscriptionListBox.Items.Clear();
            foreach (SubscriptionConfig subscription in _configService.GetAllSubscriptions())
            {
                SubscriptionListBox.Items.Add(subscription);
            }
        }

        private void AddSubscriptionButton_Click(object sender, RoutedEventArgs e)
        {
            string name = NameTextBox.Text.Trim();
            string url = UrlTextBox.Text.Trim();

            if (string.IsNullOrWhiteSpace(name))
            {
                MessageBox.Show("请输入订阅名称", "提示", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }

            if (string.IsNullOrWhiteSpace(url))
            {
                MessageBox.Show("请输入订阅地址", "提示", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }

            SubscriptionConfig subscription = new SubscriptionConfig();
            subscription.Name = name;
            subscription.Url = url;
            subscription.LastUpdate = DateTime.Now;
            subscription.ServerCount = 0;

            _configService.AddSubscription(subscription);
            LoadSubscriptions();

            NameTextBox.Text = string.Empty;
            UrlTextBox.Text = string.Empty;
        }

        private void DeleteSubscriptionButton_Click(object sender, RoutedEventArgs e)
        {
            if (SubscriptionListBox.SelectedItem == null)
            {
                MessageBox.Show("请先选择一个订阅", "提示", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }

            MessageBoxResult result = MessageBox.Show("确定要删除这个订阅吗?", "确认", MessageBoxButton.YesNo, MessageBoxImage.Question);
            if (result == MessageBoxResult.Yes)
            {
                SubscriptionConfig subscription = (SubscriptionConfig)SubscriptionListBox.SelectedItem;
                _configService.RemoveSubscription(subscription.Id);
                _configService.GetConfig().Servers.RemoveAll(s => s.SubscriptionId == subscription.Id);
                LoadSubscriptions();
            }
        }

        private async void UpdateSubscriptionButton_Click(object sender, RoutedEventArgs e)
        {
            if (SubscriptionListBox.SelectedItem == null)
            {
                MessageBox.Show("请先选择一个订阅", "提示", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }

            SubscriptionConfig subscription = (SubscriptionConfig)SubscriptionListBox.SelectedItem;

            try
            {
                UpdateSubscriptionButton.IsEnabled = false;
                await _subscriptionService.UpdateSubscription(subscription);
                LoadSubscriptions();
                MessageBox.Show("订阅更新成功", "提示", MessageBoxButton.OK, MessageBoxImage.Information);
            }
            catch (Exception ex)
            {
                MessageBox.Show("订阅更新失败: " + ex.Message, "错误", MessageBoxButton.OK, MessageBoxImage.Error);
            }
            finally
            {
                UpdateSubscriptionButton.IsEnabled = true;
            }
        }

        private async void UpdateAllButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                UpdateAllButton.IsEnabled = false;
                await _subscriptionService.UpdateAllSubscriptions();
                LoadSubscriptions();
                MessageBox.Show("所有订阅更新成功", "提示", MessageBoxButton.OK, MessageBoxImage.Information);
            }
            catch (Exception ex)
            {
                MessageBox.Show("订阅更新失败: " + ex.Message, "错误", MessageBoxButton.OK, MessageBoxImage.Error);
            }
            finally
            {
                UpdateAllButton.IsEnabled = true;
            }
        }

        private void CloseButton_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }
    }
}
