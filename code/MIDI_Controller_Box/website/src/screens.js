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
const instructionSelect = document.getElementById("instructionSelect");
const selectSongUp = document.getElementById("selectSongUp");
const selectSongDown = document.getElementById("selectSongDown");
const selectSong = [
	document.getElementById("selectSong0"),
	document.getElementById("selectSong1"),
	document.getElementById("selectSong2"),
	document.getElementById("selectSong3"),
	document.getElementById("selectSong4") ];
const selectSample = [
	document.getElementById("selectSample0"),
	document.getElementById("selectSample1"),
	document.getElementById("selectSample2"),
	document.getElementById("selectSample3"),
	document.getElementById("selectSample4") ];

const menuColorNo = "rgba(100,181,246,0.6)";
const menuColorYes = "rgba(33,150,243,1.0)";
const menuColorSelect = "rgba(21,101,192,1.0)";

function screenGoto(index) {
    screenPairing.style.display = "none";
    mainDiv.style.display = "none";

    if (index == 0) {
	screenPairing.style.display = "";
    } else if (index == 1) {
	mainDiv.style.display = "";
    }
}

var menuMode = 0;
function showMenu() {
	menuMode = 1;
	toggleMenu();
	songSelect.style.left = "0px";
	fxSelect.style.right = "0px";
	instructionSelect.style.bottom = "0px";
}

function hideMenu() {
	menuMode = 0;
	songSelect.style.left = "-300px";
	fxSelect.style.right = "-300px";
	instructionSelect.style.bottom = "-480px";
}

function toggleMenu() {
	menuSelected ^= 1;
	songSelect.style.backgroundColor = (menuSelected) ? menuColorYes : menuColorNo;
	fxSelect.style.backgroundColor = (menuSelected) ? menuColorNo : menuColorYes;
}