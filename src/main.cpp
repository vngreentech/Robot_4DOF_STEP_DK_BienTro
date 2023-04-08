#include <Arduino.h>
#include <Servo.h>
#include <AccelStepper.h>
#include <MultiStepper.h>

Servo servo_kep;

int step_X = 54; int Dir_X = 55; int ena_X = 38;
int step_Y = 60; int Dir_Y = 61; int ena_Y = 56;
int step_Z = 46; int Dir_Z = 48; int ena_Z = 62;

AccelStepper Step_X(1, step_X, Dir_X, ena_X);
AccelStepper Step_Y(1, step_Y, Dir_Y, ena_Y);
AccelStepper Step_Z(1, step_Z, Dir_Z, ena_Z);

#define end_X  3
#define end_Y  14
#define end_Z  18

int bt_X = A3; 
int bt_Y = A4; 
int bt_Z = A5; 
int nutnhan = A10;

int gt_bt_X; 
int gt_bt_Y; 
int gt_bt_Z;
int gt_nutnhan; bool macdinh=1; unsigned int dem=0;

int bandau_X = 0; 
int bandau_Y = 0; 
int bandau_Z = 0;

int buoc_X; 
int buoc_Y; 
int buoc_Z;
 
void home_YZ()
{
  int homeY = 0; int homeZ = 0;

  Step_Y.setMaxSpeed(100);
  Step_Y.setAcceleration(100);
  Step_Z.setMaxSpeed(100);
  Step_Z.setAcceleration(100);  
  Step_Y.enableOutputs();
  Step_Z.enableOutputs();

  while(digitalRead(end_Y)==1 and digitalRead(end_Z)==1)
  {
    Step_Y.moveTo(homeY);
    Step_Z.moveTo(homeZ);
    homeY ++;
    homeZ --;
    Step_Y.run();
    Step_Z.run();
  }

  while(digitalRead(end_Y)==1)
  {
    Step_Y.moveTo(homeY);
    homeY ++;
    Step_Y.run();
  }
  Step_Y.setCurrentPosition(0);

  while(digitalRead(end_Z)==1)
  {
    Step_Z.moveTo(homeZ);
    homeZ --;
    Step_Z.run();        
  }
  Step_Z.setCurrentPosition(0);
    
  homeY = 0; homeZ = 0;  
}

void home_X()
{
  int homeX = 0;
  Step_X.setMaxSpeed(100);
  Step_X.setAcceleration(100);  
  Step_X.enableOutputs();

  while(digitalRead(end_X)==1)
  {
    Step_X.moveTo(homeX);
    homeX ++;
    Step_X.run();
  }
  Step_X.setCurrentPosition(0);

  homeX = 0;
}

void home()
{
  home_YZ();
  home_X();
}

void setup() 
{
  Serial.begin(115200);

  servo_kep.attach(4);

  pinMode(bt_X, INPUT);
  pinMode(bt_Y, INPUT); 
  pinMode(bt_Z, INPUT);
  pinMode(nutnhan, INPUT_PULLUP);

  pinMode(end_X, INPUT_PULLUP);
  pinMode(end_Y, INPUT_PULLUP);
  pinMode(end_Z, INPUT_PULLUP);

  Step_X.setEnablePin(38);
  Step_X.setPinsInverted(false, false, true);
  Step_Y.setEnablePin(56);
  Step_Y.setPinsInverted(false, false, true);
  Step_Z.setEnablePin(62);
  Step_Z.setPinsInverted(false, false, true);    

  home();  

  gt_bt_X = analogRead(bt_X);
  gt_bt_Y = analogRead(bt_Y); 
  gt_bt_Z = analogRead(bt_Z);    
  buoc_X = map(gt_bt_X, 0, 1023, 0, -500);
  buoc_Y = map(gt_bt_Y, 0, 1023, 0, -300);
  buoc_Z = map(gt_bt_Z, 0, 1023, 0, 250);

  Step_X.moveTo(buoc_X);
  Step_Y.moveTo(buoc_Y);
  Step_Z.moveTo(buoc_Z);   

  if (analogRead(bt_X)!=0 or analogRead(bt_Y)!=0 or analogRead(bt_Z)!=0)
  {
    while(Step_X.distanceToGo()!=0 or Step_Y.distanceToGo() != 0 or Step_Z.distanceToGo() != 0)
    {
      Step_X.run();
      Step_Y.run();
      Step_Z.run();      
    }         
  }

  Step_X.setMaxSpeed(1000);
  Step_X.setAcceleration(1000); 
  Step_Y.setMaxSpeed(1000);
  Step_Y.setAcceleration(1000); 
  Step_Z.setMaxSpeed(1000);
  Step_Z.setAcceleration(1000);  

}

void loop() 
{
  gt_bt_X = analogRead(bt_X);
  gt_bt_Y = analogRead(bt_Y); 
  gt_bt_Z = analogRead(bt_Z);
  
  gt_nutnhan = digitalRead(nutnhan);
  if(gt_nutnhan != macdinh) //KEP
  {
    if(gt_nutnhan == 0) dem++;
    macdinh = gt_nutnhan;
  }
  if(dem%2 == 0)  servo_kep.write(180);
  else servo_kep.write(0);

  if(((gt_bt_Y > bandau_Y+3) or (gt_bt_Y < bandau_Y-3)) or 
      ((gt_bt_Z > bandau_Z+3) or (gt_bt_Z < bandau_Z-3)) or 
      ((gt_bt_X > bandau_X+3) or (gt_bt_X < bandau_X-3)))
  {
    Step_X.enableOutputs();
    Step_Y.enableOutputs();
    Step_Z.enableOutputs();

    buoc_X = map(gt_bt_X, 0, 1023, 0, -500);
    buoc_Y = map(gt_bt_Y, 0, 1023, 0, -300);
    buoc_Z = map(gt_bt_Z, 0, 1023, 0, 250);
    
    Step_X.moveTo(buoc_X);
    Step_Y.moveTo(buoc_Y);
    Step_Z.moveTo(buoc_Z);    
    while(Step_X.distanceToGo() != 0 or Step_Y.distanceToGo() != 0 or Step_Z.distanceToGo() != 0)
    {
      Step_X.run();
      Step_Y.run();
      Step_Z.run();
    }

    bandau_X = gt_bt_X;
    bandau_Y = gt_bt_Y;
    bandau_Z = gt_bt_Z;
  }

  Serial.print("BT X: "); Serial.print(gt_bt_X);
  Serial.print(" - BT Y: "); Serial.print(gt_bt_Y);
  Serial.print(" - BT Z: "); Serial.print(gt_bt_Z);
  Serial.print(" - Button: "); Serial.print(gt_nutnhan);
  Serial.print(" | DEM: "); Serial.print(dem);
  Serial.print(" - BUOC X: "); Serial.print(buoc_X);
  Serial.print(" - BUOC Y: "); Serial.print(buoc_Y);
  Serial.print(" - BUOC Z: "); Serial.print(buoc_Z);

  Serial.println();
}