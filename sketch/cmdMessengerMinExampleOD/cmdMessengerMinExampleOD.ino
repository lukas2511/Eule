// CmdMessenger library available from https://github.com/dreamcat4/cmdmessenger
#include <CmdMessenger.h>

// Streaming4 library available from http://arduiniana.org/libraries/streaming/
#include <Streaming.h>

// Drawbot portrait of Erik Satie
// (cc) 2008 AS220 Labs

#include <AFMotor.h>
#include <Servo.h> 

#define motorStepsA 240               
#define motorStepsB 240                                           
#define SCHRITTART INTERLEAVE // -- > SINGLE, DOUBLE. INTERLEAVE or MICROSTEP
#define SPEED 5

// Approximate number of steps per inch, calculated from radius of spool
// and the number of steps per radius
int StepUnit = 40;   

// Approximate dimensions of the total drawing area
int w= 4.5*StepUnit;
int h= 4.5*StepUnit;

AF_Stepper motorL(motorStepsA, 1);
AF_Stepper motorR(motorStepsB, 2);
Servo pen;

// Coordinates of current (starting) point
int x1= w/2;
int y1= h;

// Approximate length of strings from marker to staple
int a1= sqrt(pow(x1,2)+pow(y1,2));
int b1= sqrt(pow((w-x1),2)+pow(y1,2));

// Size of image array
int rows = 31;  
int columns = 23  ;

// Radius of pixel circles, in StepUnits
int radius = 40;   // equals about .4 inches  

// Size of page
int pageW = columns*radius*2;
int pageH = rows*radius*2;

// Size of one "pixel"
int cellW =2*radius;
int cellH = 2*radius;

// Coordinate of upper left corner of page
int page0X = x1-(pageW/2)-400;
int page0Y = y1-pageH-500;



// cmdMessenger changes by Oliver Dille
// merging cmdMessenger with Drawbot portrait of Erik Satie: habo

// Mustnt conflict / collide with our message payload data. Fine if we use base64 library ^^ above
char field_separator = ',';
char command_separator = ';';

// Attach a new CmdMessenger object to the default Serial port
CmdMessenger cmdMessenger = CmdMessenger(Serial, field_separator, command_separator);

// ------------------ C M D  L I S T I N G ( T X / R X ) ---------------------

// We can define up to a default of 50 cmds total, including both directions (send + recieve)
// and including also the first 4 default command codes for the generic error handling.
// If you run out of message slots, then just increase the value of MAXCALLBACKS in CmdMessenger.h

// Commands we send from the Arduino to be received on the PC
enum
{
  kCOMM_ERROR    = 000, // Lets Arduino report serial port comm error back to the PC (only works for some comm errors)
  kACK           = 001, // Arduino acknowledges cmd was received
  kARDUINO_READY = 002, // After opening the comm port, send this cmd 02 from PC to check arduino is ready
  kERR           = 003, // Arduino reports badly formatted cmd, or cmd not recognised

  kSEND_CMDS_END, // Mustnt delete this line
};

// Commands we send from the PC and want to recieve on the Arduino.
// We must define a callback function in our Arduino program for each entry in the list below vv.
// They start at the address kSEND_CMDS_END defined ^^ above as 004
messengerCallbackFunction messengerCallbacks[] = 
{
  moveX,  //004
  moveY, // 005
  penDown, //006
  movetoXY, //007
  
  NULL
};




// ------------------ C A L L B A C K  M E T H O D S -------------------------

void penDown() {
  int i=0;
  int penvalue=0;
  cmdMessenger.sendCmd(kACK,"Pen command received");
  
  while ( cmdMessenger.available() )
  {
    char buf[50] = { '\0' };
    cmdMessenger.copyString(buf, 50);
    if(buf[0]) {
      i++;
      if (i==1)
        penvalue = atoi(buf);
      cmdMessenger.sendCmd(kACK, buf);
    }
    if (i==1) {
      setPen(penvalue==1);
      Serial.print("Pen down set to :");
      Serial.print(penvalue==1);
      Serial.println(';');
    }    
  }
}

