#include <Arduino.h>
#include <FUTABA_SBUS.h>

/*
THIS CODE IS AN EXAMPLE OF USING NON BLOCKING CODE
basically this code reads the controller and if it reads your sbus receiver then 
should print channels 1-8 to the serial port also if you are not using an inverter 
circuit for the serial input this will most likely not work research how to invert
sbus before supplying the signal to your microcontroller or if your micro controller 
has a built in inverter circuit then change line 5 of the FUTABA_SBUS.cpp file that
simply is passing the whole interface to the function so if that serial port supports 
inverting changing the code slightly should allow you to use this library
*/

SBUS_Receiver sBus(Serial1);
int16_t* CH = sBus.UpdateChannels(); //allows you to easily parse the Channels by doing CH[Channel number] to get the value of any specific channel 
int16_t How_many_cycles_before_you_update_the_receiver_values = 3500; //adjust how fast it updates the values received NOTE IF YOU HAVE ISSUES WITH NO VALUES INCREASE THIS OTHERWISE IF YOU HAVE ISSUES WITH VALUES BEING STUCK TRY DECREASING THIS VALUE
int16_t How_many_cycles_before_checking_the_receiver_after_an_error = How_many_cycles_before_you_update_the_receiver_values * 10;
  
void setup(){
  sBus.error_check_interval = 100; //default
  sBus.amount_of_allowable_errors = 500; //default
  sBus.enable_error_checking = true; //this is on by default
  Serial.begin(9600);
}

//this simply increments a number by +1 unless the number supplied is above the given limit then it resets it to 0 and returns the result
int Generic_Wait(int number,int number_limit) {
    if (number >= number_limit) {
        number = 0;
    }
    number++;
    return number;
}

int Receiver_Cycle_count = 0;
int Error_Cycle_count = 0;
//this returns true and prints if the code has cycled "How_many_cycles_before_you_update_the_receiver_values" 
//other wise if channel 0 is equal to zero then it waits for "How_many_cycles_before_checking_the_receiver_after_an_error" 
//amount of cycles and prints an error retries to read the device and the returns false
bool RecieveCheck() {
    Receiver_Cycle_count = Generic_Wait(Receiver_Cycle_count,How_many_cycles_before_you_update_the_receiver_values);
    Error_Cycle_count = Generic_Wait(Error_Cycle_count,How_many_cycles_before_checking_the_receiver_after_an_error);
    if (CH[0] == 0) {
        if (Error_Cycle_count >= How_many_cycles_before_checking_the_receiver_after_an_error) {
            Serial.print("Error_0x500\n"); //Failed to get Signal Update Retrying
            sBus.ReadDevice();
        }
        return false;
    } else {
        if (Receiver_Cycle_count >= How_many_cycles_before_you_update_the_receiver_values) {
            sBus.ReadDevice();
            Serial.print(CH[1]);
            Serial.print("\t");
            Serial.print(CH[2]);
            Serial.print("\t");
            Serial.print(CH[3]);
            Serial.print("\t");
            Serial.print(CH[4]);
            Serial.print("\t");
            Serial.print(CH[5]);
            Serial.print("\t");
            Serial.print(CH[6]);
            Serial.print("\t");
            Serial.print(CH[7]);
            Serial.print("\t");
            Serial.println(CH[8]);
        }
        return true;
    }
}


void loop(){
  //this if statement should be true if your receiver is detected to be working correctly
  //and if you want to execute some code if its working normally or not your can do that
  //by adding logic to this if statement and this code is setup as non blocking so if you
  //need to do other things this wont stop execution of other parts of your code
  if(RecieveCheck()) { //if receiver is working normally
    //do something
  }
}

