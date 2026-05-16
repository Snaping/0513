import * as vscode from 'vscode';

export interface AIConfiguration {
    defaultProvider: string;
    apiKeys: Record<string, string>;
    enabledPlugins: string[];
    modelSettings: Record<string, ModelSettings>;
    codeCompletion: CodeCompletionSettings;
    chatSettings: ChatSettings;
}

export interface ModelSettings {
    temperature: number;
    maxTokens: number;
    topP?: number;
    frequencyPenalty?: number;
    presencePenalty?: number;
}

export interface CodeCompletionSettings {
    enabled: boolean;
    autoTrigger: boolean;
    delay: number;
    maxLineCount: number;
    disableInFiles: string[];
}

export interface ChatSettings {
    enabled: boolean;
    showCodeActions: boolean;
    autoApplySuggestions: boolean;
    showInlineHints: boolean;
}

const DEFAULT_CONFIG: AIConfiguration = {
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

export class ConfigManager {
    private readonly CONFIG_KEY = 'ai-assistant-manager.config';
    private config: AIConfiguration;

    constructor(private context: vscode.ExtensionContext) {
        this.config = this.loadConfig();
    }

    private loadConfig(): AIConfiguration {
        const saved = this.context.globalState.get<AIConfiguration>(this.CONFIG_KEY);
        return { ...DEFAULT_CONFIG, ...saved };
    }

    private saveConfig(): void {
        this.context.globalState.update(this.CONFIG_KEY, this.config);
    }

    getConfig(): AIConfiguration {
        return { ...this.config };
    }

    updateConfig(updates: Partial<AIConfiguration>): void {
        this.config = { ...this.config, ...updates };
        this.saveConfig();
    }

    getApiKey(provider: string): string | undefined {
        return this.config.apiKeys[provider];
    }

    async setApiKey(provider: string, key: string): Promise<void> {
        this.config.apiKeys[provider] = key;
        this.saveConfig();
        await vscode.commands.executeCommand('setContext', `ai-assistant-manager.${provider}KeySet`, true);
    }

    removeApiKey(provider: string): void {
        delete this.config.apiKeys[provider];
        this.saveConfig();
    }

    hasApiKey(provider: string): boolean {
        return !!this.config.apiKeys[provider];
    }

    getDefaultProvider(): string {
        return this.config.defaultProvider;
    }

    setDefaultProvider(provider: string): void {
        this.config.defaultProvider = provider;
        this.saveConfig();
    }

    getModelSettings(provider: string): ModelSettings | undefined {
        return this.config.modelSettings[provider];
    }

    updateModelSettings(provider: string, settings: Partial<ModelSettings>): void {
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

    getCodeCompletionSettings(): CodeCompletionSettings {
        return { ...this.config.codeCompletion };
    }

    updateCodeCompletionSettings(settings: Partial<CodeCompletionSettings>): void {
        this.config.codeCompletion = {
            ...this.config.codeCompletion,
            ...settings
        };
        this.saveConfig();
    }

    getChatSettings(): ChatSettings {
        return { ...this.config.chatSettings };
    }

    updateChatSettings(settings: Partial<ChatSettings>): void {
        this.config.chatSettings = {
            ...this.config.chatSettings,
            ...settings
        };
        this.saveConfig();
    }

    isPluginEnabled(pluginId: string): boolean {
        return this.config.enabledPlugins.includes(pluginId);
    }

    enablePlugin(pluginId: string): void {
        if (!this.config.enabledPlugins.includes(pluginId)) {
            this.config.enabledPlugins.push(pluginId);
            this.saveConfig();
        }
    }

    disablePlugin(pluginId: string): void {
        this.config.enabledPlugins = this.config.enabledPlugins.filter(id => id !== pluginId);
        this.saveConfig();
    }

    resetToDefaults(): void {
        this.config = JSON.parse(JSON.stringify(DEFAULT_CONFIG));
        this.saveConfig();
        vscode.window.showInformationMessage('Configuration reset to defaults');
    }

    exportConfig(filePath: string): void {
        const { apiKeys, ...exportData } = this.config;
        const fs = require('fs');
        fs.writeFileSync(filePath, JSON.stringify(exportData, null, 2));
    }

    importConfig(filePath: string): void {
        const fs = require('fs');
        const content = fs.readFileSync(filePath, 'utf-8');
        const imported = JSON.parse(content);
        this.config = { ...this.config, ...imported };
        this.saveConfig();
    }

    validateConfig(): { valid: boolean; issues: string[] } {
        const issues: string[] = [];

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
