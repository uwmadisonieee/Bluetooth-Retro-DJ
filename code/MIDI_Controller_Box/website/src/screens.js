var screenMode = 0;
const screenPairing = document.getElementById("screenPairing");
const mainDiv = document.getElementById("mainDiv");
const spinningRecord = document.getElementById("spinningRecord");
const startStopDiv = document.getElementById("startStopDiv");
const sampleDiv = document.getElementById("sampleDiv");
const audioDiv = document.getElementById("audioDiv");
const audioVisualize = document.getElementById("audioVisualize");
const audioName = document.getElementById("audioName");
const gifDiv = document.getElementById("gifDiv");
const gifSwap = document.getElementById("gifSwap");
const gif0 = document.getElementById("gif0");
const gif1 = document.getElementById("gif1");
const gif2 = document.getElementById("gif2");
const gifSpeed = document.getElementById("gifSpeed");
const volumeDiv = document.getElementById("volumeDiv");
const songSelect = document.getElementById("songSelect");
const fxSelect = document.getElementById("fxSelect");

function screenGoto(index) {
    screenPairing.style.display = "none";
    mainDiv.style.display = "none";

    if (index == 0) {
	screenPairing.style.display = "";
    } else if (index == 1) {
	mainDiv.style.display = "";
    }
}
