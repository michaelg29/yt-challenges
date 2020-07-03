const { appData, setResponse } = require("../renderer");
const jQuery = require("jquery");

setResponse("events.renderer.page.init", (e, data) => {
    jQuery("#text").text(data.text);
});