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
exports.ConfigManager = void 0;
const vscode = __importStar(require("vscode"));
const DEFAULT_CONFIG = {
    defaultProvider: 'openai',
    apiKeys: {},
    enabledPlugins: [],
    modelSettings: {
        openai: {
            temperature: 0.7,
            maxTokens: 4096,
            topP: 1.0
        },
        anthropic: {
            temperature: 0.7,
            maxTokens: 4096
        },
        google: {
            temperature: 0.7,
            maxTokens: 2048
        },
        ollama: {
            temperature: 0.8,
            maxTokens: 2048
        }
    },
    codeCompletion: {
        enabled: true,
        autoTrigger: true,
        delay: 500,
        maxLineCount: 10,
        disableInFiles: ['*.md', '*.txt']
    },
    chatSettings: {
        enabled: true,
        showCodeActions: true,
        autoApplySuggestions: false,
        showInlineHints: true
    }
};
class ConfigManager {
    constructor(context) {
        this.context = context;
        this.CONFIG_KEY = 'ai-assistant-manager.config';
        this.config = this.loadConfig();
    }
    loadConfig() {
        const saved = this.context.globalState.get(this.CONFIG_KEY);
        return { ...DEFAULT_CONFIG, ...saved };
    }
    saveConfig() {
        this.context.globalState.update(this.CONFIG_KEY, this.config);
    }
    getConfig() {
        return { ...this.config };
    }
    updateConfig(updates) {
        this.config = { ...this.config, ...updates };
        this.saveConfig();
    }
    getApiKey(provider) {
        return this.config.apiKeys[provider];
    }
    async setApiKey(provider, key) {
        this.config.apiKeys[provider] = key;
        this.saveConfig();
        await vscode.commands.executeCommand('setContext', `ai-assistant-manager.${provider}KeySet`, true);
    }
    removeApiKey(provider) {
        delete this.config.apiKeys[provider];
        this.saveConfig();
    }
    hasApiKey(provider) {
        return !!this.config.apiKeys[provider];
    }
    getDefaultProvider() {
        return this.config.defaultProvider;
    }
    setDefaultProvider(provider) {
        this.config.defaultProvider = provider;
        this.saveConfig();
    }
    getModelSettings(provider) {
        return this.config.modelSettings[provider];
    }
    updateModelSettings(provider, settings) {
        if (!this.config.modelSettings[provider]) {
            this.config.modelSettings[provider] = {
                temperature: 0.7,
                maxTokens: 2048
            };
        }
        this.config.modelSettings[provider] = {
            ...this.config.modelSettings[provider],
            ...settings
        };
        this.saveConfig();
    }
    getCodeCompletionSettings() {
        return { ...this.config.codeCompletion };
    }
    updateCodeCompletionSettings(settings) {
        this.config.codeCompletion = {
            ...this.config.codeCompletion,
            ...settings
        };
        this.saveConfig();
    }
    getChatSettings() {
        return { ...this.config.chatSettings };
    }
    updateChatSettings(settings) {
        this.config.chatSettings = {
            ...this.config.chatSettings,
            ...settings
        };
        this.saveConfig();
    }
    isPluginEnabled(pluginId) {
        return this.config.enabledPlugins.includes(pluginId);
    }
    enablePlugin(pluginId) {
        if (!this.config.enabledPlugins.includes(pluginId)) {
            this.config.enabledPlugins.push(pluginId);
            this.saveConfig();
        }
    }
    disablePlugin(pluginId) {
        this.config.enabledPlugins = this.config.enabledPlugins.filter(id => id !== pluginId);
        this.saveConfig();
    }
    resetToDefaults() {
        this.config = JSON.parse(JSON.stringify(DEFAULT_CONFIG));
        this.saveConfig();
        vscode.window.showInformationMessage('Configuration reset to defaults');
    }
    exportConfig(filePath) {
        const { apiKeys, ...exportData } = this.config;
        const fs = require('fs');
        fs.writeFileSync(filePath, JSON.stringify(exportData, null, 2));
    }
    importConfig(filePath) {
        const fs = require('fs');
        const content = fs.readFileSync(filePath, 'utf-8');
        const imported = JSON.parse(content);
        this.config = { ...this.config, ...imported };
        this.saveConfig();
    }
    validateConfig() {
        const issues = [];
        if (!this.config.apiKeys[this.config.defaultProvider]) {
            issues.push(`No API key configured for default provider: ${this.config.defaultProvider}`);
        }
        if (this.config.codeCompletion.delay < 0) {
            issues.push('Code completion delay cannot be negative');
        }
        if (this.config.modelSettings[this.config.defaultProvider]?.temperature < 0 ||
            this.config.modelSettings[this.config.defaultProvider]?.temperature > 2) {
            issues.push('Temperature must be between 0 and 2');
        }
        return {
            valid: issues.length === 0,
            issues
        };
    }
}
exports.ConfigManager = ConfigManager;
//# sourceMappingURL=configManager.js.map