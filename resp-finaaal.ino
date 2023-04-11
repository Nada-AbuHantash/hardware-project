
#include <NewPing.h>
#define MAX_DISTANCE 400 // Maximum distance (in cm) to ping.

#define TRIGGER_PIN_L  35  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN_L     34  // Arduino pin tied to echo pin on the ultrasonic sensor.

NewPing sonar_L(TRIGGER_PIN_L, ECHO_PIN_L, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

#define TRIGGER_PIN_R  30  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN_R     31 // Arduino pin tied to echo pin on the ultrasonic sensor.

NewPing sonar_R(TRIGGER_PIN_R, ECHO_PIN_R, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

#define TRIGGER_PIN_F  32  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN_F     33 // Arduino pin tied to echo pin on the ultrasonic sensor.

NewPing sonar_F(TRIGGER_PIN_F, ECHO_PIN_F, MAX_DISTANCE); // NewPing setup of pins and maximum distance.


#include <Servo.h>

Servo myservo;
#define Servoo 6

int pos = 0;

#define flame_Left 23      // left sensor
#define flame_Right 24    // right sensor
#define flame_Forward A15 //front sensor < 860 fire 
#define flame_MaxRight 25
#define flame_MaxLeft 22

#define pump 7
#define Level A14
#define buzeer 13

#define Motor_Right_Forward 2
#define Motor_Right_Backward 3
#define Motor_Left_Forward 4
#define Motor_Left_Backward 5

int Ultrasonic_L;
int Ultrasonic_F;
int Ultrasonic_R;
int coun = 0;

boolean fire = false;
boolean autoMovee = false;

char Value = 0;
char Old_Value = 1;

short Speed = 150;

int sensorReading = 0;


void setup() {
  //flame
  pinMode(flame_Left, INPUT);
  pinMode(flame_Right, INPUT);
  pinMode(flame_Forward, INPUT);
  pinMode(flame_MaxRight, INPUT);
  pinMode(flame_MaxLeft, INPUT);

  pinMode(pump, OUTPUT);
  pinMode(buzeer, OUTPUT);

  pinMode(Motor_Left_Forward, OUTPUT);
  pinMode(Motor_Left_Backward, OUTPUT);
  pinMode(Motor_Right_Forward, OUTPUT);
  pinMode(Motor_Right_Backward, OUTPUT);
  //pinMode(WATER, INPUT);



  Serial.begin(9600);

  Serial3.begin(115200);

  myservo.attach(Servoo); // attaches the servo on pin 17 to the servo object
  myservo.write(90);

}


void loop() {


  while (!autoMovee) {
    if (Serial3.available() > 0) {
      Value = Serial3.read();
      Serial.println(Value);
      if ( Value != Old_Value ) {
        Old_Value = Value;
        DoSomethingWith(Value);
      }
    }

    sensorReading = analogRead(flame_Forward);

    Serial.println(sensorReading);

  }
  while (autoMovee) {
    if (Serial3.available() > 0) {
      Value = Serial3.read();
      Serial.println(Value);
      if ( Value == 'M') {
        autoMovee = false;
      }
    }
    autoMove();

  }

}

void DoSomethingWith(char input)
{

  switch (input) {
    case 'F':
      Move_Forward();
      break;
    case 'B':
      Move_Backward();
      break;
    case 'L':
      Turn_Left();
      break;
    case 'R':
      Turn_Right();
      break;
    case 'S':
      Stop();
      break;
    case 'A':
      autoMovee = true;
      break;
    case 'M':
      autoMovee = false;
      break;
    case 'P':
      if (sensorReading < 1000) put_off_fire();
      break;
    default :
      break;
  }
}

void Move_Forward() {
  digitalWrite(Motor_Left_Backward, Speed);
  digitalWrite(Motor_Left_Forward, 0);
  digitalWrite(Motor_Right_Backward, 0 );
  digitalWrite(Motor_Right_Forward, Speed);
}

void Move_Backward() {
  digitalWrite(Motor_Left_Backward, 0);
  digitalWrite(Motor_Left_Forward, Speed);
  digitalWrite(Motor_Right_Backward, Speed);
  digitalWrite(Motor_Right_Forward, 0);

}

void Turn_Right() {
  digitalWrite(Motor_Left_Backward, Speed);
  digitalWrite(Motor_Left_Forward, 0);
  digitalWrite(Motor_Right_Backward, Speed);
  digitalWrite(Motor_Right_Forward, 0);
}

void Turn_Left() {
  digitalWrite(Motor_Left_Backward, 0);
  digitalWrite(Motor_Left_Forward, Speed);
  digitalWrite(Motor_Right_Backward, 0);
  digitalWrite(Motor_Right_Forward, Speed);
}

void Stop() {
  digitalWrite(Motor_Left_Forward, 0);
  digitalWrite(Motor_Left_Backward, 0);
  digitalWrite(Motor_Right_Forward, 0);
  digitalWrite(Motor_Right_Backward, 0);
}

void autoMove()
{


  myservo.write(90); //Sweep_Servo();
  sensorReading = analogRead(flame_Forward);
  Serial.println(sensorReading);
  if (digitalRead(flame_Left) == 1 && digitalRead(flame_Right) == 1 && analogRead(flame_Forward) > 1000 && digitalRead(flame_MaxRight) == 1 && digitalRead(flame_MaxLeft) == 1)
  {
    ultra();
    if (Serial3.available() > 0) {
      Value = Serial3.read();
      Serial.println(Value);
      if ( Value == 'M') {
        autoMovee = false;
      }
    }
  }
  else if (analogRead(flame_Forward) < 1000)
  {
    Move_Forward();

    if (sensorReading < 900)
    {
      fire = true;
    }
  }
  else if (digitalRead(flame_Right) == 0 || digitalRead(flame_MaxRight) == 0)
  {
    Turn_Right();
  }
  else if ( digitalRead(flame_MaxLeft) == 0 || digitalRead(flame_Left) == 0)
  {
    Turn_Left();
  }

  delay(400);//change this value to increase the distance

  while (fire == true)
  {
    put_off_fire();
    if (Serial3.available() > 0) {
      Value = Serial3.read();
      Serial.println(Value);
      if ( Value == 'M') {
        autoMovee = false;
        fire = false;
      }
    }
  }
}

void put_off_fire()
{
  Stop();
  //delay (500);

  if (analogRead(Level) > 200 ) {
    digitalWrite(pump, HIGH);
    digitalWrite(buzeer, LOW);
    delay(500);
  }
  else {
    digitalWrite(pump, LOW);
    digitalWrite(buzeer, HIGH);
    delay(500);
  }

  for (pos = 10; pos <= 130; pos += 1) {
    myservo.write(pos);
    delay(10);
  }
  for (pos = 130; pos >= 10; pos -= 1) {
    myservo.write(pos);
    delay(10);
  }

  if (analogRead(flame_Forward)  > 1000)
  {
    digitalWrite(pump, LOW);
    digitalWrite(buzeer, LOW);
    myservo.write(90);
    fire = false;
  }

}

void ultra() {

  delay(50);
  unsigned int L = sonar_L.ping();
  if ((L / US_ROUNDTRIP_CM) != 0)
    Ultrasonic_L = (L / US_ROUNDTRIP_CM);
  delay(50);

  unsigned int R = sonar_R.ping();
  if ((R / US_ROUNDTRIP_CM) != 0)
    Ultrasonic_R = (R / US_ROUNDTRIP_CM);
  delay(50);
  unsigned int F = sonar_F.ping();
  if ((F / US_ROUNDTRIP_CM) != 0)
    Ultrasonic_F = (F / US_ROUNDTRIP_CM);
  /*
    Serial.print("from forward :");
    Serial.println(Ultrasonic_F);
    Serial.print("from right :");
    Serial.println(Ultrasonic_R);
    Serial.print("from left :");
    Serial.println(Ultrasonic_L);
  */



  if (Ultrasonic_F > 50 && Ultrasonic_R > 20 &&  Ultrasonic_L > 20 )
  {

    if (Ultrasonic_F > 20) {
      coun = 0;
      Move_Forward();
      delay(50);
    }
    else {
      Stop();
      delay(50);
    }
  }
  else if (Ultrasonic_R > Ultrasonic_L )
  {
    if (Ultrasonic_R > 20) {
      coun = 0;
      Turn_Right();
      delay(50);
    }
    else {
      Stop();
      delay(50);
    }
  }
  else  if (Ultrasonic_L > Ultrasonic_R  )
  {
    if (Ultrasonic_L > 20) {
      coun = 0;
      Turn_Left();
      delay(50);
    }
    else {
      Stop();
      delay(50);
    }

  }
  else
  {
    if (coun < 4) {
      Stop();
      delay(50);
      Move_Backward();
      delay(100);
      Stop();
      coun++;
    }
  }



}
