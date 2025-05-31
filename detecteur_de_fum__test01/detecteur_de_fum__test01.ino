#include <MQ2_LPG.h>

const int mq2Pin= A0;
const int ledVert= 7;
const int ledRouge=8;
const int buzer=9;


int seuil= 500;

void setup() {
  // put your setup code here, to run once:
  pinMode(ledVert,OUTPUT);
  pinMode(ledRouge,OUTPUT);
  pinMode(buzer,OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int valeurGaz=analogRead(mq2Pin);
  Serial.println(valeurGaz);
  if(valeurGaz>seuil){
    Serial.print("gaz detécté");
    digitalWrite(ledVert,LOW);
    digitalWrite(ledRouge,HIGH);
    digitalWrite(buzer,HIGH);
  }else{
    Serial.print("air propre");
    digitalWrite(ledVert,HIGH);
    digitalWrite(ledRouge,LOW);
    digitalWrite(buzer,LOW);
  }
  delay(500);
}
