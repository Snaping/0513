import * as vscode from 'vscode';
import { AIPluginManager } from './aiPluginManager';
import { EnvironmentManager } from './environmentManager';
import { ConfigManager } from './configManager';
import { AIPluginsView, EnvironmentsView, ConfigurationView } from './views';

export function activate(context: vscode.ExtensionContext) {
    console.log('AI Assistant Manager is now active!');

    const pluginManager = new AIPluginManager(context);
    const envManager = new EnvironmentManager(context);
    const configManager = new ConfigManager(context);

    new AIPluginsView(context, pluginManager);
    new EnvironmentsView(context, envManager);
    new ConfigurationView(context, configManager);

    let showPanel = vscode.commands.registerCommand('ai-assistant-manager.showPanel', () => {
        vscode.commands.executeCommand('workbench.view.extension.ai-assistant');
    });

    let installPlugin = vscode.commands.registerCommand('ai-assistant-manager.installPlugin', async () => {
        const plugins = pluginManager.getAvailablePlugins();
        const pluginNames = plugins.map(p => p.name);
        const selected = await vscode.window.showQuickPick(pluginNames, {
            placeHolder: 'Select an AI plugin to install'
        });
        if (selected) {
            const plugin = plugins.find(p => p.name === selected);
            if (plugin) {
                await pluginManager.installPlugin(plugin);
                vscode.window.showInformationMessage(`Successfully installed ${selected}!`);
            }
        }
    });

    let switchEnvironment = vscode.commands.registerCommand('ai-assistant-manager.switchEnvironment', async () => {
        const envs = envManager.getEnvironments();
        const envNames = envs.map(e => e.name);
        const selected = await vscode.window.showQuickPick(envNames, {
            placeHolder: 'Select an environment'
        });
        if (selected) {
            const env = envs.find(e => e.name === selected);
            if (env) {
                await envManager.switchEnvironment(env);
                vscode.window.showInformationMessage(`Switched to ${selected} environment!`);
            }
        }
    });

    let configureModel = vscode.commands.registerCommand('ai-assistant-manager.configureModel', async () => {
        const providers = ['OpenAI', 'Anthropic', 'Google', 'Ollama', 'Custom'];
        const selected = await vscode.window.showQuickPick(providers, {
            placeHolder: 'Select AI provider to configure'
        });
        if (selected) {
            const apiKey = await vscode.window.showInputBox({
                placeHolder: `Enter your ${selected} API key`,
                password: true
            });
            if (apiKey) {
                await configManager.setApiKey(selected.toLowerCase(), apiKey);
                vscode.window.showInformationMessage(`${selected} API key configured!`);
            }
        }
    });

    context.subscriptions.push(showPanel, installPlugin, switchEnvironment, configureModel);
}

export function deactivate() {
    console.log('AI Assistant Manager is deactivated!');
}
