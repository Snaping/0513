using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace XProxyClient.Models
{
    public class AppConfig : INotifyPropertyChanged
    {
        private CoreType _coreType = CoreType.Xray;
        private int _socksPort = 10808;
        private int _httpPort = 10809;
        private SystemProxyMode _systemProxyMode = SystemProxyMode.Off;
        private bool _autoConnect = false;
        private bool _allowInsecure = false;
        private string _logLevel = "warning";
        private string _xrayPath = string.Empty;
        private string _singBoxPath = string.Empty;
        private List<ServerConfig> _servers = new List<ServerConfig>();
        private List<SubscriptionConfig> _subscriptions = new List<SubscriptionConfig>();

        public CoreType CoreType
        {
            get { return _coreType; }
            set { _coreType = value; OnPropertyChanged(); }
        }

        public int SocksPort
        {
            get { return _socksPort; }
            set { _socksPort = value; OnPropertyChanged(); }
        }

        public int HttpPort
        {
            get { return _httpPort; }
            set { _httpPort = value; OnPropertyChanged(); }
        }

        public SystemProxyMode SystemProxyMode
        {
            get { return _systemProxyMode; }
            set { _systemProxyMode = value; OnPropertyChanged(); }
        }

        public bool AutoConnect
        {
            get { return _autoConnect; }
            set { _autoConnect = value; OnPropertyChanged(); }
        }

        public bool AllowInsecure
        {
            get { return _allowInsecure; }
            set { _allowInsecure = value; OnPropertyChanged(); }
        }

        public string LogLevel
        {
            get { return _logLevel; }
            set { _logLevel = value; OnPropertyChanged(); }
        }

        public string XrayPath
        {
            get { return _xrayPath; }
            set { _xrayPath = value; OnPropertyChanged(); }
        }

        public string SingBoxPath
        {
            get { return _singBoxPath; }
            set { _singBoxPath = value; OnPropertyChanged(); }
        }

        public List<ServerConfig> Servers
        {
            get { return _servers; }
            set { _servers = value; OnPropertyChanged(); }
        }

        public List<SubscriptionConfig> Subscriptions
        {
            get { return _subscriptions; }
            set { _subscriptions = value; OnPropertyChanged(); }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }

    public class SubscriptionConfig : INotifyPropertyChanged
    {
        private string _id = Guid.NewGuid().ToString();
        private string _name = string.Empty;
        private string _url = string.Empty;
        private DateTime _lastUpdate = DateTime.Now;
        private int _serverCount = 0;

        public string Id
        {
            get { return _id; }
            set { _id = value; OnPropertyChanged(); }
        }

        public string Name
        {
            get { return _name; }
            set { _name = value; OnPropertyChanged(); }
        }

        public string Url
        {
            get { return _url; }
            set { _url = value; OnPropertyChanged(); }
        }

        public DateTime LastUpdate
        {
            get { return _lastUpdate; }
            set { _lastUpdate = value; OnPropertyChanged(); }
        }

        public int ServerCount
        {
            get { return _serverCount; }
            set { _serverCount = value; OnPropertyChanged(); }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
