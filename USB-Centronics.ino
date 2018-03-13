/*
This sketch translates the serial input to parallel output.

It can be used to print on a parallel printer using the default Generic / Text-only driver
in Windows if you don't have a parallel port.

The escape sequence for Start and End of print is <1B>E (for HP printers)

To use the default USB-serial connection you need to add a 10uF capacitor between the reset
line and ground.
*/

#define STROBE_WAIT 1 // 1ms strobe
#define SERIAL_BAUD 9600

// pin mappings
#define STROBE 2
#define ACK 11
#define BUSY 12

byte incomingByte = 0;   // for incoming serial data
byte pin[8];

// Set fonts routine (not used)
/*
byte setFont[] = {
  0x1B, 0x28, 0x31, 0x30, 0x55,
  0x1B, 0x28, 0x73, 0x31, 0x50,
  0x1B, 0x28, 0x73, 0x0A, 0x48,
  0x1B, 0x28, 0x73, 0x0C, 0x56,
  0x1B, 0x28, 0x73, 0x30, 0x53,
  0x1B, 0x28, 0x73, 0x30, 0x42,
  0x1B, 0x28, 0x73, 0x35, 0x32, 0x54
  }
*/

void setup()
{
  Serial.begin(SERIAL_BAUD);
  
  pinMode(STROBE, OUTPUT);      // is active LOW
  digitalWrite(STROBE, HIGH);   // set HIGH
   /*pinMode(ACK, INPUT);*/    // ACK not used
  pinMode(BUSY, INPUT);  

  pin[0] = 3;
  pin[1] = 4;
  pin[2] = 5;
  pin[3] = 6;
  pin[4] = 7;
  pin[5] = 8;
  pin[6] = 9;
  pin[7] = 10;
  
  for (uint8_t i = 0; i < 8; i++)
  {
    pinMode(pin[i], OUTPUT);
  }

  delay(1000);
  
  resetPrinter();
}

void loop()
{
  if (Serial.available() > 0)
  {
    // read the incoming byte
    incomingByte = Serial.read();
    
    // send to printer
    printByte(incomingByte);
  }
}

void printLine(char string[])
{
  for(int i = 0; i < strlen(string); i++)
  {  
    printByte(string[i]);
  }
 
  printByte(0xA); // new line
  printByte(0xD); // carriage return
}

void printByte(byte inByte)
{
  // wait for BUSY line to go low
  while(digitalRead(BUSY) == HIGH);

  for (uint8_t i = 0; i < 8; i++)
  {
    digitalWrite(pin[i], bitRead(inByte, i));    // direct port access will be faster
  }

  digitalWrite(STROBE, LOW);       // strobe STROBE to input data bits
  delayMicroseconds(STROBE_WAIT);
  digitalWrite(STROBE, HIGH);

  // wait for BUSY line to go low
  while(digitalRead(BUSY) == HIGH);
}

void resetPrinter()
{
  printByte(0x1B); // reset printer
  printByte('E');
}

