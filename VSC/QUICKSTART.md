# 快速入门指南

## 项目概览

你现在有两个主要项目:

1. **`ai-assistant-extension/`** - AI Assistant Manager VSCode 扩展
   - 已完成开发和编译
   - 可以在 VSCode 中直接运行和测试

2. **`vscode-core/`** - VSCode 源代码
   - 已克隆到本地
   - 可用于构建自定义版本

---

## 快速开始: 运行 AI Assistant 扩展

### 方法 1: 在 VSCode 中调试运行

1. 用 VSCode 打开 `ai-assistant-extension` 文件夹:
```bash
code ai-assistant-extension
```

2. 按 `F5` 键启动扩展开发主机 (Extension Development Host)

3. 在新窗口中:
   - 点击活动栏的 "AI Assistant" 图标
   - 或按 `Ctrl+Shift+A` 快捷键

### 方法 2: 打包为 VSIX 安装

```bash
cd ai-assistant-extension

# 安装 vsce 打包工具
npm install -g @vscode/vsce

# 打包
vsce package

# 安装生成的 .vsix 文件
# 在 VSCode 中: 扩展面板 → ... → 从 VSIX 安装...
```

---

## 扩展功能介绍

### 🔌 AI 插件管理
- 浏览 18+ 个流行 AI 编码助手插件
- 一键安装/卸载
- 按类别过滤（编码、聊天、重构、测试）

**支持的插件列表:**
- GitHub Copilot & Copilot Chat
- Continue (开源 AI 助手)
- Codeium
- Cody AI by Sourcegraph
- Tabnine
- Sourcery (AI 重构)
- Amazon Q
- CodeGPT
- Genie AI
- Cursor
- Bito
- 等等...

### 🌍 环境切换
- OpenAI (GPT-4o, GPT-4, GPT-3.5)
- Anthropic (Claude 3 Opus/Sonnet/Haiku)
- Google (Gemini Pro/Ultra)
- Ollama (本地模型: Llama 3, Mistral 等)
- Azure OpenAI

### ⚙️ 配置管理
- 集中管理所有 API 密钥
- 模型参数配置 (temperature, max tokens)
- 代码完成设置
- 聊天功能定制
- 配置导入/导出

---

## 可用命令

| 命令 | 快捷键 | 说明 |
|------|--------|------|
| `AI Assistant: Open Manager Panel` | `Ctrl+Shift+A` | 打开 AI 助手面板 |
| `AI Assistant: Install Plugin` | - | 安装 AI 插件 |
| `AI Assistant: Switch Environment` | - | 切换 AI 环境 |
| `AI Assistant: Configure AI Model` | - | 配置模型设置 |

---

## 项目目录结构

```
VSC/
├── ai-assistant-extension/          # AI 助手扩展项目
│   ├── src/
│   │   ├── extension.ts            # 扩展入口
│   │   ├── aiPluginManager.ts      # 插件管理器
│   │   ├── environmentManager.ts   # 环境管理器
│   │   ├── configManager.ts        # 配置管理器
│   │   └── views.ts                # 视图组件
│   ├── resources/
│   │   └── robot-icon.svg          # 扩展图标
│   ├── out/                        # 编译输出 (已生成)
│   ├── package.json
│   ├── tsconfig.json
│   └── README.md
│
├── vscode-core/                    # VSCode 源代码
│   ├── src/                        # 核心源码
│   ├── extensions/                 # 内置扩展
│   ├── resources/                  # 资源文件
│   └── ...
│
├── CUSTOM_VSCODE_BUILD_GUIDE.md    # 完整构建指南
└── QUICKSTART.md                   # 本文档
```

---

## 下一步建议

### 1. 测试扩展功能
- [ ] 按 F5 启动扩展
- [ ] 测试插件列表显示
- [ ] 测试环境切换
- [ ] 测试配置设置

### 2. 开发增强功能
- [ ] 添加 AI 聊天界面
- [ ] 集成本地 LLM (Ollama)
- [ ] 添加代码解释功能
- [ ] 添加智能重构建议

### 3. 构建自定义 VSCode
- [ ] 参阅 `CUSTOM_VSCODE_BUILD_GUIDE.md`
- [ ] 安装 Visual Studio Build Tools (含 Spectre 库)
- [ ] 将扩展集成到 VSCode 源码
- [ ] 构建自定义版本

### 4. 打包分发
- [ ] 生成 VSIX 包
- [ ] 构建 Windows 可执行文件
- [ ] 创建安装程序

---

## 常见问题

### Q: 如何调试扩展?
A: 打开扩展项目，按 F5 启动调试模式，会打开一个新的 VSCode 窗口加载扩展。

### Q: 为什么 vscode-core npm install 失败?
A: VSCode 构建需要 Visual Studio Build Tools 和 Spectre 缓解库。参阅构建指南中的详细说明。

### Q: 如何添加更多 AI 插件?
A: 编辑 `src/aiPluginManager.ts` 中的 `availablePlugins` 数组。

### Q: 如何支持更多 AI 提供商?
A: 在 `src/environmentManager.ts` 中添加新的 provider 配置。

---

## 技术栈

- **TypeScript** - 主要开发语言
- **VSCode Extension API** - 扩展框架
- **Node.js** - 运行环境
- **Electron** - 桌面应用框架 (VSCode 底层)

---

## 参考资源

- VSCode Extension API: https://code.visualstudio.com/api
- VSCode 源码: https://github.com/microsoft/vscode
- VSCE 打包工具: https://github.com/microsoft/vscode-vsce

---

现在开始探索你的 AI 助手扩展吧！ 🚀
