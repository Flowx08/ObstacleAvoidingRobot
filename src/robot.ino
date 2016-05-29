//Includes
#include <AFMotor.h>
#include "Qlearning.h"

//Constants
#define PIN_SENS_LIGHT     0    // Light sensor analog pin 0
#define PIN_SENS_ECHO      12   // Echo Pin
#define PIN_SENS_TRIG      13   // Trigger Pin

//Motors
AF_DCMotor motor1(1);
AF_DCMotor motor2(2);
const unsigned char motor_speed = 105;

//Collision detection variables
int collision = 0;
int distance = 100;
int act = 0;
float reward = 0;

//AI
Qstruct* q = 0;

//Get the distance from an object in cm
int robot_getdistance() 
{
	digitalWrite(PIN_SENS_TRIG, LOW);
	delayMicroseconds(2);
  
	digitalWrite(PIN_SENS_TRIG, HIGH);
	delayMicroseconds(10);
  
	digitalWrite(PIN_SENS_TRIG, LOW);
	return (pulseIn(PIN_SENS_ECHO, HIGH) / 58.2);
}

int robot_getlightpower()
{
  return analogRead(0);
}

void robot_action(int action)
{
  switch (action)
  {
     case 0:
           motor1.setSpeed(motor_speed);
           motor2.setSpeed(motor_speed);
           motor1.run(FORWARD);
           motor2.run(FORWARD); 
           break;
    
    case 1:
           motor1.setSpeed(motor_speed);
           motor2.setSpeed(motor_speed);
           motor1.run(FORWARD);
           motor2.run(BACKWARD);
           break;
           
    case 2:
           motor1.setSpeed(motor_speed);
           motor2.setSpeed(motor_speed);
           motor1.run(BACKWARD);
           motor2.run(FORWARD);
           break;
           
   case 3:
           motor1.setSpeed(motor_speed);
           motor2.setSpeed(motor_speed);
           motor1.run(BACKWARD);
           motor2.run(BACKWARD);
           break;
           
   case 4:
           motor1.setSpeed(0);
           motor2.setSpeed(0); 
           motor1.run(FORWARD);
           motor2.run(FORWARD);
           break;
  }
}

//Seutp I/O pins
void setup() {
        Serial.begin(9600);
	pinMode(PIN_SENS_TRIG, OUTPUT);
	pinMode(PIN_SENS_ECHO, INPUT);
        motor1.setSpeed(0);
        motor2.setSpeed(0);
        motor1.run(RELEASE);
        motor2.run(RELEASE);
        q = qstruct_init(15, 4);
        q->learning_rate = 0.05;
        q->longterm_learning = 0.6;
        q->exploration_factor = 0.25;     
}

float evaluate(int action, int distance) {
    float reward = 0;
    if (action == 0) reward = distance * 2; //Move straight
    else if (action == 3) reward = distance <= 1 ? 5 : 0; //Move backward
    else reward = distance; //Turn right or left
    return reward;
}

void loop() {
  
    //Wait
    delay(200);
    
    //Get distance from object
    distance = robot_getdistance();
    distance /= 7;
    distance = distance >= 14 ? 14 : distance;
    q->exploration_factor -= 0.0002;
    if (q->exploration_factor < 0.05) q->exploration_factor = 0.05;
        
    if (distance == 0)
    {
        collision++;
        if (collision > 3) {
          collision = 0;
          Serial.print("STOP / distance: ");
          Serial.print(distance);
          Serial.print("\n");
          //reward = distance;
          //robot_action(3);
          //delay(800);
          }
    }
    
    {
        collision = 0;
        qstruct_update(q, distance, reward);
        act = qstruct_takeaction(q, distance);
        robot_action(act);
        reward = evaluate(act, distance);
        Serial.print(" Action: ");
        Serial.print(act);
        Serial.print(" Sensor: ");
        Serial.print(distance);
        Serial.print(" Light: ");
        Serial.print(robot_getlightpower());
        Serial.print(" Reward: ");
        Serial.print(reward);
        Serial.print(" Exploration: ");
        Serial.print(q->exploration_factor);
        Serial.print("\n");
    }
}
