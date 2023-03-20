#include <Arduino.h>
#include <FUTABA_SBUS.h>
#include <Streaming.h> //not sure if this is needed dont have an easy way to check i would recommend trying to remove this line and see if you have issues or not


FUTABA_SBUS sBus(Serial1);
sBus.error_check_interval = 100; //default
sBus.amount_of_allowable_errors = 500; //default
sBus.enable_error_checking = true; //this is on by default
int16_t* CH = sBus.UpdateChannels(); //allows you to easily parse the Channels by doing CH[Channel number] to get the value of any specific channel 
int16_t How_many_cycles_before_you_update_the_receiver_values = 3500; //adjust how fast it updates the values received NOTE IF YOU HAVE ISSUES WITH NO VALUES INCREASE THIS OTHERWISE IF YOU HAVE ISSUES WITH VALUES BEING STUCK TRY DECREASING THIS VALUE
int16_t How_many_cycles_before_checking_the_receiver_after_an_error = How_many_cycles_before_you_update_the_receiver_values * 10; //
  
void setup(){
  sBus.begin();
  Serial.begin(9600);
}

void loop(){
  if(RecieveCheck()) { //if receiver is working normally
    //do something
  }
}

Generic_Wait(int number,int number_limit) {
    if (number >= number_limit) {
        number = 0;
    }
    number++;
    return number;
}

int Receiver_Cycle_count = 0;
int Error_Cycle_count = 0;

bool RecieveCheck() {
    Receiver_Cycle_count = Generic_Wait(Receiver_Cycle_count,How_many_cycles_before_you_update_the_receiver_values);
    Error_Cycle_count = Generic_Wait(Error_Cycle_count,How_many_cycles_before_checking_the_receiver_after_an_error);
    if (CH[0] == 0) {
        if (Error_Cycle_count >= 35000) {
            Serial.print("Error_0x500\n"); //Failed to get Signal Update Retrying
            sBus.ReadDevice();
        }
        return false;
    } else {
        if (Receiver_Cycle_count >= 3500) {
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
