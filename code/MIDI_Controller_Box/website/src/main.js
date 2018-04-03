var screenMode = 0;
var song_list = [];
var song_count = 0;
var song_analysis;
var play_pause = 0; // 1 is play
var ctx;

// $(document).ready(function() {
//     init(); // wait for DOM to load
// });

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
    // REMOVE
    screenGoto(1);
    parseSongs(test_songs);

    ctx = audioVisualize.getContext("2d");
    setSong(0);
    changeGifSpeed(50);
}

function updateSonglist(song) {
    $('#trackList')[0].innerHTML += "<li>" + song + "</li>";
}
