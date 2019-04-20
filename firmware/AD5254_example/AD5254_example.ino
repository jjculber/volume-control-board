// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// AD5254
// This code is designed to work with the AD5254_I2CPOT_10K I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Potentiometers?sku=AD5254_I2CPOT_10K#tabs-0-product_tabset-2

#include <Wire.h>
#include <Encoder.h>

// AD5254 I2C address is 0x2C(44)
#define Addr 0x2F

Encoder myEnc(1, 2);
const int buttonPin = 0;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin

void setup()
{
  
  // Initialise I2C communication as Master
  Wire.begin();
  
  // Initialise serial communication, set baud rate = 9600
  Serial.begin(9600);
  
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);

  mute();
  unmute();
}
unsigned int volume = 0;
unsigned int muted = 0;
long oldPosition  = -999;
int ledState = LOW;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
int registeredPress = 0;
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers

void loop()
{
  readPushButtonState();
  readRotaryEncoderState();
}

void readRotaryEncoderState() {
  long newPosition = myEnc.read();
  if (newPosition < 0) {
    Serial.print("<0 = ");
    Serial.println(newPosition);
    newPosition = 0;
    myEnc.write(0);
    readAndPrintAllValues();
  }
  if (newPosition > 255) {
    Serial.print(">255 = ");
    Serial.println(newPosition);
    newPosition = 255;
    myEnc.write(255);
    setAllValues(newPosition);
    readAndPrintAllValues();
  }
  if (newPosition != oldPosition) {
    //unmute();
    Serial.print("mid = ");
    Serial.println(newPosition);
    oldPosition = newPosition;
    setAllValues(newPosition);
    readAndPrintAllValues();
  }
}

void readPushButtonState() {
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button 
  // (i.e. the input went from LOW to HIGH),  and you've waited 
  // long enough since the last press to ignore any noise:  

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  } 
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
    buttonState = reading;
    
    if (buttonState == LOW && registeredPress == 0) {
      ledState = ((ledState == LOW) ? HIGH : LOW);
      registeredPress = 1;
      // set the LED using the state of the button:
      if (ledState == HIGH) {
        mute();
      } else {
        unmute();
      }
    }
    if (buttonState == HIGH && registeredPress == 1) {
      registeredPress = 0;
    } 
  }
  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;
}

void mute() {
  ledState = HIGH;
  digitalWrite(ledPin, ledState);
  //setAllValues(0);

  Wire.beginTransmission(Addr);
  Wire.write(0x00);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.beginTransmission(Addr);
  Wire.write(0x01);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.beginTransmission(Addr);
  Wire.write(0x02);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.beginTransmission(Addr);
  Wire.write(0x03);
  Wire.write(0x00);
  Wire.endTransmission();
  
  Wire.beginTransmission(Addr);
  Wire.write(0x80);
  Wire.endTransmission();

  volume = 0;
  muted = 1;
  
  myEnc.write(0);
  Serial.println("muted");
  readAndPrintAllValues();
}

void unmute() {
  ledState = LOW;
  digitalWrite(ledPin, ledState);
  muted = 0;
  myEnc.write(1);
  setAllValues(1);
  Serial.println("unmuted");
  readAndPrintAllValues();
}

void readAndPrintAllValues() {
  unsigned int data = -1;

  // Start I2C transmission
  Wire.beginTransmission(Addr);
  // Select data register
  Wire.write(0x00);
  // Stop I2C transmission
  Wire.endTransmission();

  // Request 1 byte of data
  Wire.requestFrom(Addr, 1);

  // Read 1 byte of data
  if (Wire.available() == 1)
  {
    data = Wire.read();
  }
  // Convert the data
  float res_0 = (data / 256.0 ) * 10.0;

  // Start I2C transmission
  Wire.beginTransmission(Addr);
  // Select data register
  Wire.write(0x01);
  // Stop I2C transmission
  Wire.endTransmission();

  // Request 1 byte of data
  Wire.requestFrom(Addr, 1);

  // Read 1 byte of data
  if (Wire.available() == 1)
  {
    data = Wire.read();
  }
  // Convert the data
  float res_1 = (data / 256.0 ) * 10.0;

  // Start I2C transmission
  Wire.beginTransmission(Addr);
  // Select data register
  Wire.write(0x02);
  // Stop I2C transmission
  Wire.endTransmission();

  // Request 1 byte of data
  Wire.requestFrom(Addr, 1);

  // Read 1 byte of data
  if (Wire.available() == 1)
  {
    data = Wire.read();
  }
  // Convert the data
  float res_2 = (data / 256.0 ) * 10.0;

  // Start I2C transmission
  Wire.beginTransmission(Addr);
  // Select data register
  Wire.write(0x03);
  // Stop I2C transmission
  Wire.endTransmission();

  // Request 1 byte of data
  Wire.requestFrom(Addr, 1);

  // Read 1 byte of data
  if (Wire.available() == 1)
  {
    data = Wire.read();
  }
  // Convert the data
  float res_3 = (data / 256.0 ) * 10.0;

  // Output data to serial monitor
  Serial.print("Resistance Channel-0 : ");
  Serial.print(res_0);
  Serial.println(" K");
  Serial.print("Resistance Channel-1 : ");
  Serial.print(res_1);
  Serial.println(" K");
  Serial.print("Resistance Channel-2 : ");
  Serial.print(res_2);
  Serial.println(" K");
  Serial.print("Resistance Channel-3 : ");
  Serial.print(res_3);
  Serial.println(" K");
}

void setAllValues(unsigned int val) {
  if (muted == 1) {
    return;
  }
  uint8_t command = 0x80; // nop
  if (volume > val) {
    // decrease volume
    command = 0xb0;
  } else if (volume < val) {
    // increase volume
    command = 0xd8;
  }
  Serial.println(command);

  Wire.beginTransmission(Addr);
  Wire.write(command);
  Wire.endTransmission();
  
  Wire.beginTransmission(Addr);
  Wire.write(0x80);
  Wire.endTransmission();

  volume = val;
  return;


  // Start I2C transmission
  Wire.beginTransmission(Addr);
  // Send instruction for POT channel-0
  Wire.write(0x00);
  // Input resistance value, 0x80(128)
  //Wire.write(val);
  // Stop I2C transmission
  Wire.endTransmission();

  // Start I2C transmission
  Wire.beginTransmission(Addr);
  // Send instruction for POT channel-1
  Wire.write(0x01);
  // Input resistance value, 0x80(128)
  Wire.write(val);
  // Stop I2C transmission
  Wire.endTransmission();

  // Start I2C transmission
  Wire.beginTransmission(Addr);
  // Send instruction for POT channel-2
  Wire.write(0x02);
  // Input resistance value, 0x80(128)
  Wire.write(val);
  // Stop I2C transmission
  Wire.endTransmission();

  // Start I2C transmission
  Wire.beginTransmission(Addr);
  // Send instruction for POT channel-3
  Wire.write(0x03);
  // Input resistance value, 0x80(128)
  Wire.write(val);
  // Stop I2C transmission
  Wire.endTransmission();

}


