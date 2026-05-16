const { ipcRenderer } = require('electron');
const path = require('path');

let editor = null;
let currentFile = null;
let openTabs = [];
let folderFiles = [];
let currentFolder = null;

const fileIcons = {
  js: '📜',
  jsx: '⚛️',
  ts: '📘',
  tsx: '⚛️',
  py: '🐍',
  html: '🌐',
  css: '🎨',
  json: '📋',
  md: '📝',
  txt: '📄',
  default: '📄'
};

const aiPlugins = [
  { name: 'GitHub Copilot', desc: 'AI 结对编程助手，提供智能代码补全', enabled: true },
  { name: 'Codeium', desc: '免费的 AI 代码补全工具', enabled: false },
  { name: 'Tabnine', desc: '基于机器学习的智能代码补全', enabled: false },
  { name: 'Cursor', desc: 'AI 优先的代码编辑器集成', enabled: false },
  { name: 'Continue', desc: '开源 AI 代码助手，支持本地模型', enabled: false },
  { name: 'Amazon Q', desc: 'AWS 推出的 AI 开发助手', enabled: false },
  { name: 'Sourcery', desc: 'AI 驱动的代码重构工具', enabled: false }
];

require(['vs/editor/editor.main'], function () {
  initEditor();
  initEventListeners();
  loadPluginsList();
});

function initEditor() {
  editor = monaco.editor.create(document.getElementById('monaco-editor'), {
    value: '',
    language: 'javascript',
    theme: 'vs-dark',
    fontSize: 14,
    minimap: { enabled: true },
    automaticLayout: true,
    lineNumbers: 'on',
    scrollBeyondLastLine: false,
    wordWrap: 'on',
    bracketPairColorization: { enabled: true },
    guides: { bracketPairs: true },
    suggest: {
      showKeywords: true,
      showSnippets: true,
      showFiles: true
    }
  });

  editor.onDidChangeCursorPosition((e) => {
    const statusBar = document.querySelector('.status-bar');
    statusBar.innerHTML = `
      <span class="status-item">UTF-8</span>
      <span class="status-item">${getLanguageName()}</span>
      <span class="status-item">Ln ${e.position.lineNumber}, Col ${e.position.column}</span>
    `;
  });

  updateEditorVisibility();
}

function getLanguageName() {
  if (!currentFile) return 'Plain Text';
  const ext = path.extname(currentFile.filename).slice(1);
  const langMap = {
    js: 'JavaScript',
    jsx: 'JavaScript React',
    ts: 'TypeScript',
    tsx: 'TypeScript React',
    py: 'Python',
    html: 'HTML',
    css: 'CSS',
    json: 'JSON',
    md: 'Markdown'
  };
  return langMap[ext] || 'Plain Text';
}

function initEventListeners() {
  document.querySelectorAll('.activity-item').forEach(item => {
    item.addEventListener('click', () => {
      const panel = item.dataset.panel;
      const action = item.dataset.action;

      if (action === 'ai') {
        toggleAIPanel();
      } else if (panel) {
        document.querySelectorAll('.activity-item').forEach(i => i.classList.remove('active'));
        item.classList.add('active');

        document.querySelectorAll('.sidebar').forEach(p => p.classList.remove('active'));
        document.getElementById(`${panel}-panel`).classList.add('active');
      }
    });
  });

  ipcRenderer.on('new-file', () => {
    createNewTab('Untitled', '');
  });

  ipcRenderer.on('file-opened', (event, data) => {
    createNewTab(data.filename, data.content, data.path);
  });

  ipcRenderer.on('folder-opened', (event, folderPath) => {
    loadFolder(folderPath);
  });

  ipcRenderer.on('save-file', () => {
    saveCurrentFile();
  });

  ipcRenderer.on('save-file-as', () => {
    saveFileAs();
  });

  ipcRenderer.on('toggle-ai-panel', () => {
    toggleAIPanel();
  });

  ipcRenderer.on('file-search', () => {
    showSearchModal();
  });

  document.getElementById('search-modal').addEventListener('click', (e) => {
    if (e.target.id === 'search-modal') {
      hideSearchModal();
    }
  });

  document.getElementById('search-input').addEventListener('input', (e) => {
    searchFiles(e.target.value);
  });

  document.getElementById('ai-input').addEventListener('keydown', (e) => {
    if (e.key === 'Enter' && !e.shiftKey) {
      e.preventDefault();
      sendAIMessage();
    }
  });
}

