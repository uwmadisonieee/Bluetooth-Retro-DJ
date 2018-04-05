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
    break;
  case 2: // RED BUTTON
    break;
  case 3: // GREEN BUTTON
    break;
  case 4: // BLUE BUTTON
    break;
  case 5: // ROTARY ENCODER
      if (menuMode) {
        if (menuSelected) {
          song_select += message.value;          
          if (song_select < 0) {song_select = 0;}
          else if (song_select >= song_count) {song_select = song_count-1;}
          setSelectSong(song_select);
        } else {
          samnple_select += message.value;          
          if (samnple_select < 0) {samnple_select = 0;}
          else if (samnple_select >= sample_count) {samnple_select = sample_count-1;}
          setSelectSong(samnple_select);
        }
      }
    break;
  case 6: // VOLUME SLIDER
    break; 
  case 7: // GIF POT
    break;
  case 8: //
    break;
  case 9:
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

