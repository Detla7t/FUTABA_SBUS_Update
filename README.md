# FUTABA_SBUS_Update
SBUS driver for arduino Based on Futaba_sbus library but with some better creature comforts and useability improvements

This is Based off of FUTABA_SBUS this code is just test code it should work to read the data from a sbus rc receiver ive previously used it with
a FrSky R-XSR Ultra in sbus mode and a teensy 4.1 using the built in serial inverter that atleast on the teensy 4.1 can be used by opening the serial
port that has the inverter with PORTNAME.begin(100000,SERIAL_8E2_RXINV_TXINV) but i haven't tested it in awhile with real hardware the code is written
assuming you are using an inverter circuit and feeding that to the serial port if so you should have no issues also if the  but YOU WILL NEED to most
likely adjust the error_check_interval, and amount_of_allowable_errors depending on how much processing your project uses also you can disable the
error checking with the enable_error_checking variable PLEASE look at the example before using. feel free to use as you see fit all i ask is you credit
both me and the original creator https://github.com/mikeshub if you use this code.