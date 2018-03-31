#!/bin/bash

# Audio setup
amixer -c 0 cset iface=MIXER,name='RX3 MIX1 INP1' 'RX1'
amixer -c 0 cset iface=MIXER,name='SPK DAC Switch' 1
amixer -c 0 cset iface=MIXER,name='RX3 Digital Volume' 115


# Bluetooth - Turn discoverable on
echo -e 'discoverable on\nquit' | bluetoothctl

# Run the show
/home/linaro/Bluetooth-Retro-DJ/code/LED_Speaker/LED_Speaker



