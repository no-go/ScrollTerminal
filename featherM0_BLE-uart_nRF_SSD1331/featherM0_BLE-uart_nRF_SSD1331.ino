#include <Arduino.h>
#include <Adafruit_GFX.h>
#include "Adafruit_SSD1331.h"

#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "BluefruitConfig.h"

// 6x16 zeichen:
#define BUFLEN 96

#define OLED_DC      5
#define OLED_CS     12
#define OLED_RESET   6

int countX = 0;
int countY = 0;

// Color definitions
#define BLACK           0x0000
#define GREYBLUE        0b0010000100010000
#define LIGHTBLUE       0b0111000111011111
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0  
#define AMBER           0b1111101111100111
#define WHITE           0xFFFF


unsigned short swipp = 0;
Adafruit_SSD1331  oled = Adafruit_SSD1331(OLED_CS, OLED_DC, OLED_RESET);
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

void scroll() {
  unsigned short oldSwipp = swipp;
  for (int i=1; i<=10; ++i) {
    oled.fillRect(0, oldSwipp, oled.width(), i, BLACK);
    oled.writeCommand(SSD1331_CMD_DISPLAYOFFSET);
    oled.writeCommand(swipp);
    swipp++;
    delay(50);
  }
}

char umlReplace(char inChar) {
  if (inChar == 159) {
    inChar = 224; // ß
  } else if (inChar == 164) {
    inChar = 132; // ä
  } else if (inChar == 182) {
    inChar = 148; // ö
  } else if (inChar == 188) {
    inChar = 129; // ü
  } else if (inChar == 132) {
    inChar = 142; // Ä
  } else if (inChar == 150) {
    inChar = 153; // Ö
  } else if (inChar == 156) {
    inChar = 154; // Ü
  } else if (inChar == 171) {
    inChar = 0xAE; // <<
  } else if (inChar == 187) {
    inChar = 0xAF; // >>
  }  
  return inChar;  
}

void setup() {
  oled.begin();
  oled.fillScreen(BLACK);
  oled.setTextSize(0);

  ble.begin(false);
  ble.echo(false);
  ble.sendCommandCheckOK("AT+HWModeLED=BLEUART");
  ble.sendCommandCheckOK("AT+GAPDEVNAME=oLED Feather");
  ble.sendCommandCheckOK("ATE=0");
  ble.sendCommandCheckOK("AT+BAUDRATE=115200");
  ble.sendCommandCheckOK("AT+BLEPOWERLEVEL=4");
  ble.sendCommandCheckOK("ATZ");
  ble.setMode(BLUEFRUIT_MODE_DATA);
  ble.verbose(false);

}

void loop() {
  if (ble.isConnected()) {
    while ( ble.available() ) {
      char inChar = (char) ble.read();
      if (inChar == 194) continue; // symbol before utf-8
      if (inChar == 195) continue; // symbol before utf-8
      if(inChar == '\n') {
        countY++;
        countX=0;
      } else {
        countX++;
        if(countX == 17) {
          countY++;
          countX=1;
        }      
      }
      inChar = umlReplace(inChar);
  
      if (swipp == 0) {
        if(countY>5) {
          countX=1;
          if(inChar == '\n') countX=0;
          countY=0;
          scroll();
        }    
      } else {
        for (int j=0;j<5;++j) {
          if (swipp == 10*(j+1)) {
            if(countY>j) {
              countX=1;
              if(inChar == '\n') countX=0;
              countY=j+1;
              scroll();
            } 
          }    
        }
      }
      if (swipp == 60) {
        swipp=0;
        oled.writeCommand(SSD1331_CMD_DISPLAYOFFSET);
        oled.writeCommand(swipp);
      }
              
      oled.setCursor((countX-1)*6,countY*10);
      oled.print(inChar);
    }
  }
}

