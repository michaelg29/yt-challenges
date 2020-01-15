var inelement = document.querySelector("#input");
var outelement = document.querySelector("#output");
var btn = document.querySelector("#btn");

var canvas = document.querySelector("#canvas");
var ctx = canvas.getContext("2d");

// align canvas properly
ctx.translate(0, canvas.height);
ctx.rotate(Math.PI);
ctx.scale(-1, 1);

var equations = [];
var global_scale = 100; // pixels per unit on axis
var minX = -1;
var minY = -1;
var dx;

var rect;
var width;
var height;
var left;
var bottom;
var cursorX; // x coordinate of cursor in pixels
var cursorY; // y coordinate of cursor in pixels
var cursor_x_val; // x val of cursor
var cursor_y_val; // y val of cursor

var lastX;
var lastY;

var mouseDown;

function updateRect() {
    rect = canvas.getBoundingClientRect();
    width = rect.width;
    height = rect.height;
    left = rect.left;
    bottom = rect.bottom;
}

/*
    listeners
*/

// mouse goes down
canvas.addEventListener("mousedown", (e) => {
    cursorX = e.clientX;
    cursorY = e.clientY;

    mouseDown = true;

    updateRect(e);

    lastX = cursorX;
    lastY = cursorY;
});
// mouse goes up
['mouseup', 'mouseleave'].forEach((event) =>
    canvas.addEventListener(event, (e) => mouseDown = false)
);

// mousemove
canvas.addEventListener("mousemove", (e) => {
    cursorX = e.clientX;
    cursorY = e.clientY;

    outelement.innerHTML = `${minX + cursorX / global_scale} ${minY + cursorY / global_scale}`;

    if (!mouseDown) {
        return;
    }

    updateRect(e);

    // translate minX and minY
    var _dx = cursorX - lastX;
    var _dy = cursorY - lastY;

    minX -= _dx / global_scale;
    minY += _dy / global_scale;

    lastX = cursorX;
    lastY = cursorY;

    redraw();
});

// scroll for zoom
canvas.addEventListener("wheel", (e) => {
    var dy = -e.deltaY;

    var x = minX + cursorX / global_scale;
    var y = minY + cursorY / global_scale;

    if (dy > 0) {
        // scale up
        global_scale *= 1.05;
    } else if (dy < 0) {
        // scale down
        global_scale *= .95;
    }

    // update minX, minY
    minX = x - cursorX / global_scale;
    minY = y - cursorY / global_scale;

    redraw();
});

// submit button click
btn.addEventListener('click', (e) => {
    let eqn = inelement.value;
    let rpn = RPN(eqn);

    var out = 'invalid input';

    if (rpn) {
        let tree = parse(rpn);
        console.log(tree);
        equations.push({
            eqn: eqn,
            tree: tree
        });
        draw(tree);
        out = `Graphing y = ${eqn}`;
    }

    outelement.innerHTML = out;
});

/*
    drawing functions
*/

// draws equation on canvas
function draw(tree) {
    var i = 0;
    dx = 1 / global_scale;

    var y;
    variables.x = minX;

    var width = canvas.width;

    // find first defined point on graph
    while ((y = eval(tree)) === NaN && i < width) {
        variables.x = minX + i * dx;
        i++;
    }

    let previousY = (y - minY) * global_scale;

    for (; i < width; i++) {
        variables.x = minX + i * dx;
        y = eval(tree);

        if (y === NaN) {
            console.log(`discontinuity at x = ${x}`);
            // detect dicontinuities
            while ((y = eval(tree)) === NaN && i < width) {
                variables.x = minX + i * dx;
                i++;
            }
            previousY = (y - minY) * global_scale;
            continue;
        }

        y = (y - minY) * global_scale;

        // draw line
        ctx.beginPath();
        ctx.moveTo(i - 1, previousY); // previous coordinate
        ctx.lineTo(i, y); // current coordinate
        ctx.lineWidth = 2;
        ctx.stroke();
        previousY = y;
    }
}

function drawAxes() {
    if (minX >= -canvas.width / global_scale && minX <= canvas.width / global_scale) {
        // draw y-axis
        ctx.beginPath();
        ctx.moveTo(-minX * global_scale, 0);
        ctx.lineTo(-minX * global_scale, canvas.height);
        ctx.lineWidth = 5;
        ctx.stroke();
    }

    if (minY >= -canvas.height / global_scale && minY <= canvas.height / global_scale) {
        // draw x-axis
        ctx.beginPath();
        ctx.moveTo(0, -minY * global_scale);
        ctx.lineTo(canvas.width, -minY * global_scale);
        ctx.lineWidth = 5;
        ctx.stroke();
    }
}

function clearCanvas() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
}

function redraw() {
    clearCanvas();

    drawAxes();

    Array.from(equations).forEach((eqn) => draw(eqn.tree));
}

function reset() {
    equations = [];
    clearCanvas();
}

updateRect();
drawAxes();