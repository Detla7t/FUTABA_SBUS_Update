#include "FUTABA_SBUS.h"

void FUTABA_SBUS::begin(HardwareSerial& bus){
	port = &bus; //gets the current address of the serial port you want to use
  	port->begin(BAUDRATE); //starts the listener on the port you provieded
	memcpy(sbusData,loc_sbusData,25);
	memcpy(channels,loc_channels,18);
	memcpy(servos,loc_servos,18);
	failsafe_status = SBUS_SIGNAL_OK;
	sbus_passthrough = 1;
	toChannels = 0;
	bufferIndex=0;
	feedState = 0;
}

int16_t FUTABA_SBUS::Channel(uint8_t ch) {
  // Read channel data and if its out of range return 0
  if ((ch>0)&&(ch<=16)){
  	return channels[ch-1];
  }
  else{
  	return 0;
  }
}

uint8_t FUTABA_SBUS::DigiChannel(uint8_t ch) {
  // Read channel data and if its out of range return 0
  if ((ch>0) && (ch<=2)) {
    return channels[15+ch];
  }
  else{
    return 0;
  }
}
void FUTABA_SBUS::Servo(uint8_t ch, int16_t position) {
  // Set servo position
  if ((ch>0)&&(ch<=16)) {
    if (position>2048) {
      position=2048;
    }
    servos[ch-1] = position;
  }
}
void FUTABA_SBUS::DigiServo(uint8_t ch, uint8_t position) {
  // Set digital servo position
  if ((ch>0) && (ch<=2)) {
    if (position>1) {
      position=1;
    }
    servos[15+ch] = position;
  }
}
uint8_t FUTABA_SBUS::Failsafe(void) {
  return failsafe_status;
}

void FUTABA_SBUS::PassthroughSet(int mode) {
  // Set passtrough mode, if true, received channel data is send to servos
  sbus_passthrough = mode;
}

int FUTABA_SBUS::PassthroughRet(void) {
  // Return current passthrough mode
  return sbus_passthrough;
}
void FUTABA_SBUS::UpdateServos(void) {
  // Send data to servos
  // Passtrough mode = false >> send own servo data
  // Passtrough mode = true >> send received channel data
  uint8_t i;
  if (sbus_passthrough==0) {
    // clear received channel data
    for (i=1; i<24; i++) {
      sbusData[i] = 0;
    }

    // reset counters
    ch = 0;
    bit_in_servo = 0;
    byte_in_sbus = 1;
    bit_in_sbus = 0;

    // store servo data
    for (i=0; i<176; i++) {
      if (servos[ch] & (1<<bit_in_servo)) {
        sbusData[byte_in_sbus] |= (1<<bit_in_sbus);
      }
      bit_in_sbus++;
      bit_in_servo++;

      if (bit_in_sbus == 8) {
        bit_in_sbus =0;
        byte_in_sbus++;
      }
      if (bit_in_servo == 11) {
        bit_in_servo =0;
        ch++;
      }
    }

    // DigiChannel 1
    if (channels[16] == 1) {
      sbusData[23] |= (1<<0);
    }
    // DigiChannel 2
    if (channels[17] == 1) {
      sbusData[23] |= (1<<1);
    }

    // Failsafe
    if (failsafe_status == SBUS_SIGNAL_LOST) {
      sbusData[23] |= (1<<2);
    }

    if (failsafe_status == SBUS_SIGNAL_FAILSAFE) {
      sbusData[23] |= (1<<2);
      sbusData[23] |= (1<<3);
    }
  }
  // send data out
  //serialPort.write(sbusData,25);
  for (i=0;i<25;i++) {
    port.write(sbusData[i]);
  }
}

