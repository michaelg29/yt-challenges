const jQuery = require("jquery");
const { ipcRenderer, remote } = require("electron");

// get global app data
const appData = remote.getGlobal("appData");
const routes = require("./html/routes.json");

/*
    Signals
*/

// send alert to backend
function sendAlert(alert) {
    ipcRenderer.send(alert);
}

// send data to backend
function sendData(alert, data) {
    ipcRenderer.send(alert, data);
}

// set response to event
function setResponse(trigger, response) {
    ipcRenderer.on(trigger, response);
}

// trigger frontend event
function trigger(event) {
    remote.BrowserWindow.getFocusedWindow().webContents.send(alert);
}

// trigger frontend event with data
function triggerWithData(alert, data) {
    remote.BrowserWindow.getFocusedWindow().webContents.send(alert, data);
}

/*
    Routing
*/

// load html page
function goto(route) {
    let path;
    if (typeof route == "string") {
        // route passed in as string
        path = route;
    } else {
        // route passed in as dictionary
        path = route.route;
    }

    if (Object.keys(routes).includes(path)) {
        // path contained in dictionary
        jQuery("main").load(routes[path]);
        if (typeof route != "string" && Object.keys(route).includes("args")) {
            triggerWithData("events.renderer.page.init", route.args);
        }
        return true;
    } else {
        return false;
    }
}

// listener for main process signal
setResponse("events.renderer.route.request", (e, route) => {
    sendData(goto(route) ? "events.renderer.route.success" : "events.renderer.route.failure", route);
});

/*
    Exports
*/
module.exports = {
    appData,
    sendAlert,
    sendData,
    setResponse,
    trigger,
    triggerWithData,
    goto
}

/*
    Runtime
*/
sendAlert("events.renderer.ready");