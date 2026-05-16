import * as vscode from 'vscode';
import { AIPluginManager, AIPlugin } from './aiPluginManager';
import { EnvironmentManager, Environment } from './environmentManager';
import { ConfigManager } from './configManager';

class PluginTreeItem extends vscode.TreeItem {
    constructor(public readonly plugin: AIPlugin) {
        super(plugin.name, vscode.TreeItemCollapsibleState.None);
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

export class AIPluginsView implements vscode.TreeDataProvider<AIPlugin> {
    private _onDidChangeTreeData = new vscode.EventEmitter<AIPlugin | undefined | null>();
    readonly onDidChangeTreeData = this._onDidChangeTreeData.event;

    constructor(
        private context: vscode.ExtensionContext,
        private pluginManager: AIPluginManager
    ) {
        vscode.window.registerTreeDataProvider('aiAssistantExplorer', this);

        context.subscriptions.push(
            vscode.commands.registerCommand('ai-assistant-manager.pluginAction', async (plugin: AIPlugin) => {
                if (plugin.installed) {
                    const action = await vscode.window.showQuickPick(['Disable', 'Uninstall'], {
                        placeHolder: `Select action for ${plugin.name}`
                    });
                    if (action === 'Uninstall') {
                        await pluginManager.uninstallPlugin(plugin);
                        this.refresh();
                    }
                } else {
                    await pluginManager.installPlugin(plugin);
                    this.refresh();
                }
            }),
            vscode.commands.registerCommand('ai-assistant-manager.refreshPlugins', () => this.refresh())
        );
    }

    refresh(): void {
        this._onDidChangeTreeData.fire(null);
    }

    getTreeItem(element: AIPlugin): vscode.TreeItem {
        return new PluginTreeItem(element);
    }

    getChildren(element?: AIPlugin): Thenable<AIPlugin[]> {
        if (element) {
            return Promise.resolve([]);
        }
        return Promise.resolve(this.pluginManager.getAvailablePlugins());
    }
}

class EnvironmentTreeItem extends vscode.TreeItem {
    constructor(public readonly env: Environment) {
        super(env.name, vscode.TreeItemCollapsibleState.Collapsed);
        this.description = `${env.provider} - ${env.model || ''}`;
        this.tooltip = `${env.name}\nType: ${env.type}\nProvider: ${env.provider}`;
        this.iconPath = env.active
            ? new vscode.ThemeIcon('check', new vscode.ThemeColor('testing.iconPassed'))
            : new vscode.ThemeIcon('server-environment');
        this.contextValue = env.active ? 'activeEnvironment' : 'inactiveEnvironment';
    }
}

export class EnvironmentsView implements vscode.TreeDataProvider<Environment> {
    private _onDidChangeTreeData = new vscode.EventEmitter<Environment | undefined | null>();
    readonly onDidChangeTreeData = this._onDidChangeTreeData.event;

    constructor(
        private context: vscode.ExtensionContext,
        private envManager: EnvironmentManager
    ) {
        vscode.window.registerTreeDataProvider('aiEnvironments', this);

        context.subscriptions.push(
            vscode.commands.registerCommand('ai-assistant-manager.switchEnv', async (env: Environment) => {
                await envManager.switchEnvironment(env);
                this.refresh();
            }),
            vscode.commands.registerCommand('ai-assistant-manager.addEnvironment', async () => {
                vscode.window.showInformationMessage('Add environment feature coming soon!');
            }),
            vscode.commands.registerCommand('ai-assistant-manager.refreshEnvironments', () => this.refresh())
        );
    }

    refresh(): void {
        this._onDidChangeTreeData.fire(null);
    }

    getTreeItem(element: Environment): vscode.TreeItem {
        return new EnvironmentTreeItem(element);
    }

    getChildren(element?: Environment): Thenable<Environment[]> {
        if (element) {
            return Promise.resolve([]);
        }
        return Promise.resolve(this.envManager.getEnvironments());
    }
}

class ConfigTreeItem extends vscode.TreeItem {
    constructor(label: string, description?: string, command?: vscode.Command) {
        super(label, vscode.TreeItemCollapsibleState.None);
        this.description = description;
        this.command = command;
    }
}

export class ConfigurationView implements vscode.TreeDataProvider<vscode.TreeItem> {
    private _onDidChangeTreeData = new vscode.EventEmitter<vscode.TreeItem | undefined | null>();
    readonly onDidChangeTreeData = this._onDidChangeTreeData.event;

    constructor(
        private context: vscode.ExtensionContext,
        private configManager: ConfigManager
    ) {
        vscode.window.registerTreeDataProvider('aiConfiguration', this);

        context.subscriptions.push(
            vscode.commands.registerCommand('ai-assistant-manager.openSettings', () => {
                vscode.commands.executeCommand('workbench.action.openSettings', 'ai-assistant-manager');
            }),
            vscode.commands.registerCommand('ai-assistant-manager.configureApiKeys', async () => {
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
            }),
            vscode.commands.registerCommand('ai-assistant-manager.resetConfig', async () => {
                const confirm = await vscode.window.showWarningMessage(
                    'Are you sure you want to reset all configuration to defaults?',
                    'Yes',
                    'No'
                );
                if (confirm === 'Yes') {
                    configManager.resetToDefaults();
                    this.refresh();
                }
            })
        );
    }

    refresh(): void {
        this._onDidChangeTreeData.fire(null);
    }

    getTreeItem(element: vscode.TreeItem): vscode.TreeItem {
        return element;
    }

    getChildren(): Thenable<vscode.TreeItem[]> {
        const config = this.configManager.getConfig();
        const items: vscode.TreeItem[] = [];

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
