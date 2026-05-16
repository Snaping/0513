# AI Assistant Manager - VSCode Extension

A unified AI assistant management extension for Visual Studio Code that helps you manage all AI-related plugins, configurations, and environments in one place.

## Features

### 🔌 AI Plugin Management
- Browse and install popular AI coding assistants
- Manage installed plugins from a central location
- Categorized plugin list (coding, chat, refactoring, testing)
- One-click installation and uninstallation

**Supported Plugins:**
- GitHub Copilot & Copilot Chat
- Continue (Open-source AI assistant)
- Codeium
- Cody AI by Sourcegraph
- Tabnine
- Sourcery (AI refactoring)
- Amazon Q
- CodeGPT
- Genie AI
- And many more...

### 🌍 Environment Switching
- Manage multiple AI provider environments
- Switch between local and cloud models
- Support for major AI providers:
  - OpenAI (GPT-4, GPT-3.5)
  - Anthropic (Claude 3)
  - Google (Gemini)
  - Ollama (Local models)
  - Azure OpenAI

### ⚙️ Configuration Management
- Centralized API key management
- Model parameter configuration (temperature, max tokens)
- Code completion settings
- Chat feature customization
- Import/Export configurations

## Installation

### From VSIX File
1. Download the latest `.vsix` file
2. Open VSCode
3. Run `Extensions: Install from VSIX...` from the command palette
4. Select the downloaded file

### From Source
```bash
# Clone the repository
cd ai-assistant-extension

# Install dependencies
npm install

# Compile
npm run compile

# Press F5 to launch the extension in debug mode
```

## Usage

### Open AI Assistant Panel
- Click the AI Assistant icon in the Activity Bar
- Or use the keyboard shortcut: `Ctrl+Shift+A` (Windows/Linux) / `Cmd+Shift+A` (Mac)
- Or run `AI Assistant: Open Manager Panel` from the command palette

### Install AI Plugins
1. Go to the "AI Plugins" view
2. Click on any plugin to install it
3. Right-click installed plugins to manage them

### Switch Environments
1. Go to the "Environments" view
2. Click on an environment to activate it
3. View environment details and settings

### Configure Settings
1. Go to the "Configuration" view
2. Set up API keys for your providers
3. Customize model parameters and features

## Available Commands

| Command | Description |
|---------|-------------|
| `ai-assistant-manager.showPanel` | Open AI Assistant Manager Panel |
| `ai-assistant-manager.installPlugin` | Install AI plugin |
| `ai-assistant-manager.switchEnvironment` | Switch AI environment |
| `ai-assistant-manager.configureModel` | Configure AI model settings |

## Extension Settings

This extension contributes the following settings:

- `ai-assistant-manager.defaultProvider`: Default AI provider to use
- `ai-assistant-manager.apiKeys`: Stored API keys for different providers
- `ai-assistant-manager.enabledPlugins`: List of enabled AI plugins

## Building Custom VSCode with AI Assistant

To create a custom VSCode build with the AI Assistant pre-installed:

1. Clone VSCode source:
```bash
git clone https://github.com/microsoft/vscode.git
cd vscode
```

2. Install dependencies:
```bash
npm install
```

3. Add this extension to the built-in extensions folder

4. Build:
```bash
npm run compile
```

5. Run:
```bash
./scripts/code.sh
```

6. Package for Windows:
```bash
npm run package-win
```

## Requirements

- VSCode 1.80.0 or higher
- Node.js 16+ (for development)
- API keys for AI providers you want to use

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

MIT License

## Support

If you encounter any issues or have questions, please file an issue on the project repository.
