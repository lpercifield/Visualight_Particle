#include "application.h"
#include "visualight_particle/Visualight.h"
Visualight visualight;



void setup()
{
  // start listening for clients

  // Make sure your Serial Terminal app is closed before powering your device
  Serial.begin(115200);
  // Now open your Serial Terminal, and hit any key to continue!
  while(!Serial.available()) Particle.process();

  Serial.println(WiFi.localIP());
  Serial.println(WiFi.subnetMask());
  Serial.println(WiFi.gatewayIP());
  Serial.println(WiFi.SSID());
}

void loop()
{
  visualight.setup("visualight.cloudapp.net", 5001, 1);
}
