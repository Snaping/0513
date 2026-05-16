import * as vscode from 'vscode';
import * as fs from 'fs';
import * as path from 'path';

export interface Environment {
    id: string;
    name: string;
    type: 'local' | 'cloud' | 'remote';
    provider: string;
    baseUrl?: string;
    model?: string;
    apiKey?: string;
    active: boolean;
    config: Record<string, any>;
}

export class EnvironmentManager {
    private environments: Environment[] = [];
    private readonly STORAGE_KEY = 'ai_environments';

    constructor(private context: vscode.ExtensionContext) {
        this.loadEnvironments();
        this.ensureDefaultEnvironments();
    }

    private loadEnvironments(): void {
        const saved = this.context.globalState.get<Environment[]>(this.STORAGE_KEY);
        if (saved) {
            this.environments = saved;
        }
    }

    private saveEnvironments(): void {
        this.context.globalState.update(this.STORAGE_KEY, this.environments);
    }

    private ensureDefaultEnvironments(): void {
        const defaults: Environment[] = [
            {
                id: 'openai-default',
                name: 'OpenAI (GPT-4)',
                type: 'cloud',
                provider: 'openai',
                model: 'gpt-4o',
                active: false,
                config: {
                    baseUrl: 'https://api.openai.com/v1',
                    temperature: 0.7,
                    maxTokens: 4096
                }
            },
            {
                id: 'anthropic-default',
                name: 'Anthropic (Claude 3)',
                type: 'cloud',
                provider: 'anthropic',
                model: 'claude-3-opus-20240229',
                active: false,
                config: {
                    baseUrl: 'https://api.anthropic.com/v1',
                    temperature: 0.7,
                    maxTokens: 4096
                }
            },
            {
                id: 'google-default',
                name: 'Google (Gemini)',
                type: 'cloud',
                provider: 'google',
                model: 'gemini-pro',
                active: false,
                config: {
                    baseUrl: 'https://generativelanguage.googleapis.com/v1',
                    temperature: 0.7
                }
            },
            {
                id: 'ollama-local',
                name: 'Ollama (Local)',
                type: 'local',
                provider: 'ollama',
                model: 'llama3',
                baseUrl: 'http://localhost:11434',
                active: false,
                config: {
                    temperature: 0.8,
                    numCtx: 4096
                }
            },
            {
                id: 'azure-openai',
                name: 'Azure OpenAI',
                type: 'cloud',
                provider: 'azure',
                active: false,
                config: {
                    apiVersion: '2024-02-15-preview'
                }
            }
        ];

        defaults.forEach(defaultEnv => {
            if (!this.environments.find(e => e.id === defaultEnv.id)) {
                this.environments.push(defaultEnv);
            }
        });

        if (!this.environments.some(e => e.active)) {
            this.environments[0].active = true;
        }

        this.saveEnvironments();
    }

    getEnvironments(): Environment[] {
        return this.environments;
    }

    getActiveEnvironment(): Environment | undefined {
        return this.environments.find(e => e.active);
    }

    async switchEnvironment(env: Environment): Promise<void> {
        this.environments.forEach(e => e.active = false);
        const target = this.environments.find(e => e.id === env.id);
        if (target) {
            target.active = true;
            this.saveEnvironments();
            vscode.window.showInformationMessage(`Switched to ${target.name} environment`);
        }
    }

    async addEnvironment(env: Omit<Environment, 'id'>): Promise<Environment> {
        const newEnv: Environment = {
            ...env,
            id: `env_${Date.now()}`
        };
        this.environments.push(newEnv);
        this.saveEnvironments();
        return newEnv;
    }

    async removeEnvironment(id: string): Promise<void> {
        this.environments = this.environments.filter(e => e.id !== id);
        this.saveEnvironments();
    }

    async updateEnvironment(id: string, updates: Partial<Environment>): Promise<void> {
        const env = this.environments.find(e => e.id === id);
        if (env) {
            Object.assign(env, updates);
            this.saveEnvironments();
        }
    }

    testConnection(env: Environment): Promise<boolean> {
        return new Promise((resolve) => {
            setTimeout(() => {
                resolve(true);
            }, 1000);
        });
    }

    getModelsForProvider(provider: string): string[] {
        const models: Record<string, string[]> = {
            openai: ['gpt-4o', 'gpt-4-turbo', 'gpt-4', 'gpt-3.5-turbo'],
            anthropic: ['claude-3-opus-20240229', 'claude-3-sonnet-20240229', 'claude-3-haiku-20240307'],
            google: ['gemini-pro', 'gemini-ultra', 'gemini-1.5-pro-latest'],
            ollama: ['llama3', 'mistral', 'codellama', 'phi3', 'vicuna'],
            azure: ['gpt-4o', 'gpt-4-turbo', 'gpt-35-turbo']
        };
        return models[provider] || [];
    }

    async exportEnvironment(id: string, filePath: string): Promise<void> {
        const env = this.environments.find(e => e.id === id);
        if (env) {
            const exportData = { ...env };
            delete exportData.apiKey;
            fs.writeFileSync(filePath, JSON.stringify(exportData, null, 2));
        }
    }

    async importEnvironment(filePath: string): Promise<void> {
        const content = fs.readFileSync(filePath, 'utf-8');
        const env = JSON.parse(content) as Environment;
        env.id = `env_${Date.now()}`;
        this.environments.push(env);
        this.saveEnvironments();
    }
}
