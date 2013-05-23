#include <SerialLCD.h>
#include <SoftwareSerial.h>
#include <Bounce.h>
#include <SD.h>
#include "pins.h"
#include "logging.h"
#include "dht.h"

// methods declarations
LoggerRecord RecordValues(); // Store sensors' values in new entry
void SetupButtons(void); // Setup buttons' handlers
void ReactOnButtons(void); // Check buttons' handlers
void DisplayState(LoggerRecord lastRecord); // Refresh data on the LCD
void SaveLoggerData(void); // Write log entries to SD card
void ErrorLoop(void); // SD card error indicator loop
double ThermistorRead(uint16_t value); // Read thermistor values
void UpdateAnalogs(void); // Read analog sensors data

// globals
SerialLCD gSlcd(LCD_TX, LCD_RX); // LCD
volatile boolean gSlcdBacklight = true; // LCD backlight
volatile boolean gRecordData = false; // Is recording enabled?
LoggerRecord gRecords[LOGGER_ARRAY_SIZE]; // Log entries cache
uint8_t gRecordsCounter = 0; // Current log entry index
char gCurrentMode = LOGGER_RECORD_TYPE_1; // Current environment type
char gFileName[] = "data00.csv"; // Log file name mask
File gFile; // Log file handler
long gLastLoggerCycleTime = 0; // Last log entry time
LoggerRecord gLastRecord; // Current log entry
unsigned long gCurrentTimestamp; // Current log entry timestamp

// sensors
uint16_t sGsr;
uint16_t sBodyTemp;
uint16_t sExtTemp;
uint16_t sLuminance;
uint16_t sSoundLevel;
Dht11 sDht(DHT_PIN);

// buttons
Bounce gBacklightButton(BTN_BACKLIGHT, BTN_DEBOUNCE);
Bounce gRecordDataButton(BTN_RECORD, BTN_DEBOUNCE);
Bounce gModeButton(BTN_MODE, BTN_DEBOUNCE);

void setup()
{
  SetupButtons();
  gSlcd.begin();
  pinMode(SD_CS, OUTPUT);
  pinMode(ERROR_LED, OUTPUT);
  if (!SD.begin(SD_CS))
  {
    ErrorLoop();
  }
  delay(1000);
}

void loop()
{
  ReactOnButtons();
  UpdateAnalogs();

  if (millis() - gLastLoggerCycleTime >= LOGGER_DELAY)
  {
    gLastLoggerCycleTime = millis();
    gLastRecord = RecordValues();
    DisplayState(gLastRecord);
    if (gRecordData)
    {
      gRecords[gRecordsCounter++] = gLastRecord;
    }
    if (gRecordsCounter >= LOGGER_ARRAY_SIZE)
    {
      SaveLoggerData();
    }
  }
}

// Logging

LoggerRecord RecordValues()
{
  LoggerRecord rec;
  rec.millis = gCurrentTimestamp;
  rec.type = gCurrentMode;
  rec.gsr = map(sGsr, 0, 1023, 0, 999);
  rec.bodyTemperature = ThermistorRead(sBodyTemp);
  rec.externalTemperatureAnalog = ThermistorRead(sExtTemp);

  if (sDht.Read())
  {
    rec.externalTemperature = sDht.Temperature;
    rec.externalHumidity = sDht.Humidity;
  }
  // DHT11 values stub
  else
  {
    rec.externalHumidity = 0;
    rec.externalTemperature = 0;
  }
  rec.luminance = map(sLuminance, 0, 1023, 0, 999);
  rec.soundPressure = map(sSoundLevel, 0, 1023, 0, 999);
  
  gCurrentTimestamp += LOGGER_DELAY;
  
  return rec;
}

// Sensors code

void UpdateAnalogs(void)
{
  sGsr = analogRead(GSR_PIN);
  sBodyTemp = analogRead(TEMP_PIN);
  sExtTemp = analogRead(EXT_TEMP_PIN);
  sLuminance = analogRead(LUMI_PIN);
  sSoundLevel = analogRead(SOUND_PIN);
}

double ThermistorRead(uint16_t value)
{
  // Formula values are based on 10K Thermistor Epcos B57164-K 103-J
  if (!value) value = 1;
  unsigned long resistance = ((long)1024*TEMP_DIV_RES)/value - TEMP_DIV_RES;
  return 4300/(log(resistance/0.0054511412)) - 273.15;
}

// service

