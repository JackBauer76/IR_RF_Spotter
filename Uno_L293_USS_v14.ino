#include "Timer.h"
#include <AFMotor.h>
#include <Servo.h>
AF_DCMotor motor1(1, MOTOR12_64KHZ); // create motor #2 at 64KHz pwm
AF_DCMotor motor2(2, MOTOR12_64KHZ); // create motor #2 at 64KHz pwm
Timer t;  
#define trigPin A5
#define echoPin A4
#define trigPin2 A2
#define echoPin2 A3

volatile long duration, distance_fine;
volatile long durationBack, distance_fineBack;
int randNumber;
int randNumber2;
int randomdir;
int randomturning;
int rand_speed;

//////////////////// Setup ///////////////////////////

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(2, OUTPUT);
  distance_fine =t.every(10, readDistance);
  distance_fineBack =t.every(20, readDistanceBack);
  motor1.run(RELEASE);     // release the motor
  motor2.run(RELEASE);     // release the motor
}

///////////////////////// Functions////////////////////

int readDistance() {
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  if(duration ==0)
  {
    duration = 10000;
  }
  distance_fine = (duration/2) / 29.1;
  return distance_fine;
}


int readDistanceBack() {
  digitalWrite(trigPin2, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin2, LOW);
  durationBack = pulseIn(echoPin2, HIGH);
  if(durationBack ==0)
  {
    durationBack = 10000;
  }
  distance_fineBack = (durationBack/2) / 29.1;
  return distance_fineBack;
}

void forward(int DC_Speed){
  Serial.println("****************Forwards");
  motor1.run(BACKWARD);      // turn it on going forward
  motor1.setSpeed(DC_Speed);
  motor2.run(BACKWARD);      // turn it on going forward
  motor2.setSpeed(DC_Speed);
  }

void backward(int DC_Speed){
  Serial.println("****************Backwards");
  motor1.run(FORWARD);      // turn it on going forward
  motor1.setSpeed(DC_Speed);
  motor2.run(FORWARD);      // turn it on going forward
  motor2.setSpeed(DC_Speed);
}

void turnleft(int DC_Speed){
  Serial.println("************Turn left");
  motor1.run(BACKWARD);      // turn it on going forward
  motor1.setSpeed(DC_Speed);
  motor2.run(FORWARD);      // turn it on going forward
  motor2.setSpeed(DC_Speed);
}

void turnright(int DC_Speed){
  Serial.println("************ Turn right");
  motor1.run(FORWARD);      // turn it on going forward
  motor1.setSpeed(DC_Speed);
  motor2.run(BACKWARD);      // turn it on going forward
  motor2.setSpeed(DC_Speed);    
}

void stop_engine(){
  Serial.println("************ Engines stopped");
  motor1.run(RELEASE);     // release the motor
  motor2.run(RELEASE);     // release the motor
}

int randomspeed(){
  rand_speed = random(60, 255);
  return rand_speed;
}

int randomtime(){
  randNumber2 = random(20, 60);
  return randNumber2;
}

int randomdirection(){
  randomdir = random(0,3);
  return randomdir;
}

int randomturn(){
  randomturning = random(0,1);
  return randomturning;
}




//////////////////// Loop///////////////////////////////

void loop() 
{
    t.update();
    if (distance_fine > 120)
    {
        forward(randomspeed());
    }
    else if (distance_fine < 30)
    {
        backward(randomspeed());
        if(distance_fineBack < 30)
        {

            if(randomturning==1){
              turnleft(randomspeed());  
            }
            else
            {
              turnright(randomspeed());
            }
            
            if(distance_fineBack <20 && distance_fine <20)
            {
                if(randomturning==1)
                {
                  turnleft(randomspeed());  
                }
                else
                {
                  turnright(randomspeed());
                }
            }
             
        }
        
     }
    else
    {
      switch (randomdirection()) 
      {
         case 0:
            forward(randomspeed());
            break;
          case 1:
            backward(randomspeed());
            break;
          case 2:
            turnright(randomspeed());  
            break;
          case 3:  
            turnleft(randomspeed());  
            break;
      }
      
    }
}

