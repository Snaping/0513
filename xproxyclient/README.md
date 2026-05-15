
# XProxyClient - 跨平台代理协议图形客户端

## 项目简介

XProxyClient 是一款基于 WPF 框架开发的 Windows 原生代理客户端，为 Xray、sing-box 等代理内核提供了易于使用的图形界面。支持几乎所有主流代理协议，包括 VMess、VLESS、Shadowsocks、Trojan、Hysteria2、TUIC 和 WireGuard 等。

## 功能特性

### 多协议支持
- **VMess**: 支持 TCP、WebSocket、HTTP/2、gRPC、QUIC、KCP 等传输方式
- **VLESS**: 支持 XTLS 流控
- **Shadowsocks**: 支持多种加密方式
- **Trojan**: 支持 TLS 伪装
- **Hysteria2**: 基于 QUIC 的高速代理协议
- **TUIC**: 基于 QUIC 的轻量级代理协议
- **WireGuard**: 现代 VPN 协议
- **Socks5 / HTTP**: 基础代理协议

### 内核支持
- Xray
- sing-box

### 核心功能
1. **服务器管理**
   - 添加、编辑、删除服务器配置
   - 支持通过分享链接快速导入
   - 延迟测试

2. **订阅管理**
   - 支持订阅链接导入
   - 自动更新订阅服务器列表
   - 多订阅源管理

3. **连接控制**
   - 一键连接/断开
   - 实时连接状态显示
   - 系统代理自动配置

4. **系统代理**
   - 全局模式
   - PAC 模式
   - 关闭系统代理

## 项目结构

```
XProxyClient/
├── XProxyClient.sln          # 解决方案文件
├── XProxyClient/              # 主项目目录
│   ├── XProxyClient.csproj    # 项目文件
│   ├── App.xaml              # 应用程序入口
│   ├── App.xaml.cs
│   ├── Models/               # 数据模型
│   │   ├── ProxyType.cs      # 枚举类型定义
│   │   └── ServerConfig.cs   # 服务器配置模型
│   ├── Services/             # 服务层
│   │   ├── IConfigService.cs       # 配置服务接口
│   │   ├── ConfigService.cs        # 配置服务实现
│   │   ├── IProxyCoreService.cs    # 代理内核服务接口
│   │   ├── ProxyCoreService.cs     # 代理内核服务实现
│   │   ├── ISystemProxyService.cs  # 系统代理服务接口
│   │   ├── SystemProxyService.cs   # 系统代理服务实现
│   │   ├── ISubscriptionService.cs # 订阅服务接口
│   │   └── SubscriptionService.cs  # 订阅服务实现
│   └── Views/                # 视图层
│       ├── MainWindow.xaml           # 主窗口
│       ├── MainWindow.xaml.cs
│       ├── ServerEditWindow.xaml     # 服务器编辑窗口
│       ├── ServerEditWindow.xaml.cs
│       ├── SubscriptionWindow.xaml   # 订阅管理窗口
│       ├── SubscriptionWindow.xaml.cs
│       ├── SettingsWindow.xaml       # 设置窗口
│       └── SettingsWindow.xaml.cs
└── README.md                  # 使用文档
```

## 快速开始

### 环境要求
- Windows 10/11
- .NET 6.0 或更高版本
- Xray / sing-box 内核程序

### 编译运行

1. 克隆或下载项目
2. 使用 Visual Studio 2022 打开 `XProxyClient.sln`
3. 还原 NuGet 包
4. 编译并运行

### 安装内核

1. 创建 `Core` 目录于程序运行目录下
2. 下载 Xray 内核并命名为 `xray.exe` 放入 Core 目录
3. 或下载 sing-box 内核并命名为 `sing-box.exe` 放入 Core 目录

### 配置使用

#### 添加服务器
1. 点击主界面右上角的 "+" 按钮
2. 可以直接粘贴分享链接并点击"解析"
3. 或手动填写服务器信息：
   - 服务器名称
   - 选择协议类型
   - 填写服务器地址和端口
   - 根据协议类型填写相应配置（UUID、密码等）
   - 配置传输协议和 TLS 设置

#### 导入订阅
1. 点击主界面右上角的下载图标
2. 点击"添加订阅"
3. 填写订阅名称和订阅链接
4. 保存后系统会自动更新订阅的服务器列表

#### 连接服务器
1. 在左侧服务器列表中选择要连接的服务器
2. 点击右侧的"连接"按钮
3. 连接成功后状态显示为"已连接"

#### 系统代理设置
1. 在底部状态栏右侧的"系统代理"下拉框中选择模式
   - **关闭**: 不设置系统代理
   - **全局**: 所有流量走代理
   - **PAC**: 使用 PAC 自动配置（需要内核支持）

## 技术架构

### 依赖框架
- **WPF**: Windows 原生 UI 框架
- **MaterialDesignInXamlToolkit**: Material Design 风格 UI 组件
- **Microsoft.Extensions.DependencyInjection**: 依赖注入容器
- **Newtonsoft.Json**: JSON 序列化库

### 架构设计
项目采用 MVVM 架构模式：
- **Models**: 数据模型，包含服务器配置、应用配置等
- **Services**: 业务逻辑层，处理内核管理、系统代理等核心功能
- **Views**: 视图层，负责用户界面展示和交互

## 内核配置生成

### Xray 配置
应用会根据所选服务器自动生成 Xray 兼容的 JSON 配置文件，包含：
- inbound: SOCKS5 和 HTTP 代理入口
- outbound: 根据服务器配置生成的出口
- routing: 路由规则
- dns: DNS 配置

### sing-box 配置
同样支持生成 sing-box 兼容的 JSON 配置文件。

## 数据存储

应用配置存储于：
```
%AppData%\XProxyClient\config.json
```

包含：
- 服务器配置列表
- 订阅配置列表
- 用户偏好设置
- 系统代理模式

## 常见问题

### Q: 为什么连接失败？
A: 请检查：
1. 服务器配置是否正确
2. 内核程序是否存在于 Core 目录
3. 端口是否被占用
4. 防火墙是否阻止了连接

### Q: 系统代理不生效？
A: 请检查：
1. 确保已成功连接到服务器
2. 在底部状态栏选择正确的代理模式
3. 浏览器的代理设置是否为"使用系统代理"

### Q: 支持哪些分享链接格式？
A: 支持以下格式：
- vmess:// (V2RayN 格式)
- vless://
- ss:// (Shadowsocks SIP002)
- trojan://
- hysteria2://
- tuic://
- wireguard://

## 开发计划

- [ ] 托盘图标和菜单
- [ ] 流量统计显示
- [ ] 路由规则配置
- [ ] PAC 配置编辑
- [ ] 连接日志查看
- [ ] 节点测速和排序
- [ ] 主题切换
- [ ] 多语言支持
- [ ] 自动更新

## 许可证

本项目仅供学习交流使用。

## 致谢

- Xray 项目
- sing-box 项目
- MaterialDesignInXamlToolkit
- 所有开源贡献者
