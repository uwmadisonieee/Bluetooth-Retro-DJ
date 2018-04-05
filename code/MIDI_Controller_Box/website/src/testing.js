// Data purly for testing without the speaker connected

var test_songs = "Spectrum  Zedd;228609000;Bass Down Low  Dev;197765000;Alone  Marshmello;187688000;Kyoto  Skrillex;193135000;Take Over Control  Afrojack;204090000;Animals  Martin Garrix;175569000;Harder Better Faster Stronger  Daft Punk;206380000;Go Deep  Neoteric and Wax Motif;271590000;Boneless  Steve Aoki;216794000;Jump Around  House of Pain;75031000;Lose Control  Missy Elliott;167744000;Bangarang Skrillex;196812000;Where Are U Now  Jack U;226671000;Sandstorm  Darude;210420000;Summer  Calvin Harris;196742000;Levels  Avicii;194560000;This Is What You Came For  Calvin Harris;207258000;Lean On  Major Lazer;159337000;Party Rock Anthem  LFMAO;219223000;Wake Me Up  Avicii;208524000;=Hi Hats;Claps;Bass Kick;Snares;Toms;";
var test_analysis = new Uint8Array(1024);
for (i=0;i<125; i++) {
	test_analysis[i] = i*2;
}

for (i=125;i<150; i++) {
	test_analysis[i] = 255;
}

var test_a = "77,58,108,53,97,69,108,115,105,116,102,104,104,98,100,101,103,106,101,112,108,122,112,110,122,111,118,114,97,110,112,116,112,110,119,118,113,111,83,119,121,118,129,118,132,123,122,127,118,124,120,116,119,124,127,118,120,105,107,106,104,108,115,112,100,109,108,105,113,121,118,124,123,123,148,130,123,94,9,55,118,117,111,113,113,122,118,114,114,121,121,116,116,112,113,117,119,112,120,108,87,96,95,84,113,119,111,116,105,105,113,94,117,108,135,110,119,122,113,125,120,121,118,124,122,123,136,157,130,133,142,132,124,136,131,129,131,130,133,121,126,119,117,127,133,132,121,124,119,107,";