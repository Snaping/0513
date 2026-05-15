using System;
using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace XProxyClient.Models
{
    public class ServerConfig : INotifyPropertyChanged
    {
        private string _id = Guid.NewGuid().ToString();
        private string _name = string.Empty;
        private ProxyType _type = ProxyType.VMess;
        private string _address = string.Empty;
        private int _port = 0;
        private string _uuid = string.Empty;
        private string _alterId = string.Empty;
        private string _security = string.Empty;
        private string _encryption = string.Empty;
        private string _flow = string.Empty;
        private string _password = string.Empty;
        private string _network = string.Empty;
        private string _headerType = string.Empty;
        private string _host = string.Empty;
        private string _path = string.Empty;
        private string _tls = string.Empty;
        private string _sni = string.Empty;
        private string _alpn = string.Empty;
        private bool _allowInsecure = false;
        private string _username = string.Empty;
        private string _privateKey = string.Empty;
        private string _peerPublicKey = string.Empty;
        private string _subscriptionId = string.Empty;

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

        public ProxyType Type
        {
            get { return _type; }
            set { _type = value; OnPropertyChanged(); }
        }

        public string Address
        {
            get { return _address; }
            set { _address = value; OnPropertyChanged(); }
        }

        public int Port
        {
            get { return _port; }
            set { _port = value; OnPropertyChanged(); }
        }

        public string Uuid
        {
            get { return _uuid; }
            set { _uuid = value; OnPropertyChanged(); }
        }

        public string AlterId
        {
            get { return _alterId; }
            set { _alterId = value; OnPropertyChanged(); }
        }

        public string Security
        {
            get { return _security; }
            set { _security = value; OnPropertyChanged(); }
        }

        public string Encryption
        {
            get { return _encryption; }
            set { _encryption = value; OnPropertyChanged(); }
        }

        public string Flow
        {
            get { return _flow; }
            set { _flow = value; OnPropertyChanged(); }
        }

        public string Password
        {
            get { return _password; }
            set { _password = value; OnPropertyChanged(); }
        }

        public string Network
        {
            get { return _network; }
            set { _network = value; OnPropertyChanged(); }
        }

        public string HeaderType
        {
            get { return _headerType; }
            set { _headerType = value; OnPropertyChanged(); }
        }

        public string Host
        {
            get { return _host; }
            set { _host = value; OnPropertyChanged(); }
        }

        public string Path
        {
            get { return _path; }
            set { _path = value; OnPropertyChanged(); }
        }

        public string Tls
        {
            get { return _tls; }
            set { _tls = value; OnPropertyChanged(); }
        }

        public string Sni
        {
            get { return _sni; }
            set { _sni = value; OnPropertyChanged(); }
        }

        public string Alpn
        {
            get { return _alpn; }
            set { _alpn = value; OnPropertyChanged(); }
        }

        public bool AllowInsecure
        {
            get { return _allowInsecure; }
            set { _allowInsecure = value; OnPropertyChanged(); }
        }

        public string Username
        {
            get { return _username; }
            set { _username = value; OnPropertyChanged(); }
        }

        public string PrivateKey
        {
            get { return _privateKey; }
            set { _privateKey = value; OnPropertyChanged(); }
        }

        public string PeerPublicKey
        {
            get { return _peerPublicKey; }
            set { _peerPublicKey = value; OnPropertyChanged(); }
        }

        public string SubscriptionId
        {
            get { return _subscriptionId; }
            set { _subscriptionId = value; OnPropertyChanged(); }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
