$(document).ready(function() {
    init(); // wait for DOM to load
});

function init() {
    setWebSocket();
    setup();
}

function setWebSocket() {
    // Attempts to just reload webpage if it was not able to get websocket
    // Will cause loop if not connect, but app is useless anyways without WS
    try {
	webSocket = new WebSocket('ws://' + location.host);
	webSocket.onmessage = wsOnMessage;
	webSocket.onopen = wsOnOpen;
    } catch (e) {
	   location.reload();
    }
}


function setup() {


}

function updateSonglist(song) {
    $('#trackList')[0].innerHTML += "<li>" + song + "</li>";
}
