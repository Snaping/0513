"use strict";
var __createBinding = (this && this.__createBinding) || (Object.create ? (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    var desc = Object.getOwnPropertyDescriptor(m, k);
    if (!desc || ("get" in desc ? !m.__esModule : desc.writable || desc.configurable)) {
      desc = { enumerable: true, get: function() { return m[k]; } };
    }
    Object.defineProperty(o, k2, desc);
}) : (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    o[k2] = m[k];
}));
var __setModuleDefault = (this && this.__setModuleDefault) || (Object.create ? (function(o, v) {
    Object.defineProperty(o, "default", { enumerable: true, value: v });
}) : function(o, v) {
    o["default"] = v;
});
var __importStar = (this && this.__importStar) || (function () {
    var ownKeys = function(o) {
        ownKeys = Object.getOwnPropertyNames || function (o) {
            var ar = [];
            for (var k in o) if (Object.prototype.hasOwnProperty.call(o, k)) ar[ar.length] = k;
            return ar;
        };
        return ownKeys(o);
    };
    return function (mod) {
        if (mod && mod.__esModule) return mod;
        var result = {};
        if (mod != null) for (var k = ownKeys(mod), i = 0; i < k.length; i++) if (k[i] !== "default") __createBinding(result, mod, k[i]);
        __setModuleDefault(result, mod);
        return result;
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
exports.ConfigurationView = exports.EnvironmentsView = exports.AIPluginsView = void 0;
const vscode = __importStar(require("vscode"));
class PluginTreeItem extends vscode.TreeItem {
    constructor(plugin) {
        super(plugin.name, vscode.TreeItemCollapsibleState.None);
        this.plugin = plugin;
        this.description = plugin.version;
        this.tooltip = plugin.description;
        this.iconPath = new vscode.ThemeIcon(plugin.icon || 'package');
        this.contextValue = plugin.installed ? 'installedPlugin' : 'availablePlugin';
        this.command = {
            command: 'ai-assistant-manager.pluginAction',
            title: plugin.installed ? 'Manage' : 'Install',
            arguments: [plugin]
        };
    }
}
class AIPluginsView {
    constructor(context, pluginManager) {
        this.context = context;
        this.pluginManager = pluginManager;
        this._onDidChangeTreeData = new vscode.EventEmitter();
        this.onDidChangeTreeData = this._onDidChangeTreeData.event;
        vscode.window.registerTreeDataProvider('aiAssistantExplorer', this);
        context.subscriptions.push(vscode.commands.registerCommand('ai-assistant-manager.pluginAction', async (plugin) => {
            if (plugin.installed) {
                const action = await vscode.window.showQuickPick(['Disable', 'Uninstall'], {
                    placeHolder: `Select action for ${plugin.name}`
                });
                if (action === 'Uninstall') {
                    await pluginManager.uninstallPlugin(plugin);
                    this.refresh();
                }
            }
            else {
                await pluginManager.installPlugin(plugin);
                this.refresh();
            }
        }), vscode.commands.registerCommand('ai-assistant-manager.refreshPlugins', () => this.refresh()));
    }
    refresh() {
        this._onDidChangeTreeData.fire(null);
    }
    getTreeItem(element) {
        return new PluginTreeItem(element);
    }
    getChildren(element) {
        if (element) {
            return Promise.resolve([]);
        }
        return Promise.resolve(this.pluginManager.getAvailablePlugins());
    }
}
exports.AIPluginsView = AIPluginsView;
class EnvironmentTreeItem extends vscode.TreeItem {
    constructor(env) {
        super(env.name, vscode.TreeItemCollapsibleState.Collapsed);
        this.env = env;
        this.description = `${env.provider} - ${env.model || ''}`;
        this.tooltip = `${env.name}\nType: ${env.type}\nProvider: ${env.provider}`;
        this.iconPath = env.active
            ? new vscode.ThemeIcon('check', new vscode.ThemeColor('testing.iconPassed'))
            : new vscode.ThemeIcon('server-environment');
        this.contextValue = env.active ? 'activeEnvironment' : 'inactiveEnvironment';
    }
}
class EnvironmentsView {
    constructor(context, envManager) {
        this.context = context;
        this.envManager = envManager;
        this._onDidChangeTreeData = new vscode.EventEmitter();
        this.onDidChangeTreeData = this._onDidChangeTreeData.event;
        vscode.window.registerTreeDataProvider('aiEnvironments', this);
        context.subscriptions.push(vscode.commands.registerCommand('ai-assistant-manager.switchEnv', async (env) => {
            await envManager.switchEnvironment(env);
            this.refresh();
        }), vscode.commands.registerCommand('ai-assistant-manager.addEnvironment', async () => {
            vscode.window.showInformationMessage('Add environment feature coming soon!');
        }), vscode.commands.registerCommand('ai-assistant-manager.refreshEnvironments', () => this.refresh()));
    }
    refresh() {
        this._onDidChangeTreeData.fire(null);
    }
    getTreeItem(element) {
        return new EnvironmentTreeItem(element);
    }
    getChildren(element) {
        if (element) {
            return Promise.resolve([]);
        }
        return Promise.resolve(this.envManager.getEnvironments());
    }
}
exports.EnvironmentsView = EnvironmentsView;
class ConfigTreeItem extends vscode.TreeItem {
    constructor(label, description, command) {
        super(label, vscode.TreeItemCollapsibleState.None);
        this.description = description;
        this.command = command;
    }
}
class ConfigurationView {
    constructor(context, configManager) {
        this.context = context;
        this.configManager = configManager;
        this._onDidChangeTreeData = new vscode.EventEmitter();
        this.onDidChangeTreeData = this._onDidChangeTreeData.event;
        vscode.window.registerTreeDataProvider('aiConfiguration', this);
        context.subscriptions.push(vscode.commands.registerCommand('ai-assistant-manager.openSettings', () => {
            vscode.commands.executeCommand('workbench.action.openSettings', 'ai-assistant-manager');
        }), vscode.commands.registerCommand('ai-assistant-manager.configureApiKeys', async () => {
            const providers = ['OpenAI', 'Anthropic', 'Google', 'Ollama'];
            const provider = await vscode.window.showQuickPick(providers, {
                placeHolder: 'Select provider to configure API key'
            });
            if (provider) {
                const apiKey = await vscode.window.showInputBox({
                    placeHolder: `Enter your ${provider} API key`,
                    password: true
                });
                if (apiKey) {
                    await configManager.setApiKey(provider.toLowerCase(), apiKey);
                }
            }
        }), vscode.commands.registerCommand('ai-assistant-manager.resetConfig', async () => {
            const confirm = await vscode.window.showWarningMessage('Are you sure you want to reset all configuration to defaults?', 'Yes', 'No');
            if (confirm === 'Yes') {
                configManager.resetToDefaults();
                this.refresh();
            }
        }));
    }
    refresh() {
        this._onDidChangeTreeData.fire(null);
    }
    getTreeItem(element) {
        return element;
    }
    getChildren() {
        const config = this.configManager.getConfig();
        const items = [];
        items.push(new ConfigTreeItem('API Keys', 'Configure API keys', {
            command: 'ai-assistant-manager.configureApiKeys',
            title: 'Configure API Keys'
        }));
        items.push(new ConfigTreeItem('Default Provider', config.defaultProvider, {
            command: 'ai-assistant-manager.openSettings',
            title: 'Open Settings'
        }));
        items.push(new ConfigTreeItem('Code Completion', config.codeCompletion.enabled ? 'Enabled' : 'Disabled', {
            command: 'ai-assistant-manager.openSettings',
            title: 'Open Settings'
        }));
        items.push(new ConfigTreeItem('Chat Features', config.chatSettings.enabled ? 'Enabled' : 'Disabled', {
            command: 'ai-assistant-manager.openSettings',
            title: 'Open Settings'
        }));
        items.push(new ConfigTreeItem('Open Settings', '', {
            command: 'workbench.action.openSettings',
            title: 'Open Settings',
            arguments: ['ai-assistant-manager']
        }));
        items.push(new ConfigTreeItem('Reset to Defaults', '', {
            command: 'ai-assistant-manager.resetConfig',
            title: 'Reset Configuration'
        }));
        return Promise.resolve(items);
    }
}
exports.ConfigurationView = ConfigurationView;
//# sourceMappingURL=views.js.map