const { app, BrowserWindow, Menu, ipcMain, dialog } = require('electron');
const path = require('path');
const fs = require('fs');
const Fuse = require('fuse.js');

let mainWindow;
let currentFilePath = null;

function createWindow() {
  mainWindow = new BrowserWindow({
    width: 1400,
    height: 900,
    minWidth: 800,
    minHeight: 600,
    webPreferences: {
      nodeIntegration: true,
      contextIsolation: false,
      webSecurity: false
    },
    icon: path.join(__dirname, '../assets/icon.png'),
    titleBarStyle: 'default',
    frame: true
  });

  mainWindow.loadFile(path.join(__dirname, 'index.html'));

  if (process.argv.includes('--dev')) {
    mainWindow.webContents.openDevTools();
  }

  mainWindow.on('closed', () => {
    mainWindow = null;
  });
}

function createMenu() {
  const template = [
    {
      label: '文件',
      submenu: [
        {
          label: '新建文件',
          accelerator: 'Ctrl+N',
          click: () => {
            mainWindow.webContents.send('new-file');
          }
        },
        {
          label: '打开文件',
          accelerator: 'Ctrl+O',
          click: async () => {
            const result = await dialog.showOpenDialog(mainWindow, {
              properties: ['openFile'],
              filters: [
                { name: 'All Files', extensions: ['*'] },
                { name: 'JavaScript', extensions: ['js', 'jsx'] },
                { name: 'TypeScript', extensions: ['ts', 'tsx'] },
                { name: 'Python', extensions: ['py'] },
                { name: 'HTML', extensions: ['html', 'htm'] },
                { name: 'CSS', extensions: ['css'] },
                { name: 'JSON', extensions: ['json'] }
              ]
            });
            if (!result.canceled && result.filePaths.length > 0) {
              const content = fs.readFileSync(result.filePaths[0], 'utf8');
              currentFilePath = result.filePaths[0];
              mainWindow.webContents.send('file-opened', {
                path: result.filePaths[0],
                content: content,
                filename: path.basename(result.filePaths[0])
              });
            }
          }
        },
        {
          label: '打开文件夹',
          accelerator: 'Ctrl+K Ctrl+O',
          click: async () => {
            const result = await dialog.showOpenDialog(mainWindow, {
              properties: ['openDirectory']
            });
            if (!result.canceled && result.filePaths.length > 0) {
              mainWindow.webContents.send('folder-opened', result.filePaths[0]);
            }
          }
        },
        { type: 'separator' },
        {
          label: '保存',
          accelerator: 'Ctrl+S',
          click: () => {
            mainWindow.webContents.send('save-file');
          }
        },
        {
          label: '另存为',
          accelerator: 'Ctrl+Shift+S',
          click: () => {
            mainWindow.webContents.send('save-file-as');
          }
        },
        { type: 'separator' },
        {
          label: '退出',
          accelerator: 'Ctrl+Q',
          click: () => {
            app.quit();
          }
        }
      ]
    },
    {
      label: '编辑',
      submenu: [
        { label: '撤销', accelerator: 'Ctrl+Z', role: 'undo' },
        { label: '重做', accelerator: 'Ctrl+Y', role: 'redo' },
        { type: 'separator' },
        { label: '剪切', accelerator: 'Ctrl+X', role: 'cut' },
        { label: '复制', accelerator: 'Ctrl+C', role: 'copy' },
        { label: '粘贴', accelerator: 'Ctrl+V', role: 'paste' },
        { type: 'separator' },
        { label: '全选', accelerator: 'Ctrl+A', role: 'selectAll' },
        { type: 'separator' },
        {
          label: '查找',
          accelerator: 'Ctrl+F',
          click: () => {
            mainWindow.webContents.send('find');
          }
        },
        {
          label: '文件搜索',
          accelerator: 'Ctrl+P',
          click: () => {
            mainWindow.webContents.send('file-search');
          }
        }
      ]
    },
    {
      label: '视图',
      submenu: [
        { label: '放大', accelerator: 'Ctrl+=', role: 'zoomIn' },
        { label: '缩小', accelerator: 'Ctrl+-', role: 'zoomOut' },
        { label: '重置缩放', accelerator: 'Ctrl+0', role: 'resetZoom' },
        { type: 'separator' },
        { label: '切换全屏', accelerator: 'F11', role: 'togglefullscreen' },
        {
          label: '开发者工具',
          accelerator: 'Ctrl+Shift+I',
          role: 'toggleDevTools'
        }
      ]
    },
    {
      label: 'AI 助手',
      submenu: [
        {
          label: '打开 AI 面板',
          accelerator: 'Ctrl+Shift+A',
          click: () => {
            mainWindow.webContents.send('toggle-ai-panel');
          }
        },
        {
          label: 'AI 代码补全',
          accelerator: 'Ctrl+Space',
          click: () => {
            mainWindow.webContents.send('ai-complete');
          }
        },
        {
          label: 'AI 设置',
          accelerator: 'Ctrl+,',
          click: () => {
            mainWindow.webContents.send('ai-settings');
          }
        },
        { type: 'separator' },
        {
          label: '切换 AI 环境',
          click: () => {
            mainWindow.webContents.send('switch-ai-env');
          }
        }
      ]
    },
    {
      label: '帮助',
      submenu: [
        {
          label: '关于',
          click: () => {
            dialog.showMessageBox(mainWindow, {
              type: 'info',
              title: '关于 AI Editor Pro',
              message: 'AI Editor Pro v1.0.0',
              detail: '基于 Electron 和 Monaco Editor 的 AI 驱动代码编辑器\n\n支持文件搜索、AI 助手、插件扩展等功能'
            });
          }
        }
      ]
    }
  ];

  const menu = Menu.buildFromTemplate(template);
  Menu.setApplicationMenu(menu);
}

