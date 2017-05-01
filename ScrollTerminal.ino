#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define UART_SPEED 9600
#define BUFLEN 168
// OLED (11 -> MOSI/DIN, 13 ->SCK)
#define PIN_CS      8
#define PIN_RESET  10
#define PIN_DC      9
Adafruit_SSD1306 oled(PIN_DC, PIN_RESET, PIN_CS);

int countX = 0;
int countY = 0;

unsigned short swipp = 0;

void scroll() {
  unsigned short oldSwipp = swipp;
  for (int i=1; i<=10; ++i) {
    oled.fillRect(0, oldSwipp, oled.width(), i, BLACK);
    oled.ssd1306_command(SSD1306_SETDISPLAYOFFSET);
    oled.ssd1306_command(swipp);
    swipp++;
    oled.display();
    delay(50);
  }
}

char umlReplace(char inChar) {
  if (inChar == -97) {
    inChar = 224; // ß
  } else if (inChar == -80) {
    inChar = 248; // °
  } else if (inChar == -67) {
    inChar = 171; // 1/2
  } else if (inChar == -78) {
    inChar = 253; // ²
  } else if (inChar == -92) {
    inChar = 132; // ä
  } else if (inChar == -74) {
    inChar = 148; // ö
  } else if (inChar == -68) {
    inChar = 129; // ü
  } else if (inChar == -124) {
    inChar = 142; // Ä
  } else if (inChar == -106) {
    inChar = 153; // Ö
  } else if (inChar == -100) {
    inChar = 154; // Ü
  } else if (inChar == -85) {
    inChar = 0xAE; // <<
  } else if (inChar == -69) {
    inChar = 0xAF; // >>
  }
  return inChar;  
}

void setup() {
  oled.begin(SSD1306_SWITCHCAPVCC);
  oled.setTextSize(0);
  oled.setTextColor(WHITE);
  Serial.begin(UART_SPEED);
  oled.clearDisplay();
  oled.display();
}

void loop() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == -61) continue; // symbol before utf-8
    if (inChar == -62) continue; // other symbol before utf-8
    if(inChar == '\n') {
      countY++;
      countX=0;
    } else {
      countX++;
      if(countX == 22) {
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
      oled.ssd1306_command(SSD1306_SETDISPLAYOFFSET);
      oled.ssd1306_command(swipp);
    }
            
    oled.setCursor((countX-1)*6,countY*10);
    oled.print(inChar);
    oled.display();
  }
}
