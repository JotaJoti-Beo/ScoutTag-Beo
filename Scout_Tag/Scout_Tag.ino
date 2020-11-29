/**this is the main code for your LED Matrix device.
 * It makes use of the MD_MAX72XX Library, so make sure to include it
 * This works for the 24x8 LED Matrix by Skippy and Wickie, created during the PFF 2019 Mosaik
 * Because that chip is not like any other system in the library, the code wont work on other MAX7219
 * If the device doesn't work, ensure that there are no short-circuits on the board
 * If the Arduino runs hot, disconnect the battery immediately, as it could harm it.
 *
 *  the alphabet includes these letters
 *  QWERTZUIOPASDFGHJKLYXCVBNM
 *
 *  1234567890+%&/()=?'![]<>{}$
*/


//write your Text here. You may only use the alphabet provided above , add some spaces in the back

#define plain "\x0F" "esto www.stefan-reusser.ch"

//set the intensity here, from 0 to 7 (a low value is recommended)
#define intensity 0

//you should not change any of the following!

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

void setup()
{
  Serial.begin(9600);
  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY, intensity);
  Serial.println("ScoutTag, JotaJoti BEO. Ich bin deine LED-Matrix.");
}

void loop()
{
  scrollText(plain "        ");
}
