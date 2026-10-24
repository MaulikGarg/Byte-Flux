// SETTINGS + BUTTONS: Connects the FLUX and "TRANSFER ANOTHER" buttons to theirhandlers.

const PROGRESS_INTERVAL = 250; // how often (ms) the progress screen refreshes

document.getElementById("flux-btn").addEventListener("click", onFlux);
document.getElementById("again-btn").addEventListener("click", onAgain);

// SERVER KEEP-ALIVE, The page talks to the server in the background so the server knows whether the page is still open.

// tab/window is closing: tell the server
window.addEventListener("beforeunload", () => fetch("/quit"));

// page is open: ping the server every 3 seconds
setInterval(() => fetch("/heartbeat"), 3000);

// TIME FORMATTING, Turns a number of seconds into text like "1Hours 5Mins 3Secs".

function formatTimeRemaining(totalSeconds) {
  if (totalSeconds < 0) return "Unknown time remaining";

  // 86400 = seconds in a day, 3600 = in an hour, 60 = in a minute
  const days = Math.floor(totalSeconds / 86400);
  const hours = Math.floor((totalSeconds % 86400) / 3600);
  const minutes = Math.floor((totalSeconds % 3600) / 60);
  const seconds = totalSeconds % 60;

  // only show the bigger units once they matter, so you never see "0Days"
  const parts = [];
  if (days > 0) parts.push(`${days}Days`);
  if (hours > 0 || days > 0) parts.push(`${hours}Hours`);
  if (minutes > 0 || hours > 0 || days > 0) parts.push(`${minutes}Mins`);
  parts.push(`${seconds}Secs`);

  return parts.join(" ");
}

// transfer

// FLUX button: reads the form, runs the transfer, shows the result
async function onFlux() {
  const src = document.getElementById("src").value;
  const dst = document.getElementById("dst").value;
  const mode = document.getElementById("mode-input").value;

  // do nothing if either path is empty
  if (src === "" || dst === "") return;

  ui_stage2();

  // refresh the progress screen while the transfer runs
  const progress_poll = setInterval(updateProgress, PROGRESS_INTERVAL);

  // this request only finishes when the whole transfer is done
  const res = await fetch(`/transfer?src=${src}&dst=${dst}&mode=${mode}`, {
    method: "POST",
  });
  const result = await res.json();

  ui_stage3(result.j_success, result.j_fatal_error, result.j_file_errors);

  // stop polling only after the result screen is showing
  clearInterval(progress_poll);
}

// runs every PROGRESS_INTERVAL ms during a transfer:
// asks the server for progress and updates the progress screen
async function updateProgress() {
  const res = await fetch("/progress");
  const progress = await res.json();

  const bps = parseInt(progress.j_speed_bps) || 0;
  const mbps = (bps / (1024 * 1024)).toFixed(1); // bytes/sec -> MB/s, one decimal
  const timeLeft = formatTimeRemaining(parseInt(progress.j_time_remaining_s));

  ui_set_progress(progress.j_percent, mbps, timeLeft);
}

// "TRANSFER ANOTHER" button: back to the input form
function onAgain() {
  ui_stage1();
}
