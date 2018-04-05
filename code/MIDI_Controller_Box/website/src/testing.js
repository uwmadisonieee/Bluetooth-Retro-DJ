// Data purly for testing without the speaker connected

var test_songs = "Spectrum  Zedd;228609000;Bass Down Low  Dev;197765000;Alone  Marshmello;187688000;Kyoto  Skrillex;193135000;Take Over Control  Afrojack;204090000;Animals  Martin Garrix;175569000;Harder Better Faster Stronger  Daft Punk;206380000;Go Deep  Neoteric and Wax Motif;271590000;Boneless  Steve Aoki;216794000;Jump Around  House of Pain;75031000;Lose Control  Missy Elliott;167744000;Bangarang Skrillex;196812000;Where Are U Now  Jack U;226671000;Sandstorm  Darude;210420000;Summer  Calvin Harris;196742000;Levels  Avicii;194560000;This Is What You Came For  Calvin Harris;207258000;Lean On  Major Lazer;159337000;Party Rock Anthem  LFMAO;219223000;Wake Me Up  Avicii;208524000;=Hi Hats;Claps;Bass Kick;Snares;Toms;";
var test_analysis = new Uint8Array(1024);
for (i=0;i<125; i++) {
	test_analysis[i] = i*2;
}

for (i=125;i<150; i++) {
	test_analysis[i] = 255;
}
