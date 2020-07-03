const { app, BrowserWindow } = require("electron");

const mainProcess = require("./main");

const appData = require("../assets/data/app.json");
const main = require("./main");
global.appData = appData;

app.name = appData["app.name"];

let win;

function createWindow() {
    win = new BrowserWindow({
        width: 1200,
        height: 800,
        webPreferences: {
            nodeIntegration: true,
            enableRemoteModule: true
        },
        icon: appData["app.icon"],
        title: appData["app.name"]
    });

    // load main file
    win.loadFile(appData["app.entry"]);

    // open dev tools
    win.webContents.openDevTools();

    win.on("closed", () => {
        // dereference object
        win = null;
    });
}

// start app
app.on("ready", createWindow);

app.on("window-all-closed", () => {
    if (process.platform != "darwin") {
        app.quit();
    }
});

app.on("activate", () => {
    if (win == null) {
        createWindow();
    }
});

mainProcess.setResponse("events.renderer.ready", () => {
    mainProcess.sendData("events.renderer.route.request", {
        route: appData["app.initialRoute"],
        args: {
            text: "this is the test from the main bois"
        }
    })
});