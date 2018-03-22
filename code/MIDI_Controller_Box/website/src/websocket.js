// global WebSocket pointer
var webSocket;

// Used to package values to be sent down to C
function broadcast(key, ...values) {
    if (isNaN(key )) { return false; }
    webSocket.send(key + ":" + values.join(":"));
};


// decides what do when message arrives
function wsOnMessage(event) {
// console.log(event);
  // Message looks like => { "type" : 1, "value" : 0 }
  var message = JSON.parse(event.data);

  switch(parseInt(message.type)) {
  case 0:
    console.log("init");
    break;
  case 1: // song list
  
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

function wsAllReadyToStart() {
  broadcast(0, 0);
}

/////////////////////////////////////
// for testing to callback echo ws //
/////////////////////////////////////
function testInit() {    
    webSocket.send('{"type":1,"value":' + test_songs + '}');
}

