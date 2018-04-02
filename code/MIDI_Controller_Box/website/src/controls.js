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
	//gifSpeed.innerText = 
}

function parseSongs(songs) {
    var song_token = songs.split(";");
    song_count = (song_token.length-1) / 3;
    for (let i = 0; i < song_count; i++) {
        song_list.push({
        	"name" : song_token[3*i],
        	"artist" : song_token[3*i+1],
        	"seconds" : song_token[3*i+2]
        });
    }
}

function setSong(index) {

    song_analysis = test_analysis;
	drawVisualizer()
}

const VIS_HEIGHT_F = 0.25;
function drawVisualizer() {
	let temp;
	ctx.lineWidth=1;
	for (let i = 0; i < 150; i++) {

		ctx.beginPath();
		ctx.fillStyle = "rgb(255, 0," + (255 - song_analysis[i]) + ")";

		temp = (song_analysis[i] * VIS_HEIGHT_F);
		ctx.fillRect(i*2, 75 - temp, 1, 2*temp);
		ctx.stroke();

	}
}

function drawPlayed(index) {
	ctx.lineWidth=1;

	ctx.beginPath();
	ctx.fillStyle = "#0000FF";
	temp = (song_analysis[index] * VIS_HEIGHT_F);
	ctx.fillRect(index*2, 75 - temp, 1, 2*temp);
	ctx.stroke();
	
}