function createNewTab(filename, content, filePath = null) {
  const existingTab = openTabs.find(t => t.path === filePath && filePath);
  if (existingTab) {
    activateTab(existingTab.id);
    return;
  }

  const tabId = Date.now().toString();
  const ext = path.extname(filename).slice(1);
  const language = getMonacoLanguage(ext);

  openTabs.push({
    id: tabId,
    filename: filename,
    path: filePath,
    content: content
  });

  currentFile = openTabs.find(t => t.id === tabId);

  renderTabs();
  activateTab(tabId);
  updateEditorVisibility();
}

function getMonacoLanguage(ext) {
  const langMap = {
    js: 'javascript',
    jsx: 'javascript',
    ts: 'typescript',
    tsx: 'typescript',
    py: 'python',
    html: 'html',
    css: 'css',
    json: 'json',
    md: 'markdown'
  };
  return langMap[ext] || 'plaintext';
}

function renderTabs() {
  const tabsBar = document.getElementById('tabs-bar');
  tabsBar.innerHTML = openTabs.map(tab => `
    <div class="tab ${tab.id === currentFile?.id ? 'active' : ''}" data-tab-id="${tab.id}">
      <span>${fileIcons[path.extname(tab.filename).slice(1)] || fileIcons.default} ${tab.filename}</span>
      <span class="tab-close" onclick="closeTab('${tab.id}', event)">✕</span>
    </div>
  `).join('');

  tabsBar.querySelectorAll('.tab').forEach(tab => {
    tab.addEventListener('click', (e) => {
      if (!e.target.classList.contains('tab-close')) {
        activateTab(tab.dataset.tabId);
      }
    });
  });
}

function activateTab(tabId) {
  const tab = openTabs.find(t => t.id === tabId);
  if (tab) {
    currentFile = tab;
    const ext = path.extname(tab.filename).slice(1);
    const language = getMonacoLanguage(ext);
    
    editor.setValue(tab.content);
    monaco.editor.setModelLanguage(editor.getModel(), language);
    
    renderTabs();
    updateEditorVisibility();
  }
}

function closeTab(tabId, event) {
  event.stopPropagation();
  const index = openTabs.findIndex(t => t.id === tabId);
  if (index > -1) {
    openTabs.splice(index, 1);
    if (currentFile?.id === tabId) {
      if (openTabs.length > 0) {
        activateTab(openTabs[Math.max(0, index - 1)].id);
      } else {
        currentFile = null;
        editor.setValue('');
      }
    }
    renderTabs();
    updateEditorVisibility();
  }
}

function updateEditorVisibility() {
  const welcomeScreen = document.getElementById('welcome-screen');
  const editorContainer = document.getElementById('monaco-editor');
  
  if (openTabs.length === 0) {
    welcomeScreen.style.display = 'flex';
    editorContainer.style.display = 'none';
  } else {
    welcomeScreen.style.display = 'none';
    editorContainer.style.display = 'block';
    editor.layout();
  }
}

async function saveCurrentFile() {
  if (!currentFile) return;
  
  const content = editor.getValue();
  const result = await ipcRenderer.invoke('save-file-dialog', content);
  
  if (result.success) {
    currentFile.path = result.path;
    if (result.filename) {
      currentFile.filename = result.filename;
    }
    currentFile.content = content;
    renderTabs();
  }
}

async function saveFileAs() {
  if (!currentFile) return;
  
  const content = editor.getValue();
  const result = await ipcRenderer.invoke('save-file-as-dialog', content);
  
  if (result.success) {
    currentFile.path = result.path;
    currentFile.filename = result.filename;
    currentFile.content = content;
    renderTabs();
  }
}

async function loadFolder(folderPath) {
  currentFolder = folderPath;
  const result = await ipcRenderer.invoke('scan-folder', folderPath);
  
  if (result.success) {
    folderFiles = result.files;
    renderFileList();
  }
}

