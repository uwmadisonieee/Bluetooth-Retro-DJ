// Data purly for testing without the speaker connected

var test_songs = "I Love This Song;The Who;204;Sometimes I lie;Skrillox;176;Hey dude;The Bros;239;";

var test_analysis = new Uint8Array(1024);
for (i=0;i<125; i++) {
	test_analysis[i] = i*2;
}

for (i=125;i<150; i++) {
	test_analysis[i] = 255;
}
