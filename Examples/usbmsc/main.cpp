/**
 * Simple MSC device with SD card
 * author: chegewara
 */
#include <Arduino.h>
#include "sdusb.h"
// #if CFG_TUD_MSC

// #define SD_MISO  37
// #define SD_MOSI  39
// #define SD_SCK   38
// #define SD_CS    40

SDCard2USB dev;

void setup()
{
  Serial.begin(115200);

  if(dev.initSD(SCK, MISO, MOSI, SS))
  {
    if(dev.begin()) {
      Serial.println("MSC lun 1 begin");
    } else log_e("LUN 1 failed");
  } else Serial.println("Failed to init SD");

  // test();
}

void loop()
{
  delay(1000);
}

// #endif