void SetupButtons(void)
{
  pinMode(BTN_BACKLIGHT, INPUT_PULLUP);
  pinMode(BTN_MODE, INPUT_PULLUP);
  pinMode(BTN_RECORD, INPUT_PULLUP);
  gBacklightButton.write(HIGH);
  gRecordDataButton.write(HIGH);
  gModeButton.write(HIGH);
}

void ReactOnButtons(void)
{
  if (gBacklightButton.update() &&
      gBacklightButton.fallingEdge())
  {
    gSlcdBacklight = !gSlcdBacklight;
    DisplayState(gLastRecord);
  }

  if (gRecordDataButton.update() &&
      gRecordDataButton.fallingEdge())
  {
    gRecordData = !gRecordData;
    if (gRecordData)
    {
      gCurrentTimestamp = 0;
      uint8_t cnt = 0;
      // find first available index for new file
      while (SD.exists(gFileName) && ++cnt < 100)
      {
        gFileName[4] = 0x30 + cnt / 10;
        gFileName[5] = 0x30 + cnt % 10;
      }
      gFile = SD.open(gFileName, FILE_WRITE);

      if (!gFile) ErrorLoop();

      gFile.println(LOGGER_FILE_HEADER);
    }
    else
    {
      SaveLoggerData();
      gFile.close();
    }
    DisplayState(gLastRecord);
  }

  if (gModeButton.update() &&
      gModeButton.fallingEdge())
  {
    if (gCurrentMode == LOGGER_RECORD_TYPE_1)
    {
      gCurrentMode = LOGGER_RECORD_TYPE_2;
    }
    else
    {
      gCurrentMode = LOGGER_RECORD_TYPE_1;
    }
    DisplayState(gLastRecord);
  }
}

void DisplayState(LoggerRecord lastRecord)
{
  if (gSlcdBacklight) gSlcd.backlight();
  else gSlcd.noBacklight();

  gSlcd.setCursor(0,0);
  if (gRecordData)
  {
    gSlcd.print("R_");
    gSlcd.print(gFileName[4]);
    gSlcd.print(gFileName[5]);
    gSlcd.print(' ');
  }
  else gSlcd.print("     ");

  gSlcd.print(lastRecord.type);
  gSlcd.print(' ');
  gSlcd.print(lastRecord.gsr, DEC);
  gSlcd.print("g ");
  gSlcd.print(lastRecord.bodyTemperature, DEC);
  gSlcd.print(0xDF); // degree symbol
  gSlcd.print("   ");
  
  gSlcd.setCursor(0, 1);
  gSlcd.print(lastRecord.externalTemperature, DEC);
  gSlcd.print(0xDF);
  gSlcd.print(' ');
  gSlcd.print(lastRecord.externalHumidity, DEC);
  gSlcd.print("% ");
  gSlcd.print(lastRecord.luminance, DEC);
  gSlcd.print("l ");
  gSlcd.print(lastRecord.soundPressure, DEC);
  gSlcd.print("s    ");
}

void SaveLoggerData(void)
{
  for (uint8_t i = 0; i < gRecordsCounter; i++)
  {
    LoggerRecord rec = gRecords[i];
    gFile.print(rec.millis);
    gFile.print(LOGGER_FILE_VALUE_SEPARATOR);
    gFile.print(rec.type);
    gFile.print(LOGGER_FILE_VALUE_SEPARATOR);
    gFile.print(rec.gsr);
    gFile.print(LOGGER_FILE_VALUE_SEPARATOR);
    gFile.print(rec.bodyTemperature);
    gFile.print(LOGGER_FILE_VALUE_SEPARATOR);
    gFile.print(rec.externalTemperature);
    gFile.print(LOGGER_FILE_VALUE_SEPARATOR);
    gFile.print(rec.externalTemperatureAnalog);
    gFile.print(LOGGER_FILE_VALUE_SEPARATOR);
    gFile.print(rec.externalHumidity);
    gFile.print(LOGGER_FILE_VALUE_SEPARATOR);
    gFile.print(rec.luminance);
    gFile.print(LOGGER_FILE_VALUE_SEPARATOR);
    gFile.print(rec.soundPressure);
    gFile.println("");
  }
  gRecordsCounter = 0;
  memset(gRecords, 0, sizeof(LoggerRecord)*LOGGER_ARRAY_SIZE);
}

void ErrorLoop(void)
{
  while (true)
  {
    digitalWrite(ERROR_LED, HIGH);
    delay(500);
    digitalWrite(ERROR_LED, LOW);
    delay(500);
  }
}