void moveY() {
  int i=0;
  int posY=0;
  
  cmdMessenger.sendCmd(kACK,"moveY command received");
  
  
  while ( cmdMessenger.available() )
  {
    char buf[50] = { '\0' };
    cmdMessenger.copyString(buf, 50);
    if(buf[0]) {
      i++;
      
      if (i==1)
        posY = atoi(buf);
        
      cmdMessenger.sendCmd(kACK, buf);
    }
    
    if (i==1) {
      Serial.print("move y: ");
      Serial.print(posY);
      Serial.println(';');
    }    
  }
  
  // Motor control code goes here;
  
  arduino_ready();
  
}

void moveX() {
  int i = 0;
  int posX = 0;
  
    cmdMessenger.sendCmd(kACK,"moveX command received");
  while ( cmdMessenger.available() )
  {
    char buf[50] = { '\0' };
    cmdMessenger.copyString(buf, 50);
    if(buf[0]) {
      i++;
      
      if (i==1)
        posX = atoi(buf);
        
      cmdMessenger.sendCmd(kACK, buf);
    }
    
    if (i==1) {
      Serial.print("move x: ");
      Serial.print(posX);
      Serial.println(';');
    }    
  }
  
  // Motor control code goes here;
  
  arduino_ready();
}

void movetoXY() {
  int i = 0;
  int posX = 0;
  int posY = 0;
  
    cmdMessenger.sendCmd(kACK,"moveX command received");
  while ( cmdMessenger.available() )
  {
    char buf[50] = { '\0' };
    cmdMessenger.copyString(buf, 50);
    if(buf[0]) {
      i++;
      
      if (i==1)
        posX = atoi(buf);
        
      if (i==2)
        posY = atoi(buf);
        
      cmdMessenger.sendCmd(kACK, buf);
    }
    
    if (i==2) {
      moveTo(posX,posY);
      Serial.print("move x: ");
      Serial.print(posX);
      Serial.print(" y: ");
      Serial.print(posY);
      Serial.println(';');
    }    
  }
  // Motor control code goes here;
  
  arduino_ready();
}


// ------------------ D E F A U L T  C A L L B A C K S -----------------------

void arduino_ready()
{
  // In response to ping. We just send a throw-away Acknowledgement to say "im alive"
  cmdMessenger.sendCmd(kACK,"ready cmd Example");
}

void unknownCmd()
{
  // Default response for unknown commands and corrupt messages
  cmdMessenger.sendCmd(kERR,"Unknown command");
}

// ------------------ E N D  C A L L B A C K  M E T H O D S ------------------

void attach_callbacks(messengerCallbackFunction* callbacks)
{
  int i = 0;
  int offset = kSEND_CMDS_END;
  while(callbacks[i])
  {
    cmdMessenger.attach(offset+i, callbacks[i]);
    i++;
  }
}


void setup() 
{
  delay(1000);
  // Listen on serial connection for messages from the pc
  // Serial.begin(57600);  // Arduino Duemilanove, FTDI Serial
  Serial.begin(9600); // Arduino Uno, Mega, with AT8u2 USB
  delay(1000);

  motorL.setSpeed(SPEED);
  motorR.setSpeed(SPEED);
  
  pen.attach(9);

  // cmdMessenger.discard_LF_CR(); // Useful if your terminal appends CR/LF, and you wish to remove them
  cmdMessenger.print_LF_CR();   // Make output more readable whilst debugging in Arduino Serial Monitor
  
  // Attach default / generic callback methods
  cmdMessenger.attach(kARDUINO_READY, arduino_ready);
  cmdMessenger.attach(unknownCmd);

  // Attach my application's user-defined callback methods
  attach_callbacks(messengerCallbacks);

  arduino_ready();

  // blink
 // pinMode(13, OUTPUT);
}

float rads(int n) {
  // Return an angle in radians
  return (n/180.0 * PI);
}                                                                        

