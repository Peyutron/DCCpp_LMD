#!/usr/bin/env bash

esptool.py --chip esp8266 --port /dev/ttyUSB0 write_flash 0x00000 ~/NonOS/bin/boot_v1.7.bin 0x01000 ~/NonOS/bin/at/512+512/user1.1024.new.2.bin 0xfc000 ~/NonOS/bin/esp_init_data_default_v08.bin  0x7e000 ~/NonOS/bin/blank.bin 0xfe000

