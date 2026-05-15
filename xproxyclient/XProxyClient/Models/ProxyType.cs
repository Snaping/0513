namespace XProxyClient.Models
{
    public enum ProxyType
    {
        VMess,
        VLESS,
        Shadowsocks,
        Trojan,
        Hysteria2,
        TUIC,
        WireGuard,
        Socks5,
        HTTP
    }

    public enum CoreType
    {
        Xray,
        SingBox
    }

    public enum ConnectionStatus
    {
        Disconnected,
        Connecting,
        Connected,
        Failed
    }

    public enum SystemProxyMode
    {
        Off,
        Global,
        PAC
    }
}
