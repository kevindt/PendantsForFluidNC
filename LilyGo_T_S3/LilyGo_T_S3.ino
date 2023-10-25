/*
 * 
 * 
 *  Note: If the USB is not connected this program locks up when that serial port is used. 
 *        Maybe there are some floating pins that generate data.
 */

#include "Arduino.h"
#include "TFT_eSPI.h" /* Please use the TFT library provided by LilyGo. */
#include "pin_config.h"
#include "GrblParser.h"  // be sure to move the files into the library folder for Arduino

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5,0,0)
#error  "The current version is not supported for the time being, please use a version below Arduino ESP32 3.0"
#endif

/* The product now has two screens, and the initialization code needs a small change in the new version. The LCD_MODULE_CMD_1 is used to define the
   switch macro. */
#define LCD_MODULE_CMD_1
# define ECHO_RX_DATA
//#define DEBUG_USB

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite1 = TFT_eSprite(&tft); // Used to prevent flickering

// local copies so we can do one update function
String myState = "Idle";
float myAxes[MAX_N_AXIS] = {0};
int my_n_axis;

void updateDisplay();

#if defined(LCD_MODULE_CMD_1)
typedef struct {
  uint8_t cmd;
  uint8_t data[14];
  uint8_t len;
} lcd_cmd_t;

lcd_cmd_t lcd_st7789v[] = {
  {0x11, {0}, 0 | 0x80},
  {0x3A, {0X05}, 1},
  {0xB2, {0X0B, 0X0B, 0X00, 0X33, 0X33}, 5},
  {0xB7, {0X75}, 1},
  {0xBB, {0X28}, 1},
  {0xC0, {0X2C}, 1},
  {0xC2, {0X01}, 1},
  {0xC3, {0X1F}, 1},
  {0xC6, {0X13}, 1},
  {0xD0, {0XA7}, 1},
  {0xD0, {0XA4, 0XA1}, 2},
  {0xD6, {0XA1}, 1},
  {0xE0, {0XF0, 0X05, 0X0A, 0X06, 0X06, 0X03, 0X2B, 0X32, 0X43, 0X36, 0X11, 0X10, 0X2B, 0X32}, 14},
  {0xE1, {0XF0, 0X08, 0X0C, 0X0B, 0X09, 0X24, 0X2B, 0X22, 0X43, 0X38, 0X15, 0X16, 0X2F, 0X37}, 14},
};
#endif

class Displayer : public GrblParser
{

    void debug_message(String message)
    {

    }

    void parse_message(String message)
    {

    }

    void show_state(const String &state)
    {
      myState = state;
      updateDisplay();
    }

    void show_dro(const float *axes, bool isMpos, bool *limits) {
      my_n_axis = _n_axis;
      for (int i = 0; i < MAX_N_AXIS; i++) {
        myAxes[i] = axes[i];
      }
      updateDisplay();  // TO DO reduce the number of these
    }

    void show_gcode_modes(const gcode_modes &modes)
    {

    }

    void show_info_message(String message)
    {

    }

    void process_set_message(String message)
    {

    }

} displayer;

void setup()
{
  pinMode(PIN_POWER_ON, OUTPUT);
  digitalWrite(PIN_POWER_ON, HIGH);
  pinMode(PIN_BUTTON_2, INPUT);  // active low has physical pullup and RC

#ifdef DEBUG_USB
  Serial.begin(115200);   // used for debugging
  delay(2000);  // delay to allow USB connection of PC to connect
  Serial.println("Begin T-Display-S3");
#endif

  Serial1.begin(115200, SERIAL_8N1, RX1_PIN, TX1_PIN);   // connected to FluidNC

  tft.begin();
  tft.setRotation(3);
  tft.setSwapBytes(true);

  sprite1.setColorDepth(16);
  sprite1.createSprite(DISP_WIDTH, DISP_HEIGHT);
  sprite1.fillSprite(TFT_BLACK);

#if defined(LCD_MODULE_CMD_1)
  for (uint8_t i = 0; i < (sizeof(lcd_st7789v) / sizeof(lcd_cmd_t)); i++) {
    tft.writecommand(lcd_st7789v[i].cmd);
    for (int j = 0; j < (lcd_st7789v[i].len & 0x7f); j++) {
      tft.writedata(lcd_st7789v[i].data[j]);
    }

    if (lcd_st7789v[i].len & 0x80) {
      delay(120);
    }
  }
#endif

#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5,0,0)
  ledcSetup(0, 2000, 8);
  ledcAttachPin(PIN_LCD_BL, 0);
  ledcWrite(0, 255);
#else
  ledcAttach(PIN_LCD_BL, 200, 8);
  ledcWrite(PIN_LCD_BL, 255);
#endif

  tft.fillRect(0, 0, DISP_WIDTH, DISP_HEIGHT, TFT_BLACK);
  tft.drawString("FluidNC Channel pendant", 0 , 0, 4);
  delay(1000);
  Serial1.write('?'); // force a status update
}

void loop()
{
#ifdef DEBUG_USB
  while (Serial.available()) {
    char c = Serial.read();
    if (c != '\r')
    {
      Serial.print(c);
      Serial1.print(c);
    }
  }
#endif

  while (Serial1.available()) {
    char c = Serial1.read();
    if (c != '\r')
    {
#ifdef DEBUG_USB
      Serial.print(c);
#endif
      displayer.write(c);
    }
  }

  readButtons();
}

void updateDisplay() {
  char buf[12];
  String axesNames = "XYZABC";
 
  sprite1.fillSprite(TFT_BLACK);

  if (myState == "Alarm") {
    sprite1.setTextColor(TFT_RED, TFT_BLACK);
  } else if (myState.startsWith("Hold")) {
    sprite1.setTextColor(TFT_YELLOW, TFT_BLACK);
  } else {
    sprite1.setTextColor(TFT_GREEN, TFT_BLACK);
  }
  
  sprite1.drawString(myState, 0, 0, 4);
  for (int i = 0; i < my_n_axis; i++)
  {   
    sprintf(buf, "%s: %4.2f", axesNames.substring(i, i + 1), myAxes[i]); // Grrr %04.2f is broken on Arduinos
    sprite1.drawString(buf, 0 , 40 + i * 26, 4);
  }

  sprite1.pushSprite(0, 0);
}

void readButtons() {

  if (!digitalRead(PIN_BUTTON_2)) {
    if (myState == "Run") {
      Serial.write("!");
      Serial1.write("!");
    } else if (myState.startsWith("Hold")) {
      Serial.write("~");
      Serial1.write("~");
    }

    delay(50);
    while (!digitalRead(PIN_BUTTON_2)) {
    }
    delay(50);
  }
}
