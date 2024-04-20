const { app, BrowserWindow, Menu } = require('electron');
const path = require('node:path');

Menu.setApplicationMenu(null);

const createWindow = () => {
    const win = new BrowserWindow({
      width: 800,
      height: 600,
      icon: path.join(__dirname, "assets", "icon.png"),
      webPreferences: {
        preload: path.join(__dirname, 'preload.js')
      }
    });
    win.loadFile('index.html');
}

app.whenReady().then(() => {
  app.on('activate', () => {
      if (BrowserWindow.getAllWindows().length === 0) createWindow();
  });
});

app.on('window-all-closed', () => {
    if (process.platform !== 'darwin') app.quit();
});

app.whenReady().then(() => {
  createWindow()
    app.on('activate', () => {
      if (BrowserWindow.getAllWindows().length === 0) createWindow();
  })
});