void moveTo(int x2, int y2) {
    Serial.println("moveto");

  // Turn the stepper motors to move the marker from the current point (x1,
  // y1) to (x2, y2)
  // Note: This only moves in a perfectly straight line if 
  // the distance is the same in both dimensions; this should be fixed, but it
  // works well
  
  // a2 and b2 are the final lengths of the left and right strings
  int a2 = sqrt(pow(x2,2)+pow(y2,2));
  int b2 = sqrt(pow((w-x2),2)+pow(y2,2));
  int stepA;
  int stepB;
  if (a2>a1) { 
    stepA=1; 
  }
  if (a1>a2) { 
    stepA=-1;
  }
  if (a2==a1) {
    stepA=0; 
  }
  if (b2>b1) { 
    stepB=1; 
  }
  if (b1>b2) { 
    stepB=-1;
  }
  if (b2==b1) {
    stepB=0; 
  }
    Serial.print("A=");
    Serial.print(stepA,DEC);
    Serial.print("  B=");
    Serial.print(stepB,DEC);
    Serial.println("  ");

  // Change the length of a1 and b1 until they are equal to the desired length
  while ((a1!=a2) || (b1!=b2)) {
    Serial.print("a1=");
    Serial.print(a1,DEC);
    Serial.print("  a2=");
    Serial.print(a2,DEC);
    Serial.print("    b1=");
    Serial.print(b1,DEC);
    Serial.print("  b2=");
    Serial.print(b2,DEC);
    if (a1!=a2) { 
      a1 += stepA;
      //StepperA.step(stepA);
      if (stepA!=0)
        motorL.step(1, stepA<0?FORWARD:BACKWARD, SCHRITTART);   
    }
    if (b1!=b2) { 
      b1 += stepB;
//      StepperB.step(-stepB);
      if (stepB!=0)
        motorR.step(1, stepB>0?FORWARD:BACKWARD, SCHRITTART); 
    }
      Serial.print("   ");
      if (stepA!=0)
            Serial.print("A");
      if (stepB!=0)
            Serial.print("B");
    Serial.println("  ");
  }
  x1 = x2;
  y1=y2;
}

void drawCurve(float x, float y, float fx, float fy, float cx, float cy) {
  // Draw a Quadratic Bezier curve from (x, y) to (fx, fy) using control pt
  // (cx, cy)
  float xt=0;
  float yt=0;

  for (float t=0; t<=1; t+=.0025) {
    xt = pow((1-t),2) *x + 2*t*(1-t)*cx+ pow(t,2)*fx;
    yt = pow((1-t),2) *y + 2*t*(1-t)*cy+ pow(t,2)*fy;
    moveTo(xt, yt);
  }  
}
                                                     

void drawCircle(int centerx, int centery, int radius) {
  // Estimate a circle using 20 arc Bezier curve segments
  int segments =20;
  int angle1 = 0;
  int midpoint=0;
   
   moveTo(centerx+radius, centery);

  for (float angle2=360/segments; angle2<=360; angle2+=360/segments) {

    midpoint = angle1+(angle2-angle1)/2;

    float startx=centerx+radius*cos(rads(angle1));
    float starty=centery+radius*sin(rads(angle1));
    float endx=centerx+radius*cos(rads(angle2));
    float endy=centery+radius*sin(rads(angle2));
    
    int t1 = rads(angle1)*1000 ;
    int t2 = rads(angle2)*1000;
    int t3 = angle1;
    int t4 = angle2;

    drawCurve(startx,starty,endx,endy,
              centerx+2*(radius*cos(rads(midpoint))-.25*(radius*cos(rads(angle1)))-.25*(radius*cos(rads(angle2)))),
              centery+2*(radius*sin(rads(midpoint))-.25*(radius*sin(rads(angle1)))-.25*(radius*sin(rads(angle2))))
    );
    
    angle1=angle2;
  }

}


              
void drawCircles(int number, int centerx, int centery, int r) {
   // Draw a certain number of concentric circles at the given center with
   // radius r
   int dr=0;
   if (number > 0) {
     dr = r/number;
     for (int k=0; k<number; k++) {
       drawCircle(centerx, centery, r);
       r=r-dr;
     }
   }
}
          
void setPen(boolean p){
  if (p) {
    pen.write(200);  
    Serial.println("_");
  } else {
    pen.write(120);  
    Serial.println("o");
  }

}

// ------------------ M A I N ( ) --------------------------------------------

// Timeout handling
long timeoutInterval = 2000; // 2 seconds
long previousMillis = 0;
int counter = 0;


void timeout()
{
  // blink
//  if (counter % 2)
//    digitalWrite(13, HIGH);
//  else
//    digitalWrite(13, LOW);
  counter ++;
} 


void loop() 
{
  // Process incoming serial data, if any
  cmdMessenger.feedinSerialData();

  // handle timeout function, if any
  if (  millis() - previousMillis > timeoutInterval )
  {
    //timeout();
    previousMillis = millis();
  }

  // Loop.
}

