// All pins are in Arduino address space

// Periferials
// Serial LCD Tx
#define LCD_TX 24
// Serial LCD Rx
#define LCD_RX 25
// SD card CS
#define SD_CS 20
// SD card error LED
#define ERROR_LED 17

// Digital inputs
// DHT11 data pin
#define DHT_PIN 9

// Analog inputs
// GSR input
#define GSR_PIN 0
// Luminance photoresistor input
#define LUMI_PIN 1
// Body thermistor input
#define TEMP_PIN 2
// Environment thermistor input
#define EXT_TEMP_PIN 3
// Sound pressure level input
#define SOUND_PIN 4

// Thermistor pull-up resistor
#define TEMP_DIV_RES 10000

// buttons
// Debounce time, msec
#define BTN_DEBOUNCE 50
// Backlight on/off toggle
#define BTN_BACKLIGHT 10
// Tunnel/station mode toggle
#define BTN_MODE 11
// Start/stop the recording
#define BTN_RECORD 12