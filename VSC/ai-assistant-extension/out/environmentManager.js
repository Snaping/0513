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
exports.EnvironmentManager = void 0;
const vscode = __importStar(require("vscode"));
const fs = __importStar(require("fs"));
class EnvironmentManager {
    constructor(context) {
        this.context = context;
        this.environments = [];
        this.STORAGE_KEY = 'ai_environments';
        this.loadEnvironments();
        this.ensureDefaultEnvironments();
    }
    loadEnvironments() {
        const saved = this.context.globalState.get(this.STORAGE_KEY);
        if (saved) {
            this.environments = saved;
        }
    }
    saveEnvironments() {
        this.context.globalState.update(this.STORAGE_KEY, this.environments);
    }
    ensureDefaultEnvironments() {
        const defaults = [
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
    getEnvironments() {
        return this.environments;
    }
    getActiveEnvironment() {
        return this.environments.find(e => e.active);
    }
    async switchEnvironment(env) {
        this.environments.forEach(e => e.active = false);
        const target = this.environments.find(e => e.id === env.id);
        if (target) {
            target.active = true;
            this.saveEnvironments();
            vscode.window.showInformationMessage(`Switched to ${target.name} environment`);
        }
    }
    async addEnvironment(env) {
        const newEnv = {
            ...env,
            id: `env_${Date.now()}`
        };
        this.environments.push(newEnv);
        this.saveEnvironments();
        return newEnv;
    }
    async removeEnvironment(id) {
        this.environments = this.environments.filter(e => e.id !== id);
        this.saveEnvironments();
    }
    async updateEnvironment(id, updates) {
        const env = this.environments.find(e => e.id === id);
        if (env) {
            Object.assign(env, updates);
            this.saveEnvironments();
        }
    }
    testConnection(env) {
        return new Promise((resolve) => {
            setTimeout(() => {
                resolve(true);
            }, 1000);
        });
    }
    getModelsForProvider(provider) {
        const models = {
            openai: ['gpt-4o', 'gpt-4-turbo', 'gpt-4', 'gpt-3.5-turbo'],
            anthropic: ['claude-3-opus-20240229', 'claude-3-sonnet-20240229', 'claude-3-haiku-20240307'],
            google: ['gemini-pro', 'gemini-ultra', 'gemini-1.5-pro-latest'],
            ollama: ['llama3', 'mistral', 'codellama', 'phi3', 'vicuna'],
            azure: ['gpt-4o', 'gpt-4-turbo', 'gpt-35-turbo']
        };
        return models[provider] || [];
    }
    async exportEnvironment(id, filePath) {
        const env = this.environments.find(e => e.id === id);
        if (env) {
            const exportData = { ...env };
            delete exportData.apiKey;
            fs.writeFileSync(filePath, JSON.stringify(exportData, null, 2));
        }
    }
    async importEnvironment(filePath) {
        const content = fs.readFileSync(filePath, 'utf-8');
        const env = JSON.parse(content);
        env.id = `env_${Date.now()}`;
        this.environments.push(env);
        this.saveEnvironments();
    }
}
exports.EnvironmentManager = EnvironmentManager;
//# sourceMappingURL=environmentManager.js.map