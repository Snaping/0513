# AI Editor Pro

基于 Electron 和 Monaco Editor（VSCode 核心）构建的 AI 驱动复合代码编辑器。

## ✨ 功能特性

### 📝 编辑器核心
- ✅ Monaco Editor（VSCode 同款编辑器）
- ✅ 语法高亮、代码补全
- ✅ 多标签页支持
- ✅ 文件树资源管理器
- ✅ 深色主题

### 🔍 文件搜索
- ✅ Ctrl+P 快速文件搜索
- ✅ 模糊搜索匹配
- ✅ 文件夹扫描

### 🤖 AI 助手
- ✅ 独立 AI 面板（Ctrl+Shift+A）
- ✅ 多 AI 提供商支持
  - OpenAI (GPT-4)
  - Anthropic (Claude)
  - Google (Gemini)
  - Ollama (本地模型)
- ✅ API Key 配置
- ✅ AI 聊天界面
- ✅ 代码生成和解释

### 🔌 插件管理
- ✅ AI 插件列表
- ✅ 插件启用/禁用
- ✅ 支持主流 AI 插件

### 🎯 快捷键
| 功能 | 快捷键 |
|------|--------|
| 新建文件 | Ctrl + N |
| 打开文件 | Ctrl + O |
| 保存 | Ctrl + S |
| 文件搜索 | Ctrl + P |
| 打开 AI 助手 | Ctrl + Shift + A |
| AI 代码补全 | Ctrl + Space |
| 打开设置 | Ctrl + , |

## 🚀 快速开始

### 开发模式

```bash
# 安装依赖
npm install

# 运行编辑器
npm start

# 带开发者工具运行
npm run dev
```

### 打包发布

```bash
# 构建 Windows 安装包
npm run build:win

# 构建 Windows 便携版
npm run build:portable

# 构建所有平台
npm run build
```

## 📁 项目结构

```
custom-ai-editor/
├── src/
│   ├── main.js          # Electron 主进程
│   ├── renderer.js      # 渲染进程（编辑器逻辑）
│   └── index.html       # 编辑器界面
├── assets/              # 资源文件（图标等）
├── dist/                # 打包输出（自动生成）
├── package.json         # 项目配置
└── README.md
```

## 🛠️ 技术栈

- **Electron 30** - 跨平台桌面应用框架
- **Monaco Editor** - VSCode 核心编辑器
- **Fuse.js** - 轻量级模糊搜索
- **Axios** - HTTP 客户端（AI API 调用）

## 🎨 界面预览

- **左侧活动栏**：资源管理器、搜索、插件、AI 助手
- **侧边栏**：文件树、插件列表
- **主编辑区**：Monaco Editor、多标签页
- **右侧面板**：AI 助手聊天界面
- **底部状态栏**：编码、语言、行号等

## 🔧 配置 AI 提供商

1. 打开 AI 助手面板（Ctrl+Shift+A）
2. 选择你的 AI 提供商
3. 输入对应的 API Key
4. 配置模型名称（可选）
5. 开始使用 AI 聊天功能

## 📦 构建输出

构建完成后，可在 `dist/` 目录找到：
- `AI Editor Pro Setup 1.0.0.exe` - NSIS 安装程序
- `AI Editor Pro 1.0.0.exe` - 便携版本（无需安装）

## 🤝 贡献

欢迎提交 Issue 和 Pull Request！

## 📄 许可证

MIT License