void FUTABA_SBUS::UpdateChannels(void) {
	//uint8_t i;
	//uint8_t sbus_pointer = 0;
	// clear channels[]
	/*for (i=0; i<16; i++) {
	channels[i] = 0;
	}

	// reset counters
	byte_in_sbus = 1;
	bit_in_sbus = 0;
	ch = 0;
	bit_in_channel = 0;
	//this method is much slower than the other method
	// process actual sbus data
	for (i=0; i<176; i++) {
	if (sbusData[byte_in_sbus] & (1<<bit_in_sbus)) {
	channels[ch] |= (1<<bit_in_channel);
	}
	bit_in_sbus++;
	bit_in_channel++;

	if (bit_in_sbus == 8) {
	bit_in_sbus =0;
	byte_in_sbus++;
	}
	if (bit_in_channel == 11) {
	bit_in_channel =0;
	ch++;
	}
	}*/
	newdata_check = sbusData[20];
	if (Channel_Error_cycle <= error_check_interval && enable_error_checking) {
		newdata_check = old_data;
	}
	channels[0]  = ((sbusData[1]|sbusData[2]<< 8) & 0x07FF);
	channels[1]  = ((sbusData[2]>>3|sbusData[3]<<5) & 0x07FF);
	channels[2]  = ((sbusData[3]>>6|sbusData[4]<<2|sbusData[5]<<10) & 0x07FF);
	channels[3]  = ((sbusData[5]>>1|sbusData[6]<<7) & 0x07FF);
	channels[4]  = ((sbusData[6]>>4|sbusData[7]<<4) & 0x07FF);
	channels[5]  = ((sbusData[7]>>7|sbusData[8]<<1|sbusData[9]<<9) & 0x07FF);
	channels[6]  = ((sbusData[9]>>2|sbusData[10]<<6) & 0x07FF);
	channels[7]  = ((sbusData[10]>>5|sbusData[11]<<3) & 0x07FF); // & the other 8 + 2 channels if you need them
	channels[8]  = ((sbusData[12]|sbusData[13]<< 8) & 0x07FF);
	channels[9]  = ((sbusData[13]>>3|sbusData[14]<<5) & 0x07FF);
	channels[10] = ((sbusData[14]>>6|sbusData[15]<<2|sbusData[16]<<10) & 0x07FF);
	channels[11] = ((sbusData[16]>>1|sbusData[17]<<7) & 0x07FF);
	channels[12] = ((sbusData[17]>>4|sbusData[18]<<4) & 0x07FF);
		channels[13] = ((sbusData[18]>>7|sbusData[19]<<1|newdata_check<<9) & 0x07FF);
	channels[14] = ((newdata_check>>2|sbusData[21]<<6) & 0x07FF);
	channels[15] = ((sbusData[21]>>5|sbusData[22]<<3) & 0x07FF);
		//technically this is channel 13 but with an added studder to check for if the data is new or not
		channels[16] = ((sbusData[18]>>7|sbusData[19]<<1|sbusData[20]<<9) & 0x07FF);
	// DigiChannel 1
	/*if (sbusData[23] & (1<<0)) {
	channels[16] = 1;
	}
	else{
	channels[16] = 0;
	}
	// DigiChannel 2
	if (sbusData[23] & (1<<1)) {
	channels[17] = 1;
	}
	else{
	channels[17] = 0;
	}*/
	// Failsafe
	failsafe_status = SBUS_SIGNAL_OK;
	if (sbusData[23] & (1<<2)) {
		failsafe_status = SBUS_SIGNAL_LOST;
	}
	if (sbusData[23] & (1<<3)) {
		failsafe_status = SBUS_SIGNAL_FAILSAFE;
	}
	//checks if channel 16 is out of range and if so sets all channels to zero
	if (!enable_error_checking) {
		return channels;
	}
	//if channel 16 is operating normally reset a
	if (channels[16] >= 950) {
	    Channel_update_fail_count = 0;
	}
	//checks if channel 16 which is are error check channel is within normal range and if not and the update fail count goes above the amount_of_allowable_errors then it sets all channels to 0
	if (channels[16] <= 950 && Channel_update_fail_count >= amount_of_allowable_errors) {
	    Channel_update_fail_count = amount_of_allowable_errors;
	    Signal_Error();
	    Channel_update_fail_count++;
	} 
	//This copies the data in sbusData[20] copies it to old data corrupts sbusData by setting it to hex 0 and resets the counter
	if(Channel_Error_cycle >= error_check_interval) {
		old_data = sbusData[20];
		sbusData[20] = 0x00;
		Channel_Error_cycle = 0;
	} 
	Channel_Error_cycle++;
	return channels;
}

void FUTABA_SBUS::Signal_Error() {
	for (int16_t i = 0; i <= 15; i++) {
		channels[i] = 0;
	}
}

void FUTABA_SBUS::FeedLine(void){
  if (port.available() > 24){
    while(port.available() > 0){
      inData = port.read();
      switch (feedState){
      case 0:
        if (inData != 0x0f){
          while(port.available() > 0){//read the contents of in buffer this should resync the transmission
            inData = port.read();
          }
          return;
        }
        else{
          bufferIndex = 0;
          inBuffer[bufferIndex] = inData;
          inBuffer[24] = 0xff;
          feedState = 1;
        }
        break;
      case 1:
        bufferIndex ++;
        inBuffer[bufferIndex] = inData;
        if (bufferIndex < 24 && port.available() == 0){
          feedState = 0;
        }
        if (bufferIndex == 24){
          feedState = 0;
          if (inBuffer[0]==0x0f && inBuffer[24] == 0x00){
            memcpy(sbusData,inBuffer,25);
            toChannels = 1;
          }
        }
        break;
      }
    }
  }
}

