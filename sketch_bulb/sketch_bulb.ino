
int in1=13;

void setup() {
  // put your setup code here, to run once:
  pinMode(in1,OUTPUT);
  digitalWrite(in1,HIGH);
  

}

void loop() {
  // put your main code here, to run repeatedly:
   digitalWrite(in1,LOW);
   delay(3000);
   digitalWrite(in1,HIGH);
   delay(3000);

}
