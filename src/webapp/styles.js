/* BACKGROUND ANIMATION
   Draws the scrolling rows of 0/1 digits on <canvas id="bg">. It runs on its own and never touches the stages. 
   Nothing in byteflux.js calls anything in this section.*/

const ROW_COUNT = 14;
const FONT_SIZE = 24; // digit size in px
const CHAR_W = FONT_SIZE * 0.65; // horizontal space each digit takes
const SPEED = 35; // px per second for a row at speed 1

const canvas = document.getElementById("bg");
const ctx = canvas.getContext("2d");
let rows = [];

// runs on load and on every window resize:
// fits the canvas to the window and rebuilds the rows
function resize() {
  canvas.width = window.innerWidth;
  canvas.height = window.innerHeight;

  rows = [];
  const rowH = canvas.height / ROW_COUNT; // row height
  for (let i = 0; i < ROW_COUNT; i++) {
    rows.push({
      y: rowH * i + rowH * 0.5, // vertical centre of this row
      speed: (0.5 + Math.random() * 0.5) * (i % 2 === 0 ? 1 : -1), // even rows move right, odd rows move left
      offset: Math.random() * 100, // random start so rows don't line up
    });
  }
}

// runs every frame: clears the canvas and repaints every row
function draw() {
  ctx.clearRect(0, 0, canvas.width, canvas.height);
  ctx.font = `${FONT_SIZE}px "Space Mono", monospace`;
  ctx.fillStyle = "rgba(0,255,200,0.12)";

  const t = Date.now() / 1000; // time in seconds
  const count = Math.floor(canvas.width / CHAR_W) + 2; // digits per row, plus 2 to cover the edges

  for (const row of rows) {
    const shift =
      (((row.offset + t * row.speed * SPEED) % CHAR_W) + CHAR_W) % CHAR_W;

    // right-moving rows start one cell off the left edge,
    // left-moving rows start at 0 and slide left
    const startX = row.speed > 0 ? -CHAR_W + shift : -shift;

    // alternate 0 and 1 across the row
    for (let c = 0; c < count; c++) {
      ctx.fillText(c % 2 ? "1" : "0", startX + c * CHAR_W, row.y);
    }
  }

  requestAnimationFrame(draw);
}

window.addEventListener("resize", resize);
resize();
draw();

/* STAGE HELPERS
   Called from byteflux.js (ui_*) and from the onclick buttons in index.html (setMode). 
   They only change what is on screen: which stage is visible, plus text and colors inside stages.*/

function setMode(m) {
  document.getElementById("btn-copy").classList.toggle("active", m === "copy");
  document.getElementById("btn-move").classList.toggle("active", m === "move");
  document.getElementById("mode-input").value = m;
}

// shows one stage and hides the others by switching activvvvve class
function showStage(id) {
  document
    .querySelectorAll(".stage")
    .forEach((s) => s.classList.toggle("active", s.id === id));
}

// initial screen. resets the result and progress text
function ui_stage1() {
  showStage("s1");
  const log = document.getElementById("log");
  log.style.display = "none";
  log.innerHTML = "";
  document.getElementById("pct").textContent = "0%";
  document.getElementById("speed").textContent = "0 MB/s";
}

// progress screen
function ui_stage2() {
  showStage("s2");
}

// result screen
function ui_stage3(success, fatal_error, file_errors) {
  showStage("s3");
  const icon = document.getElementById("icon");
  const log = document.getElementById("log");

  if (!success) {
    // whole transfer failed
    icon.textContent = "✗";
    icon.style.color = "#ff4040";
    log.style.display = "flex";
    log.innerHTML = `<div class="log-entry">▸ ${fatal_error}</div>`;
  } else if (file_errors.length) {
    // finished, but some files failed
    icon.textContent = "⚠";
    icon.style.color = "#ffaa00";
    log.style.display = "flex";
    log.innerHTML = file_errors
      .map((e) => `<div class="log-entry">▸ ${e}</div>`)
      .join("");
  } else {
    // everything worked, log stays hidden
    icon.textContent = "✓";
    icon.style.color = "#00ffc8";
  }
}

// called every 250ms by the poll in onFlux: rewrites the three progress texts
function ui_set_progress(pct, mbps, timerem_string) {
  document.getElementById("pct").textContent = pct + "%";
  document.getElementById("speed").textContent = mbps + " MB/s";
  document.getElementById("timeleft").textContent = timerem_string;
}
