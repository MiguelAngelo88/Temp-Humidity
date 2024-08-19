// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>
#include <dht11.h>
#include <Modbus.h>
#include <ModbusSerial.h>

// Mapping the pins
#define ONE_WIRE_BUS 2
#define DHT11PIN 7

// Modbus Registers Offsets
#define HUM 0
#define TEMP1 1
#define TEMP2 2

//objects declaration
OneWire oneWire(ONE_WIRE_BUS);
dht11 DHT11;
ModbusSerial mb;
DallasTemperature sensors(&oneWire);


void setup(void)
{
  // Config Modbus Serial (port, speed, byte format) 
  mb.config(&Serial, 9600, SERIAL_8N1);
  // Set the Slave ID (1-247)
  mb.setSlaveId(1); 

  // Start up the library
  sensors.begin();

  // Registers
  mb.addIreg(HUM);
  mb.addIreg(TEMP1);
  //mb.addIreg(TEMP2);

}

void loop(void)
{ 
    // Call once inside loop() - all magic here
    mb.task();

    // temperature read
    sensors.requestTemperatures(); // Send the command to get temperatures

    // After we got the temperatures, we can print them here.
    // We use the function ByIndex, and as an example get the temperature from the first sensor only.
    float temp1 = sensors.getTempCByIndex(0);

    //Sending to scadaBr the value of temp1
    mb.Ireg(TEMP1, temp1);

    //humidity read
    int chk = DHT11.read(DHT11PIN);
    float humidity = (float)DHT11.humidity;

    //Sending to scadaBr the value of humidity
    mb.Ireg(HUM, humidity);

}
