// Data purly for testing without the speaker connected

var test_songs = "I love this song;The Who;204;Sometimes I lie;Skrillox;176;Hey dude;The Bros;239;";
var test_analysis = new Uint8Array(1024);
for (i=0;i<256; i++) {
	test_analysis[i] = i;
}

for (i=256;i<767; i++) {
	test_analysis[i] = 255;
}

for (i=767;i<1024; i++) {
	test_analysis[i] = i;
}