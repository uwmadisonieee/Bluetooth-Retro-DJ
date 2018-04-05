// Data purly for testing without the speaker connected

var test_songs = "I Love This Song;The Who;204;Sometimes I lie;Skrillox;176;Hey dude;The Bros;239;ABC 123;That guy from Noth; 322;PLease Stpo;YoloBui; 123;I would rather not;YoungThung;112;Please Come Home;Me;203;Part Rocks;LMFAO;340;Sext andK Know It; LMFAO;302;No more ideas;everyone;155;A few more only;Poololo;294;BoBo Bo Bo Bo;OOB;251;";
var test_samples = "Bass Kick;Toms;High Hat;Clap;Snare;Random";

var test_analysis = new Uint8Array(1024);
for (i=0;i<125; i++) {
	test_analysis[i] = i*2;
}

for (i=125;i<150; i++) {
	test_analysis[i] = 255;
}
