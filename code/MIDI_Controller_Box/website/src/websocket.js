// global WebSocket pointer
var webSocket;
const SEEK_RATE = 5;
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
      var divid = message.value.split("=");
      parseSongs(divid[0]);
      parseSamples(divid[1]);
      if (screenMode == 0) { screenGoto(1); } // ready to go
    break;
  case 1:
      // analysis buffers
    console.log(message);
    for (let i = 0; i < song_count; i++) {
      //song_list[i].analysis = new ArrayBuffer(150);
      for (let j=0; j<150; j++) {
        song_list[i].analysis[j] = message.value.charCodeAt((i*150) + j);
      }
    }

    break;
  case 2: // RED BUTTON
      if (menuMode) {
        hideMenu();
      } else {
          changePlayPause(message.value);
	  broadcast(2,0);
      }
    break;
  case 3: // GREEN BUTTON
    if (menuMode) {
      toggleMenu();
    } else {
      showMenu();
    }
    break;
  case 4: // BLUE BUTTON
    if (menuMode) {
      if (menuSelected) {
        broadcast(4,song_select);
      } else {
        broadcast(5,sample_select);
      }
    }
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
      } else {
	  broadcast(3, SEEK_RATE);
      }
    break;
  case 6: // VOLUME SLIDER
    changeVolume(message.value);
    break; 
  case 7: // GIF POT
    changeGifSpeed(message.value);
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