app.whenReady().then(() => {
  createWindow();
  createMenu();

  app.on('activate', () => {
    if (BrowserWindow.getAllWindows().length === 0) {
      createWindow();
    }
  });
});

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit();
  }
});

ipcMain.handle('save-file-dialog', async (event, content) => {
  if (currentFilePath) {
    fs.writeFileSync(currentFilePath, content);
    return { success: true, path: currentFilePath };
  } else {
    const result = await dialog.showSaveDialog(mainWindow, {
      filters: [
        { name: 'All Files', extensions: ['*'] }
      ]
    });
    if (!result.canceled && result.filePath) {
      fs.writeFileSync(result.filePath, content);
      currentFilePath = result.filePath;
      return { success: true, path: result.filePath, filename: path.basename(result.filePath) };
    }
    return { success: false };
  }
});

ipcMain.handle('save-file-as-dialog', async (event, content) => {
  const result = await dialog.showSaveDialog(mainWindow, {
    filters: [
      { name: 'All Files', extensions: ['*'] }
    ]
  });
  if (!result.canceled && result.filePath) {
    fs.writeFileSync(result.filePath, content);
    currentFilePath = result.filePath;
    return { success: true, path: result.filePath, filename: path.basename(result.filePath) };
  }
  return { success: false };
});

ipcMain.handle('read-file', async (event, filePath) => {
  try {
    const content = fs.readFileSync(filePath, 'utf8');
    return { success: true, content };
  } catch (error) {
    return { success: false, error: error.message };
  }
});

ipcMain.handle('scan-folder', async (event, folderPath) => {
  try {
    const files = [];
    const scanDir = (dir, relativePath = '') => {
      const items = fs.readdirSync(dir);
      for (const item of items) {
        const fullPath = path.join(dir, item);
        const relPath = path.join(relativePath, item);
        const stats = fs.statSync(fullPath);
        if (stats.isDirectory()) {
          if (!['node_modules', '.git', 'dist', 'build'].includes(item)) {
            scanDir(fullPath, relPath);
          }
        } else {
          files.push({
            name: item,
            path: fullPath,
            relativePath: relPath,
            size: stats.size,
            modified: stats.mtime
          });
        }
      }
    };
    scanDir(folderPath);
    return { success: true, files };
  } catch (error) {
    return { success: false, error: error.message };
  }
});

ipcMain.handle('fuzzy-search', async (event, files, query) => {
  const fuse = new Fuse(files, {
    keys: ['name', 'relativePath'],
    threshold: 0.3,
    location: 0,
    distance: 100
  });
  const results = fuse.search(query);
  return { success: true, results: results.map(r => r.item) };
});
