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
exports.activate = activate;
exports.deactivate = deactivate;
const vscode = __importStar(require("vscode"));
const aiPluginManager_1 = require("./aiPluginManager");
const environmentManager_1 = require("./environmentManager");
const configManager_1 = require("./configManager");
const views_1 = require("./views");
function activate(context) {
    console.log('AI Assistant Manager is now active!');
    const pluginManager = new aiPluginManager_1.AIPluginManager(context);
    const envManager = new environmentManager_1.EnvironmentManager(context);
    const configManager = new configManager_1.ConfigManager(context);
    new views_1.AIPluginsView(context, pluginManager);
    new views_1.EnvironmentsView(context, envManager);
    new views_1.ConfigurationView(context, configManager);
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
function deactivate() {
    console.log('AI Assistant Manager is deactivated!');
}
//# sourceMappingURL=extension.js.map