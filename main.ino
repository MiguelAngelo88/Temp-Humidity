// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>
#include <dht11.h>
#include <Modbus.h>
#include <ModbusSerial.h>

// Mapping the pins
#define ONE_WIRE_BUS 5
#define DHT11PIN 7

// Modbus Registers Offsets
#define HUM_RAW 0
#define HUM_FILTERED 1
#define TEMP1_RAW 2
#define TEMP1_FILTERED 3
#define TEMP2_RAW 4
#define TEMP2_FILTERED 5

// Objects declaration
ModbusSerial mb;
OneWire oneWire(ONE_WIRE_BUS);
dht11 DHT11;
DallasTemperature sensors(&oneWire);

// Variables for filtering (using simple moving average as an example)
float temp1_filtered = 0;
float temp2_filtered = 0;
float hum_filtered = 0;
const float alpha = 0.5; // Filter factor for exponential moving average

void setup(void)
{
  // Config Modbus Serial (port, speed, byte format)
  mb.config(&Serial, 9600, SERIAL_8N1);

  // Set the Slave ID
  mb.setSlaveId(1);

  // Start up the library
  sensors.begin();

  // Registers for raw and filtered values
  mb.addIreg(HUM_RAW);
  mb.addIreg(HUM_FILTERED);
  mb.addIreg(TEMP1_RAW);
  mb.addIreg(TEMP1_FILTERED);
  mb.addIreg(TEMP2_RAW);
  mb.addIreg(TEMP2_FILTERED);
}

void loop(void)
{
  // Call once inside loop() - all magic here
  mb.task();

  // Temperature read
  sensors.requestTemperatures();

  float temp1_raw = sensors.getTempCByIndex(0);
  float temp2_raw = sensors.getTempCByIndex(1);

  // Apply simple exponential moving average filter
  temp1_filtered = alpha * temp1_raw + (1 - alpha) * temp1_filtered;
  temp2_filtered = alpha * temp2_raw + (1 - alpha) * temp2_filtered;

  // Sending to ScadaBR raw and filtered temperature values
  mb.Ireg(TEMP1_RAW, temp1_raw);
  mb.Ireg(TEMP1_FILTERED, temp1_filtered);
  mb.Ireg(TEMP2_RAW, temp2_raw);
  mb.Ireg(TEMP2_FILTERED, temp2_filtered);

  // Humidity read
  int chk = DHT11.read(DHT11PIN);
  float hum_raw = (float)DHT11.humidity;

  // Apply the same filter to humidity
  hum_filtered = alpha * hum_raw + (1 - alpha) * hum_filtered;

  // Sending to ScadaBR raw and filtered humidity values
  mb.Ireg(HUM_RAW, hum_raw);
  mb.Ireg(HUM_FILTERED, hum_filtered);
}
