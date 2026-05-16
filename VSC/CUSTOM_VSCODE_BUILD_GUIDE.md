# 自定义 VSCode 构建指南 - 集成 AI Assistant

本指南将帮助你基于开源 VSCode 构建一个集成了 AI Assistant Manager 的自定义版本，并打包为 Windows 可执行文件。

## 前置要求

### 系统要求
- Windows 10/11 (64位)
- 至少 16GB RAM
- 至少 50GB 可用磁盘空间

### 必需工具
1. **Node.js** - 20.x 或更高版本
   ```bash
   node --version
   npm --version
   ```

2. **Python** - 3.8 或更高版本
   ```bash
   python --version
   ```

3. **Visual Studio 2022 Build Tools** - 必需组件:
   - Desktop Development with C++
   - MSVC v143 - VS 2022 C++ x64/x86 Spectre-mitigated libs
   - C++ ATL for latest build tools with Spectre Mitigations

4. **Git** - 最新版本
   ```bash
   git --version
   ```

## 步骤一: 克隆 VSCode 源码

```bash
# 克隆 VSCode 仓库（深度克隆以节省时间和空间）
git clone --depth 1 https://github.com/microsoft/vscode.git vscode-custom
cd vscode-custom

# 如果你需要特定版本
git checkout tags/1.91.0 -b build-branch
```

## 步骤二: 安装依赖

```bash
# 设置 npm 镜像（可选，国内用户推荐）
npm config set registry https://registry.npmmirror.com

# 安装依赖
npm install

# 验证安装
npm list --depth=0
```

## 步骤三: 集成 AI Assistant 扩展

### 方法一: 作为内置扩展
```bash
# 复制 AI Assistant 扩展到 VSCode 内置扩展目录
cp -r ../ai-assistant-extension extensions/ai-assistant-manager

# 修改 product.json 添加扩展信息
# 编辑 product.json, 在 builtInExtensions 数组中添加:
# {
#   "name": "ai-assistant-manager",
#   "version": "1.0.0",
#   "repo": "https://github.com/your-repo/ai-assistant-manager"
# }
```

### 方法二: 打包为 VSIX 并预装
```bash
# 安装 VSCode 扩展打包工具
npm install -g @vscode/vsce

# 打包扩展
cd ../ai-assistant-extension
vsce package

# 将生成的 .vsix 文件复制到 VSCode 的 resources/extensions 目录
```

## 步骤四: 编译 VSCode

```bash
# 首次完整编译
npm run compile

# 或者只编译核心组件
npm run gulp compile-client

# 启动开发版本进行测试
./scripts/code.bat
```

## 步骤五: 测试功能

启动开发版本后，验证以下功能：

1. ✅ AI Assistant 图标出现在活动栏
2. ✅ 可以查看 AI 插件列表
3. ✅ 可以切换环境
4. ✅ 配置面板正常工作
5. ✅ 文件搜索功能正常 (Ctrl+P)
6. ✅ 扩展市场可以访问

## 步骤六: 打包为 Windows 可执行文件

### 方法一: 官方打包脚本
```bash
# 设置环境变量
set NODE_ENV=production

# 打包 Windows 版本
npm run gulp vscode-win32-x64

# 或者完整打包
npm run package-win
```

### 方法二: 使用 Electron Builder
创建 `electron-builder.json` 配置文件:
```json
{
  "appId": "com.custom.vscode-ai",
  "productName": "VSCode AI Edition",
  "directories": {
    "output": "dist"
  },
  "win": {
    "target": [
      {
        "target": "nsis",
        "arch": ["x64"]
      },
      {
        "target": "portable",
        "arch": ["x64"]
      }
    ],
    "icon": "resources/win32/code.ico"
  },
  "nsis": {
    "oneClick": false,
    "allowToChangeInstallationDirectory": true,
    "createDesktopShortcut": true,
    "createStartMenuShortcut": true
  }
}
```

