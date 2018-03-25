// global WebSocket pointer
var webSocket;

// Used to package values to be sent down to C
function broadcast(key, ...values) {
    if (isNaN(key )) { return false; }
    webSocket.send(key + ":" + values.join(":"));
};

function wsOnOpen() {
    broadcast(0,0); // tells server we are ready to connect to bluetooth
}

var songList = [];
// decides what do when message arrives
function wsOnMessage(event) {
// console.log(event);
  // Message looks like => { "type" : 1, "value" : 0 }
  var message = JSON.parse(event.data);

  switch(parseInt(message.type)) {
  case 0: // add to song list
    songList.push(message.value);
      if (screenMode == 0) { screenGoto(1); }
      updateSonglist(message.value);
    break;
  case 1:
      console.log(message.value);
    break;
  case 2:
    break;
  case 3:
    break;
  case 4:
    break;
  case 5:
      break;
  default:
	  warn("WebSocket", "No case for data: %0", message);
  }
}

/////////////////////////////////////
// for testing to callback echo ws //
/////////////////////////////////////
function testInit() {    
    webSocket.send('{"type":1,"value":' + test_songs + '}');
}

