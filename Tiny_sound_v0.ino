const int Green = 4;
const int AnSensor = 1;
const int DgSensor = 2;
const int Red = 3;
int SoundLevel =0;
int SoundBarrier =0;

void setup() 
{
  pinMode(Green, OUTPUT);
  pinMode(AnSensor, INPUT);
  pinMode(DgSensor, INPUT);
  pinMode(Red, OUTPUT);
  digitalWrite(Red, HIGH);
  digitalWrite(Green, HIGH);
  delay(5000);
  digitalWrite(Green, LOW);
  digitalWrite(Red, LOW);
}

void loop() 
{
  SoundLevel=analogRead(AnSensor);
  SoundBarrier=digitalRead(DgSensor);
  if(SoundLevel>500)
  {
    digitalWrite(Green, HIGH);  
  }
  else if(DgSensor==1)
  {
    digitalWrite(Red, HIGH);  
  }
  else
  {
    digitalWrite(Red, LOW);
    digitalWrite(Green, LOW);  
  }
  
  

}
