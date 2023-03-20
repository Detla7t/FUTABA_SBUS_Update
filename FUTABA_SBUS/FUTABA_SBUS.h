#include <Arduino.h>

#ifndef SBus_Receiver
#define SBus_Receiver


#define SBUS_SIGNAL_OK          0x00
#define SBUS_SIGNAL_LOST        0x01
#define SBUS_SIGNAL_FAILSAFE    0x03
#define BAUDRATE 100000

class SBUS_Receiver
{
	public:
		//in my testing this works perfect for a teensy 4.1 running at 25mhz
		//if you are running at 600mhz according to my notes it could only
		//do an update to the channels around 3500 program cycles so basically
		//increasing the number changes how often it checks for a loss of connection
		//to the sbus receiver so this is likely need to be tuned base on how fast or
		//how slow your program operates
		uint16_t error_check_interval = 100; 
		uint16_t amount_of_allowable_errors = 500; //this adjusts how many cycles it trys to get an update for before setting all channels to zero
		bool enable_error_checking = true;
		uint8_t sbusData[25];
		int16_t channels[18];
		int16_t servos[18];
		uint8_t  failsafe_status;
		int sbus_passthrough;
		int toChannels;
		void begin(void);
		int16_t Channel(uint8_t ch);
		uint8_t DigiChannel(uint8_t ch);
		void Servo(uint8_t ch, int16_t position);
		void DigiServo(uint8_t ch, uint8_t position);
		uint8_t Failsafe(void);
		void PassthroughSet(int mode);
		int PassthroughRet(void);
		void UpdateServos(void);
		void UpdateChannels(void);
		void FeedLine(void);
		void Signal_Error();
		void ReadDevice(void); //this simplifys the usage by only needing to call this one function to actually read the sbus device
	private:
		HardwareSerial* port;
		//This variable is used to ensure the data is actually updated
		//regularly by inducing errors in this number and seeing if they
		//are corrected if not it sets all channels to zero
		uint16_t Channel_update_fail_count; 
		uint16_t Channel_Error_cycle;
		char newdata_check;
		char old_data;
		const uint8_t loc_sbusData[25] = {0x0f,0x01,0x04,0x20,0x00,0xff,0x07,0x40,0x00,0x02,0x10,0x80,0x2c,0x64,0x21,0x0b,0x59,0x08,0x40,0x00,0x02,0x10,0x80,0x00,0x00};
		const int16_t loc_channels[18]  = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		const int16_t loc_servos[18]    = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		uint8_t byte_in_sbus;
		uint8_t bit_in_sbus;
		uint8_t ch;
		uint8_t bit_in_channel;
		uint8_t bit_in_servo;
		uint8_t inBuffer[25];
		int bufferIndex;
		uint8_t inData;
		int feedState;

};

#endif