```bash
# 安装 electron-builder
npm install -g electron-builder

# 打包
electron-builder --win
```

## 步骤七: 自定义品牌

### 修改应用名称
编辑 `product.json`:
```json
{
  "nameShort": "VSCode AI",
  "nameLong": "Visual Studio Code AI Edition",
  "applicationName": "vscode-ai",
  "win32MutexName": "vscode-ai",
  "win32DirName": "VSCode AI",
  "win32RegValueName": "vscode-ai",
  "win32AppId": "{{E34002BB-1E09-4A6F-9E0F-7FBF58A3E498}",
  "win32UserAppId": "{{B0869C47-7780-48D4-B65D-4FD3E27D9F20}}"
}
```

### 自定义图标
1. 准备 256x256 的 ICO 文件
2. 替换 `resources/win32/code.ico`
3. 替换 `resources/linux/code.png`
4. 替换 `resources/darwin/code.icns`

### 自定义欢迎页面
编辑 `src/vs/workbench/browser/parts/editor/welcomePage.ts`

## 步骤八: 优化构建

### 减少构建体积
```bash
# 清理不需要的文件
npm run gulp clean-extension

# 只包含必要的内置扩展
# 编辑 build/builtin/extensions.json 移除不需要的扩展
```

### 启用优化
```bash
# 生产环境构建
set NODE_ENV=production
npm run gulp compile-build

# 压缩
npm run gulp minify-vscode
```

## 常见问题解决

### 问题 1: Spectre 库缺失
```
错误 MSB8040: 此项目需要缓解了 Spectre 漏洞的库
```
**解决**: 在 Visual Studio Installer 中安装 Spectre 缓解库

### 问题 2: Node.js 版本不兼容
```
Error: The engine "node" is incompatible with this module
```
**解决**: 使用 nvm 安装正确的 Node.js 版本
```bash
nvm install 20
nvm use 20
```

### 问题 3: 内存不足
```
FATAL ERROR: Ineffective mark-compacts near heap limit
```
**解决**: 增加 Node.js 内存限制
```bash
set NODE_OPTIONS=--max_old_space_size=8192
```

### 问题 4: 编译后扩展不加载
**解决**: 检查扩展的 package.json 是否符合 VSCode 扩展规范

## AI 插件市场推荐配置

在你的自定义 VSCode 中，预装以下 AI 插件:

```json
{
  "recommendedExtensions": [
    "github.copilot",
    "github.copilot-chat",
    "continue.continue",
    "codeium.codeium",
    "sourcegraph.cody-ai"
  ]
}
```

## 发布和分发

### 生成安装包
```bash
# 生成 NSIS 安装程序
npm run gulp vscode-win32-x64-archive

# 生成 ZIP 便携版
npm run gulp vscode-win32-x64-archive
```

### 校验和签名
```bash
# 生成 SHA256 校验和
certutil -hashfile "VSCode-AI-Setup.exe" SHA256

# 代码签名（需要证书）
signtool sign /f your-certificate.pfx /p password "VSCode-AI-Setup.exe"
```

## 项目结构

```
vscode-custom/
├── src/                    # 核心源码
├── extensions/             # 内置扩展
│   └── ai-assistant-manager/  # 我们的 AI 助手扩展
├── resources/              # 资源文件（图标、欢迎页）
├── build/                  # 构建脚本
├── out/                    # 编译输出
└── .build/                 # 打包输出
    └── electron/           # Electron 打包结果
        └── VSCode-win32-x64/  # 最终可执行文件目录
```

## 下一步

1. 添加更多 AI 功能集成
2. 自定义主题和配色方案
3. 添加团队协作功能
4. 集成更多本地 LLM 支持
5. 添加代码审查 AI 助手

## 参考资源

- VSCode 官方 Wiki: https://github.com/microsoft/vscode/wiki
- VSCode 扩展 API: https://code.visualstudio.com/api
- Electron 构建: https://www.electron.build/

---

祝你构建顺利！🚀
