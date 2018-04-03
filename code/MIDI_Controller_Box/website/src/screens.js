const screenPairing = document.getElementById("screenPairing");
const mainDiv = document.getElementById("mainDiv");
const spinningRecord = document.getElementById("spinningRecord");
const startStopDiv = document.getElementById("startStopDiv");
const sampleName = document.getElementById("sampleName");
const audioDiv = document.getElementById("audioDiv");
const audioVisualize = document.getElementById("audioVisualize");
const audioName = document.getElementById("audioName");
const audioArtist = document.getElementById("audioArtist");
const audioLength = document.getElementById("audioLength");
const gifSpeed = document.getElementById("gifSpeed");
const gifWheel = document.getElementsByTagName("circle")[0];
const volumeSlider = document.getElementById("volumeSlider");
const volumeText = document.getElementById("volumeText");
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
