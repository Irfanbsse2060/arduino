void setup()
{
  Serial.begin(9600);
}

void loop()
{
  unsigned int AnalogValue;
  AnalogValue = analogRead(0);
  Serial.println(AnalogValue);  
}

