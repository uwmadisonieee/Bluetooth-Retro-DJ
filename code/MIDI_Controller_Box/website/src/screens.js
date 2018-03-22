const screenPairing = document.getElementById("screenPairing");

function screenGoto(index) {
	screenPairing.style.visibility = "hidden";

	if (index == 0) {
		screenPairing.style.visibility = "visible"
	}
}