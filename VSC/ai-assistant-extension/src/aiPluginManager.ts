import * as vscode from 'vscode';

export interface AIPlugin {
    id: string;
    name: string;
    description: string;
    publisher: string;
    version: string;
    installed: boolean;
    category: 'coding' | 'chat' | 'refactoring' | 'testing' | 'other';
    icon?: string;
}

export class AIPluginManager {
    private readonly availablePlugins: AIPlugin[] = [
        {
            id: 'github.copilot',
            name: 'GitHub Copilot',
            description: 'AI pair programmer that helps you write better code',
            publisher: 'GitHub',
            version: 'latest',
            installed: false,
            category: 'coding',
            icon: '$(sparkle)'
        },
        {
            id: 'github.copilot-chat',
            name: 'GitHub Copilot Chat',
            description: 'Chat interface for Copilot with code explanation and generation',
            publisher: 'GitHub',
            version: 'latest',
            installed: false,
            category: 'chat',
            icon: '$(comment)'
        },
        {
            id: 'continue.continue',
            name: 'Continue',
            description: 'Open-source AI code assistant with local model support',
            publisher: 'Continue',
            version: 'latest',
            installed: false,
            category: 'coding',
            icon: '$(code)'
        },
        {
            id: 'codeium.codeium',
            name: 'Codeium',
            description: 'Free AI code completion and chat tool',
            publisher: 'Codeium',
            version: 'latest',
            installed: false,
            category: 'coding',
            icon: '$(lightbulb)'
        },
        {
            id: 'sourcegraph.cody-ai',
            name: 'Cody AI',
            description: 'AI coding assistant with codebase context',
            publisher: 'Sourcegraph',
            version: 'latest',
            installed: false,
            category: 'chat',
            icon: '$(robot)'
        },
        {
            id: 'tabnine.tabnine-vscode',
            name: 'Tabnine',
            description: 'AI code completion powered by machine learning',
            publisher: 'Tabnine',
            version: 'latest',
            installed: false,
            category: 'coding',
            icon: '$(symbol-class)'
        },
        {
            id: 'kiteco.kite',
            name: 'Kite',
            description: 'AI-powered coding assistant with line-of-code completions',
            publisher: 'Kite',
            version: 'latest',
            installed: false,
            category: 'coding',
            icon: '$(rocket)'
        },
        {
            id: 'sourcery.sourcery',
            name: 'Sourcery',
            description: 'AI-powered refactoring and code quality tool',
            publisher: 'Sourcery',
            version: 'latest',
            installed: false,
            category: 'refactoring',
            icon: '$(wand)'
        },
        {
            id: 'codota.codota',
            name: 'Codota',
            description: 'AI code completions based on millions of programs',
            publisher: 'Codota',
            version: 'latest',
            installed: false,
            category: 'coding',
            icon: '$(symbol-function)'
        },
        {
            id: 'cursorcloud.cursor',
            name: 'Cursor',
            description: 'AI-first code editor built for pair programming',
            publisher: 'Cursor',
            version: 'latest',
            installed: false,
            category: 'chat',
            icon: '$(edit)'
        },
        {
            id: 'bito.bito',
            name: 'Bito',
            description: '10x dev with AI - generate code, tests, and more',
            publisher: 'Bito',
            version: 'latest',
            installed: false,
            category: 'coding',
            icon: '$(zap)'
        },
        {
            id: 'amazonwebservices.amazon-q-vscode',
            name: 'Amazon Q',
            description: 'AWS-powered AI assistant for developers',
            publisher: 'Amazon Web Services',
            version: 'latest',
            installed: false,
            category: 'chat',
            icon: '$(cloud)'
        },
        {
            id: 'codegpt.codegpt',
            name: 'CodeGPT',
            description: 'Use GPT models inside VSCode for code generation',
            publisher: 'CodeGPT',
            version: 'latest',
            installed: false,
            category: 'chat',
            icon: '$(brain)'
        },
        {
            id: 'genieai.genieai',
            name: 'Genie AI',
            description: 'ChatGPT and GPT-4 integration for VSCode',
            publisher: 'Genie AI',
            version: 'latest',
            installed: false,
            category: 'chat',
            icon: '$(sparkle)'
        },
        {
            id: 'llm-toolbox.llm-toolbox',
            name: 'LLM Toolbox',
            description: 'All-in-one toolkit for working with LLMs',
            publisher: 'LLM Toolbox',
            version: 'latest',
            installed: false,
            category: 'other',
            icon: '$(package)'
        },
        {
            id: 'refactai.refactai',
            name: 'Refact AI',
            description: 'AI-powered code refactoring and completion',
            publisher: 'Refact AI',
            version: 'latest',
            installed: false,
            category: 'refactoring',
            icon: '$(sync)'
        },
        {
            id: 'unit-test-maker.unit-test-maker',
            name: 'Unit Test Maker',
            description: 'AI-powered unit test generation',
            publisher: 'Unit Test Maker',
            version: 'latest',
            installed: false,
            category: 'testing',
            icon: '$(beaker)'
        },
        {
            id: 'aicodec.aicodec',
            name: 'AI Codec',
            description: 'AI-powered code explanation and documentation',
            publisher: 'AI Codec',
            version: 'latest',
            installed: false,
            category: 'other',
            icon: '$(book)'
        }
    ];

    constructor(private context: vscode.ExtensionContext) {
        this.checkInstalledPlugins();
    }

    private checkInstalledPlugins(): void {
        const extensions = vscode.extensions.all;
        this.availablePlugins.forEach(plugin => {
            const ext = extensions.find(e => e.id.toLowerCase() === plugin.id.toLowerCase());
            plugin.installed = !!ext;
        });
    }

    getAvailablePlugins(): AIPlugin[] {
        return this.availablePlugins;
    }

    getPluginsByCategory(category: AIPlugin['category']): AIPlugin[] {
        return this.availablePlugins.filter(p => p.category === category);
    }

    async installPlugin(plugin: AIPlugin): Promise<void> {
        try {
            await vscode.commands.executeCommand('workbench.extensions.installExtension', plugin.id);
            plugin.installed = true;
            this.context.globalState.update(`plugin_${plugin.id}`, {
                installed: true,
                installedAt: new Date().toISOString()
            });
        } catch (error) {
            throw new Error(`Failed to install ${plugin.name}: ${error}`);
        }
    }

    async uninstallPlugin(plugin: AIPlugin): Promise<void> {
        try {
            await vscode.commands.executeCommand('workbench.extensions.uninstallExtension', plugin.id);
            plugin.installed = false;
        } catch (error) {
            throw new Error(`Failed to uninstall ${plugin.name}: ${error}`);
        }
    }

    async enablePlugin(plugin: AIPlugin): Promise<void> {
        try {
            const ext = vscode.extensions.getExtension(plugin.id);
            if (ext) {
                await ext.activate();
            }
        } catch (error) {
            throw new Error(`Failed to enable ${plugin.name}: ${error}`);
        }
    }

    getInstalledPlugins(): AIPlugin[] {
        return this.availablePlugins.filter(p => p.installed);
    }

    getRecommendedPlugins(): AIPlugin[] {
        return this.availablePlugins.slice(0, 5);
    }
}