function renderFileList() {
  const fileList = document.getElementById('file-list');
  
  const sortedFiles = [...folderFiles].sort((a, b) => {
    return a.relativePath.localeCompare(b.relativePath);
  });

  fileList.innerHTML = sortedFiles.map(file => {
    const ext = path.extname(file.name).slice(1);
    return `
      <div class="file-item" onclick="openFileFromList('${file.path}', '${file.name}')">
        <span class="file-icon">${fileIcons[ext] || fileIcons.default}</span>
        <span>${file.name}</span>
      </div>
    `;
  }).join('');
}

async function openFileFromList(filePath, filename) {
  const result = await ipcRenderer.invoke('read-file', filePath);
  if (result.success) {
    createNewTab(filename, result.content, filePath);
  }
}

function showSearchModal() {
  const modal = document.getElementById('search-modal');
  modal.classList.add('active');
  document.getElementById('search-input').focus();
  searchFiles('');
}

function hideSearchModal() {
  document.getElementById('search-modal').classList.remove('active');
  document.getElementById('search-input').value = '';
}

async function searchFiles(query) {
  const resultsContainer = document.getElementById('search-results');
  
  if (!query.trim()) {
    const recentFiles = folderFiles.slice(0, 10);
    renderSearchResults(recentFiles);
    return;
  }

  const result = await ipcRenderer.invoke('fuzzy-search', folderFiles, query);
  if (result.success) {
    renderSearchResults(result.results.slice(0, 15));
  }
}

function renderSearchResults(files) {
  const resultsContainer = document.getElementById('search-results');
  
  if (files.length === 0) {
    resultsContainer.innerHTML = '<div class="search-result-item">没有找到匹配的文件</div>';
    return;
  }

  resultsContainer.innerHTML = files.map(file => {
    const ext = path.extname(file.name).slice(1);
    return `
      <div class="search-result-item" onclick="openFileFromList('${file.path}', '${file.name}'); hideSearchModal();">
        <span class="search-result-icon">${fileIcons[ext] || fileIcons.default}</span>
        <span>${file.name}</span>
        <span class="search-result-path">${file.relativePath}</span>
      </div>
    `;
  }).join('');
}

function toggleAIPanel() {
  const panel = document.getElementById('ai-panel');
  panel.classList.toggle('active');
}

function loadPluginsList() {
  const pluginsList = document.getElementById('plugins-list');
  pluginsList.innerHTML = aiPlugins.map(plugin => `
    <div class="plugin-item">
      <div class="plugin-name">${plugin.name}</div>
      <div class="plugin-desc">${plugin.desc}</div>
      <div class="plugin-status">${plugin.enabled ? '✓ 已启用' : '○ 已禁用'}</div>
    </div>
  `).join('');
}

async function sendAIMessage() {
  const input = document.getElementById('ai-input');
  const message = input.value.trim();
  
  if (!message) return;
  
  addChatMessage(message, 'user');
  input.value = '';
  
  setTimeout(() => {
    const responses = [
      `这是一个关于 "${message}" 的代码示例：\n\n\`\`\`javascript\nfunction example() {\n  // AI 生成的代码\n  return "Hello World";\n}\n\`\`\``,
      `我理解你想要实现 "${message}"。以下是我的建议：\n\n1. 首先确定需求范围\n2. 设计代码结构\n3. 实现核心功能\n4. 添加错误处理`,
      `让我帮你分析这个问题。关于 "${message}"，我建议：\n\n• 使用模块化的方式组织代码\n• 添加适当的注释\n• 编写单元测试确保质量`
    ];
    
    const randomResponse = responses[Math.floor(Math.random() * responses.length)];
    addChatMessage(randomResponse, 'ai');
  }, 1000);
}

function addChatMessage(content, type) {
  const messagesContainer = document.getElementById('chat-messages');
  const messageDiv = document.createElement('div');
  messageDiv.className = `message ${type}`;
  messageDiv.textContent = content;
  messagesContainer.appendChild(messageDiv);
  messagesContainer.scrollTop = messagesContainer.scrollHeight;
}

function openFile() {
  document.querySelector('[label="打开文件"]').click();
}

function openFolder() {
  document.querySelector('[label="打开文件夹"]').click();
}

window.openFileFromList = openFileFromList;
window.closeTab = closeTab;
window.showSearchModal = showSearchModal;
window.hideSearchModal = hideSearchModal;
window.toggleAIPanel = toggleAIPanel;
window.openFile = openFile;
window.openFolder = openFolder;
window.sendAIMessage = sendAIMessage;
