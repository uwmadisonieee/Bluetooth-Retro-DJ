var screenMode = 0;
const screenPairing = document.getElementById("screenPairing");
const screenTracks = document.getElementById("screenTracks");

function screenGoto(index) {
    screenPairing.style.display = "none";
    screenTracks.style.display = "none";

    if (index == 0) {
	screenPairing.style.display = "";
    } else if (index == 1) {
	screenTracks.style.display = "";
    }
}
