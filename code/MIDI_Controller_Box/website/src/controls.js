function changeVolume(vol) {
	volumeText.innerText = "Volume: " + vol + "%";
	volumeSlider.style.height = (vol*1.8) + "px"; // 180px height max
	
	let temp = Math.floor(vol * 2.555).toString(16);
	if (temp.length == 1) {
		temp = "0" + temp; // padding
	}
	volumeSlider.style.boxShadow = "0 0 2px 20px #fff, 0 0 50px 50px #00"+temp+"ff";
}

function changeGifSpeed(speed) {
	gifSpeed.innerText = "GIF SPEED: " + speed + "%";
	gifWheel.style.strokeDashoffset = (250 - Math.round(speed * 1.25)).toString();
	gifWheel.style.stroke = "rgb(255, 0," + (250 - speed*2) + ")";
}

function parseSongs(songs) {
    var song_token = songs.split(";");
    song_count = (song_token.length-1) / 2;
    for (let i = 0; i < song_count; i++) {
        song_list.push({
        	"name" : song_token[2*i].substring(0,song_token[2*i].indexOf("  ")),
        	"artist" : song_token[2*i].substr(song_token[2*i].indexOf("  ") + 2),
        	"seconds" : song_token[2*i+1],
        	"analysis" : new ArrayBuffer(150)
        });
    }

    setSelectSong(0);
}

function setSong(index) {

    song_analysis = test_analysis;
	drawVisualizer();

	audioName.innerText = song_list[index].name + " - " + song_list[index].artist;
	audioLength.innerText = Math.floor(song_list[index].seconds/60) + ":" + (song_list[index].seconds%60);
}

function parseSamples(samples) {
	sample_list = sample.split(";").slice(0,sample.split(";").length - 1);
    sample_count = sample_list.length;
    setSelectSample(0);
}

const VIS_HEIGHT_F = 0.25;
function drawVisualizer() {
	let temp;
	ctx.lineWidth=1;
	for (let i = 0; i < 150; i++) {

		ctx.beginPath();
		ctx.fillStyle = "rgb(255, 10," + (255 - song_analysis[i]) + ")";

		temp = (song_analysis[i] * VIS_HEIGHT_F);
		ctx.fillRect(i*2, 75 - temp, 1, 2*temp);
		ctx.stroke();

	}
}

function drawPlayed(index) {
	ctx.lineWidth=1;

	ctx.beginPath();
	ctx.fillStyle = "#00FFFF";
	temp = (song_analysis[index] * VIS_HEIGHT_F);
	ctx.fillRect(index*2, 75 - temp, 1, 2*temp);
	ctx.stroke();
}

function changePlayPause(status){ 
	play_pause = status;
	spinningRecord.style.animationPlayState = (play_pause) ? "running" : "paused";
	startStopDiv.style.color = (play_pause) ? "#19ea00" : "#ff1f1f";
	startStopDiv.innerHTML = (play_pause) ? "&#9658;" : "&#9612;&#9612;";
}

const selectItemCount = 5;
function setSelectSong(index) {

	let offset = Math.floor(index / selectItemCount) * selectItemCount;

	for (let i = 0; i < selectItemCount; i++) {
		if (offset + i < song_count) {
			selectSong[i].innerHTML =  "<b>" + song_list[offset + i].name + "</b> - " + song_list[offset + i].artist;
			if (index == offset + i) {
				selectSong[i].style.backgroundColor = menuColorSelect;
				selectSong[i].style.color = "white";			
			} else {
				selectSong[i].style.backgroundColor = menuColorNo;
				selectSong[i].style.color = "black";
			}
		} else {
			selectSong[i].innerHTML = "";
		}
	}

	if (index < selectItemCount) {
		selectSongUp.style.visibility = "hidden";
		selectSongDown.style.visibility = "visible";
	} else if (offset + 5 >= song_count) {		
		selectSongUp.style.visibility = "visible";
		selectSongDown.style.visibility = "hidden";
	} else {
		selectSongUp.style.visibility = "visible";
		selectSongDown.style.visibility = "visible";
	}
}

function setSelectSample(index) {

	let offset = Math.floor(index / selectItemCount) * selectItemCount;

	for (let i = 0; i < selectItemCount; i++) {
		if (offset + i < sample_count) {
			selectSample[i].innerHTML =  "<b>" + sample_list[offset + i] + "</b>";
			if (index == offset + i) {
				selectSample[i].style.backgroundColor = menuColorSelect;
				selectSample[i].style.color = "white";			
			} else {
				selectSample[i].style.backgroundColor = menuColorNo;
				selectSample[i].style.color = "black";
			}
		} else {
			selectSample[i].innerHTML = "";
		}
	}

	if (index < selectItemCount) {
		selectSampleUp.style.visibility = "hidden";
		selectSampleDown.style.visibility = "visible";
	} else if (offset + 5 >= sample_count) {		
		selectSampleUp.style.visibility = "visible";
		selectSampleDown.style.visibility = "hidden";
	} else {
		selectSampleUp.style.visibility = "visible";
		selectSampleDown.style.visibility = "visible";
	}
}