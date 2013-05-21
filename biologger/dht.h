// Derived from http://playground.arduino.cc/main/DHT11Lib

// wait loop
#define DHT11_WAIT(cnt, pin, state) \
	cnt = 10000; \
	while (digitalRead(pin) == state) \
	{ \
		if (cnt-- == 0) return 0; \
	}

class Dht11
{
public:
	Dht11(uint8_t dataPin); // Constructor
	uint8_t Read(void); // Refresh the data
	uint8_t Temperature; // Temperature in Celsius
	uint8_t Humidity; // Relative humidity in %

private:
	uint8_t _pin;
};

Dht11::Dht11(uint8_t dataPin)
{
	this->_pin = dataPin;
}

// Refresh the data
// Returns 0 if timeout, 1 if no problem
uint8_t Dht11::Read(void)
{
	uint8_t bits[5];
	uint8_t cnt = 7;
	uint8_t idx = 0;
	uint8_t pin = this->_pin;

	memset(bits, 0, 5);

	pinMode(pin, OUTPUT);
	digitalWrite(pin, LOW);
	delay(18);
	digitalWrite(pin, HIGH);
	delayMicroseconds(40);
	pinMode(pin, INPUT);

	unsigned int loopCnt = 10000;
	DHT11_WAIT(loopCnt, pin, LOW);
	DHT11_WAIT(loopCnt, pin, HIGH);

	for (int i=0; i<40; i++)
	{
		DHT11_WAIT(loopCnt, pin, LOW);
		unsigned long t = micros();
		DHT11_WAIT(loopCnt, pin, HIGH);

		if ((micros() - t) > 40) bits[idx] |= (1 << cnt);
		if (cnt == 0)
		{
			cnt = 7;
			idx++;
		}
		else
		{
			cnt--;
		}
	}

	this->Humidity = bits[0];
	this->Temperature = bits[2];

	return 1;
}
