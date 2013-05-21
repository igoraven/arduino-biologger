// Log record format
typedef struct
{
  // Milliseconds from start of the record
  long millis;
  // Environment type (tunnel/station)
  char type;
  // GSR level, normalized to 0..999
  int gsr;
  // External thermistor reading, in Celsius
  int externalTemperatureAnalog;
  // DHT11 temperature reading, in Celsius
  int externalTemperature;
  // DHT11 relative humidity reading, in percent
  int externalHumidity;
  // Body thermistor reading, in Celsius
  int bodyTemperature;
  // Luminance level, normalized to 0..999
  int luminance;
  // Sound pressure level, normalized to 0.999
  int soundPressure;
} LoggerRecord;

#define LOGGER_RECORD_TYPE_STATION 'S'
#define LOGGER_RECORD_TYPE_TUNNEL 'T'

// logger cache size
#define LOGGER_ARRAY_SIZE 32
// delay between log entries
#define LOGGER_DELAY 500

// CSV file header
#define LOGGER_FILE_HEADER "millis,type,gsr,bodyTemp,envTempDigi,envTempAnalog,envHumidity,envLumi,envSound"
// CSV file field delimiter
#define LOGGER_FILE_VALUE_SEPARATOR ','