// Adafruit Motor shield library
// copyright Adafruit Industries LLC, 2009
// this code is public domain, enjoy!

#include <AFMotor.h>
#include <Servo.h>

AF_Stepper motor_r(240, 1);
AF_Stepper motor_l(240, 2);
Servo servo;

void setup() {
  Serial.begin(115200);           // set up Serial library at 9600 bps
  Serial.println("Stepper test!");
  servo.attach(9);
  motor_l.setSpeed(100);  // 10 rpm   
  motor_r.setSpeed(100);  // 10 rpm  
  motor_l.step(1,FORWARD,DOUBLE); 
  motor_r.step(1,FORWARD,DOUBLE); 
}

void loop() {
  if(Serial.available()){
    char c=Serial.read();
    if(c=='5'){
      servo.write(100);
    }
    if(c=='6'){
      servo.write(55);
    }
    if(c=='1'){
      motor_l.step(1, FORWARD, DOUBLE); 
      motor_r.step(1, FORWARD, DOUBLE); 
      delay(20);      
    }
    if(c=='2'){
      motor_l.step(1, BACKWARD, DOUBLE); 
      motor_r.step(1, BACKWARD, DOUBLE);       
      delay(20);      
    }
    if(c=='3'){
      motor_l.step(1, BACKWARD, DOUBLE); 
      motor_r.step(1, FORWARD, DOUBLE);       
      delay(20);      
    }
    if(c=='4'){
      motor_l.step(1, FORWARD, DOUBLE); 
      motor_r.step(1, BACKWARD, DOUBLE);       
      delay(20);      
    }
  }
}
