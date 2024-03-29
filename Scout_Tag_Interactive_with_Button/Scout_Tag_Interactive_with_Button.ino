/* This is the main code for your LED Matrix device.
 * It makes use of the MD_MAX72XX Library, so make sure to include it
 * This works for the 24x8 LED Matrix by Skippy and Wickie, created during the PFF 2019 Mosaik
 * Because that chip is not like any other system in the library, the code wont work on other MAX7219
 * If the device doesn't work, ensure that there are no short-circuits on the board
 * If the Arduino runs hot, disconnect the battery immediately, as it could harm it.
 *
 *  the alphabet includes these letters
 *  QWERTZUIOPASDFGHJKLYXCVBNM
 *  1234567890+%&/()=?'![]<>{}$
 *  
*/

//set the intensity here, from 0 to 7 (a low value is recommended)
#define intensity 0

//you should not change any of the following!
// 
#include <EEPROM.h>
#include <MD_MAX72xx.h>
#define HARDWARE_TYPE -1
#define MAX_DEVICES  3

#define CLK_PIN     3  // or SCK
#define DATA_PIN    5  // or MOSI
#define CS_PIN      4  // or SS
#define KILL_SWITCH 2  // Button to Stop

MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// We always wait a bit between updates of the display
#define  DELAYTIME  100  // in milliseconds

// Adresses in EEPROM
#define ADDRESS_LEN 0
#define ADDRESS_BUF 1
#define MAX_BUF_LEN 100

byte str[MAX_BUF_LEN+9];
byte str_len;

const char* text_array[] = {
  "Hallo, ich bin dein Scout-Tag!        ",
  "Pfadi BezBEO FTW        ",
  "Lööte, Funke, Chätte....        "
};
const int text_array_len = 3;
volatile int text_array_choice = 0;

void scrollText(const unsigned char *p)
{
  uint8_t charWidth;
  uint8_t cBuf[8];  // this should be ok for all built-in fonts

  mx.clear();

  while (*p != '\0')
  {
    charWidth = mx.getChar(*p++, sizeof(cBuf) / sizeof(cBuf[0]), cBuf);
    for (uint8_t i=0; i<=charWidth; i++)  // allow space between characters
    {
      mx.transform(MD_MAX72XX::TSL);
      if (i < charWidth){
        cBuf[i] = ((cBuf[i] & 0xFE) >> 1) | ((cBuf[i] & 0x01) << 7);
        mx.setColumn(0, cBuf[i]);
      }

      delay(DELAYTIME);
    }
  }
}

// Der Knopf gibt normalerweise ein high-Signal auf dem Pin 2 (Die Konstante KILL_SWITCH)
// Also wird "rising" ausgelöst, wenn der Knopf losgelassen wird.
void handle_button_falling() {
  text_array_choice = (text_array_choice + 1) % (text_array_len + 2);
}

void setup()
{
  Serial.begin(9600);
  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY, intensity);
  pinMode(KILL_SWITCH, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(KILL_SWITCH), handle_button_falling, FALLING);
  Serial.println("ScoutTag, JotaJoti BEO. Ich bin deine LED-Matrix.");

  EEPROM.get(ADDRESS_LEN, str_len);
  EEPROM.get(ADDRESS_BUF, str);

  if (str_len == 255) {
    str_len = 0;
    EEPROM.write(ADDRESS_LEN, str_len);
  }
  str[str_len] = '\0';
}

void loop()
{
  if (text_array_choice < text_array_len) {
    scrollText(text_array[text_array_choice]);
  } else if (text_array_choice == text_array_len) {
    scrollText(str);
  } else {
    delay(1000);
  }
 
  if (Serial.available()) {
    size_t new_len = Serial.readBytesUntil('\n', str, MAX_BUF_LEN);
    if (new_len >= 240) {
      str_len = 240;
    } else {
      str_len = (byte)new_len;
    }
    for (int i = 0; i < 8; i++) {
      str[str_len + i] = ' ';
    }
    str[str_len+8] = 0;
    str_len += 9;
    Serial.write(str, str_len);
    Serial.println();
    EEPROM.put(ADDRESS_LEN, str_len);
    EEPROM.put(ADDRESS_BUF, str);
  }
}
