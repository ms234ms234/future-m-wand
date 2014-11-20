#include <avr/io.h>

int chipTempOffset = 324;
float chipTempCoeff = 1.22;

int GetTemp(void)
{
  unsigned int wADC;
  double t;

  // The internal temperature has to be used
  // with the internal reference of 1.1V.
  // Channel 8 can not be selected with
  // the analogRead function yet.

  // Set the internal reference and mux.
  ADMUX = (_BV(REFS1) | _BV(REFS0) | _BV(MUX3));
  ADCSRA |= _BV(ADEN);  // enable the ADC

  delay(20);            // wait for voltages to become stable.

  ADCSRA |= _BV(ADSC);  // Start the ADC

  // Detect end-of-conversion
  while (bit_is_set(ADCSRA,ADSC));

  // Reading register "ADCW" takes care of how to read ADCL and ADCH.
  wADC = ADCW;

  t = (wADC - chipTempOffset ) / chipTempCoeff;

  // The returned temperature is in degrees Celcius.
  return (signed byte) t;
}

void setup()
{
  Serial.begin(9600);
  pinMode(3,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
}

//signed byte temper = 0;
signed int temper=-50;
char h; // helper
void loop()
{
  temper = GetTemp();
  lightUp(temper);
  //h = temper%8;
  //lightRGB(temper%255,temper/10%255,temper/100%255);
  delay(100);
}

void lightRGB(unsigned char r, unsigned char g, unsigned char b)
{
  // Control the RGB diode (R on pin 6, G on pin 5, B on pin 3, 3.3V on the other side)
  Serial.print("Lighting up ");
  Serial.print(r);Serial.print(" ");
  Serial.print(g);Serial.print(" ");
  Serial.println(b);
  analogWrite(3, 255-b);
  analogWrite(5, 255-g);
  analogWrite(6, 255-r);
}

void lightUp(int t)
{
  // Map the temperature to color and light up the diode
  Serial.print("Temperature "); Serial.println(t);
  if(t < -40) // erratically low
    lightRGB(0,0,millis()); // blue "blink"
  else if(t < 0)
    lightRGB(map(t,-40,-1,0,255),
             map(t,-40,-1,0,200),
             255);
  else if(t<16)
    lightRGB(0,
             255,
             map(t,0,15,255,0));
  else if(t<27)
    lightRGB(map(t,16,26,0,255),
             255,
             0);
  else if(t<47)
    lightRGB(255,
             map(t,27,46,255,0),
             0);
  else if(t<81)
    lightRGB(255,
             map(t,47,80,0,255),
             map(t,47,80,0,255));
  else  // erratically high
    lightRGB(millis(),0,0);  // red "blink"
}

