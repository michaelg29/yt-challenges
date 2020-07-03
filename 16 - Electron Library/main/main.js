const { app, BrowserWindow, ipcMain } = require("electron");

/*
    Signals
*/

// send alert to frontend
function sendAlert(alert) {
    BrowserWindow.getFocusedWindow().webContents.send(alert);
}

// send data to frontend
function sendData(alert, data) {
    BrowserWindow.getFocusedWindow().webContents.send(alert, data);
}

// set response to event
function setResponse(trigger, response) {
    ipcMain.on(trigger, response);
}

/*
    Exports
*/

module.exports = {
    sendAlert,
    sendData,
    setResponse
};