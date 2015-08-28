#ifndef Visualight_Particle_h
#define Visualight_Particle_h

#include "application.h"
#include "spark_wiring_string.h"
#include "spark_wiring_tcpclient.h"
//#include "spark_wiring_usbserial.h"

// telnet defaults to port 23
//TCPServer server = TCPServer(5001);
#define heartBeatInterval 25000
#define blueComp 1.2
#define redLED 1
#define greenLED 2
#define blueLED 0
/* whiteLED is digitalPin 5 on the PRODUCT Version.
if you are converting a DIY bulb to RGBW, change this to digitalPin 6 */
 #define whiteLED 3 // DIY = 6 // PRODUCT = 5 //
class Visualight_Particle {

	public:
		Visualight_Particle(void);
		TCPClient wifly; //just in case we want to access this from the sketch...
		void update();
		void setup(char* _URL, uint16_t _PORT, uint8_t _wiflyLedFlag);
		void saveStartColor(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _w);
		void setColor(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _w);

	private:
	    void sendMac();
	    void processClient();
		void colorLED(int red, int green, int blue, int white);
		void fadeOn();
		void sendHeartbeat();
		void alert();
		void setAlert(int blinkType, long durationTime, int frequency, int r, int g, int b, int w);
		boolean alerting;
		long alertBeginTimeStamp;
		uint16_t blinkState; //counts from 0 - 100
		boolean sentHeartBeat;
		char * URL;
		uint16_t PORT;
		char serBuf[31];//21 for blink
		char MAC[18];

		int _red;
		int _green;
		int _blue;
		int _white;

		int _Ared;
		int _Agreen;
		int _Ablue;
		int _Awhite;

		int _frequency;
		int _blinkType;
		long _durationTime;

		boolean _debug;
		uint8_t wiflyLedFlag;

		volatile uint8_t resetButtonState;
		int resetTime;
		uint8_t reconnectCount;

		uint32_t connectTime;
		uint32_t lastHeartbeat;
};

#endif
