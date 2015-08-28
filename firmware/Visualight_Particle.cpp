#include "Visualight_Particle.h"

Visualight_Particle::Visualight_Particle(){

  _blinkType = 0;
  _frequency = 0;
  alerting = false;
  alertBeginTimeStamp = 0;
  blinkState = 100;

  lastHeartbeat = 0;

}
void Visualight_Particle::setup(char* _URL, uint16_t _PORT, uint8_t _wiflyLedFlag){

    URL = _URL;
    PORT = _PORT;
	//colorLED(255,255,255);
	fadeOn();
	WiFi.macAddress(MAC);
	connectToServer();

}
boolean Visualight_Particle::connectToServer(){

  if (wifly.connect(URL, PORT)) {

    //VPRINTLN(F("Connected"));

    wifly.print("{\"mac\":\"");
    wifly.print(MAC);
    wifly.println("\"}");

    //reconnect = false;
    connectTime = millis();
    lastHeartbeat = millis();
    return true;
  }
  else {
    //VPRINTLN(F("Failed to open"));
    return false;
  }
}
void Visualight_Particle::update(){

	processClient();

    if(alerting && (millis() % 5)==0) alert();
}
// process all incoming server messages
void Visualight_Particle::processClient(){
  int available;

  if(millis()-lastHeartbeat > heartBeatInterval){
    sendHeartbeat();
  } else {
    available = wifly.available();

    // if (available < 0) {
    //   VPRINT(F("reconnect from available()"));
    //   reconnectCount++;
    //   if(!connectToServer()){
    //     //reconnectCount++;
    //   }
    // }
    // else
    if(available > 0){
      connectTime = millis();
      char thisChar;
      thisChar = wifly.read();
      if( thisChar == 97){
        int charCount = 0;
        thisChar = wifly.read();
        if(thisChar!='x'&&charCount<31){
        serBuf[charCount] = thisChar;
        charCount++;
        }else{
        //wifly.readBytesUntil('x', serBuf, 31);
        int duration;
        int red, green, blue, white;
        sscanf(serBuf,"%i,%i,%i,%i,%i,%i,%i",&red,&green,&blue,&white,&duration,&_frequency,&_blinkType); // INDIGO v0.1.1

         // VPRINT("buf: ");
         // VPRINTLN(serBuf);

        if(duration > 0 && _blinkType <=1){ //we are STARTING AN ALERT
          _durationTime = duration*1000;
          _frequency = (_frequency+1); //* 100; //get the right freq out //100 - 1000
          setAlert(_blinkType, _durationTime, _frequency, red, green, blue, white);
        }

        else if(_blinkType == 2){ // we are setting the start color here
					saveStartColor(red,green,blue,white);
        }

        else if(_blinkType == 3){ // reset WiFi, become a server
          //this is sent from server when a bulb is deleted from a Visualight account.
          //wifiReset(); //set isServer = true, turn on AP mode
        }

        else { //simple set color
          if(alerting){
  	        _durationTime = 0; // will time out any currently running alert
          }
            setColor(red, green, blue, white);
        }
        memset(serBuf,0,31);
        }
      }
    }
  }
}

// heartbeat to server so it knows this light is still connected
void Visualight_Particle::sendHeartbeat(){
  //VPRINTLN(F("-SENDHEARTBEAT-"));
  wifly.print("{\"mac\":\"");
  wifly.print(MAC);
  wifly.println("\",\"h\":\"h\"}");
  lastHeartbeat = millis();
  //VPRINT(F("Free memory: "));
  //int freeMem = wifly.getFreeMemory();
  //VPRINTLN(freeMem);
}
// turns all LEDs on to startColor
void Visualight_Particle::fadeOn(){
	_red = EEPROM.read(1);
	_green = EEPROM.read(2);
	_blue = EEPROM.read(3);
	_white = EEPROM.read(4);
  //VPRINTLN(F("fadeOn"));
  for(int fadeValue = 1; fadeValue <=100; fadeValue +=5) {
    colorLED((fadeValue*_red)/100, (fadeValue*_green)/100, (fadeValue*_blue)/100, (fadeValue*_white)/100);
    delay(10);
  }
}

// set the global color variables AND simple set to this color
void Visualight_Particle::setColor(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _w){
  _red = _r;
  _green = _g;
  _blue = _b;
  _white = _w;

  colorLED(_r,_g,_b,_w);
}
// if the new saveStartColor is different from what is already stored in EEPROM,
// overwrite the current EEPROM values and use this from now on.
void Visualight_Particle::saveStartColor(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _w){

  int currR = EEPROM.read(1);
  int currG = EEPROM.read(2);
  int currB = EEPROM.read(3);
  int currW = EEPROM.read(4);

  if (currR != _r){
    //VPRINTLN(F("SET R COLOR TO EEPROM"));
    EEPROM.write(1,_r);
  }

  if (currG != _g){
   // VPRINTLN(F("SET G COLOR TO EEPROM"));
    EEPROM.write(2,_g);
  }

  if (currB != _b){
    //VPRINTLN(F("SET B COLOR TO EEPROM"));
    EEPROM.write(3,_b);
  }

  if (currW != _w){
    //VPRINTLN(F("SET W COLOR TO EEPROM"));
    EEPROM.write(4,_w);
  }
}

void Visualight_Particle::setAlert(int blinkType, long durationTime, int frequency, int r, int g, int b, int w){
  _Ared = r;
  _Agreen = g;
  _Ablue = b;
  _Awhite = w;
  _blinkType = blinkType;
  _durationTime = durationTime;
  _frequency = frequency;
  alertBeginTimeStamp = millis();
  alerting = true;
  alert();
}

void Visualight_Particle::alert(){

  long elapsedBlinkTime = millis();

  if ( elapsedBlinkTime - alertBeginTimeStamp >= _durationTime){
//   	if(isServer){
//   		//wifly.setJoin(WIFLY_WLAN_JOIN_AUTO);
// 			//wifly.save();
// 			wifly.reboot();
// 	  	fadeOn();
// 	  	//VPRINTLN(F("SERVER TIMEOUT")); //TIMEOUT??
// 		}else{
//     	colorLED(_red, _green, _blue, _white);
//     }
    colorLED(_red, _green, _blue, _white);
    alerting = false;
    blinkState=100;
    //return;
  }

  else {  // update the current blink
    blinkState -= _frequency;
		if (blinkState <= 1 || blinkState >= 100) _frequency *= -1;

    switch(_blinkType){
      case 0: //FADING blink
        colorLED( int((blinkState*_Ared)/100), int((blinkState*_Agreen)/100), int((blinkState*_Ablue)/100), int((blinkState*_Awhite)/100));
				break;

      case 1: //HARD blink
        if(_frequency > 0 ){
          colorLED(_Ared, _Agreen, _Ablue, _Awhite);
        } else {
          colorLED(0, 0, 0, 0);
        }
				break;

      default:
      	break;
    }
  }
  delay(1);
}
// simple set all LEDs to this color
void Visualight_Particle::colorLED(int red, int green, int blue, int white){
  analogWrite(redLED, red);
  analogWrite(greenLED, green);
  analogWrite(blueLED, blue);
  analogWrite(whiteLED, white);
}
