# How To Update the DJ Box and Speaker

I designed this box with the knowledge that the songs/memes I put on were going to get out dated and wanted it to be easy for anyone to change it up. **please read carefully** as all the instructions below are really not hard to follow but can be easily messed up.

## Quick how it all works

So the DJ Controller contains no information about the songs or memes. **ALL** the data is stored on the Dragonboard in the LED Speaker. So plug it in and remotly access the dragonboard.

## Remoting into LED Speaker

The Dragonboard has been set to broadcast it's own WiFi. This means if you check, you should see a `SPEAKER DEBUG` WiFi connection. Connect to it and the password is `ieeerocks`

Once you are connected you can now both SSH and SCP into the board to transfer files.

- Username: `linaro`
- Password: `linaro`

## How to add a new song

First some restriction about the audio file since I was too lazy to hard error check everything :)

- Must be a 16 bit WAV file
  - Must end with a `.wav` extension
- Must be a **Mono** file, no stero, will sound bad if you try
- File name must be under 128 characters
- Contain only letters and numbers in name (this is to be safe, didn't do much error checking on this part at al)
- All underscores in name get replaced with spaces when displayed on DJ Controller
  - `Hello_World.wav` gets displayed as `Hello World`

All of these can be easily done with Audacity which is free and on the CAE computers

When you have your song add it to `/linaro/home/Bluetooth-Retro-DJ/code/LED_Speaker/audio/tracks/`

Also remove any songs from here as well

## How to add a Gif
