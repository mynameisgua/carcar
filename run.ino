#include <AccelStepper.h>
#include <MultiStepper.h>
#include <math.h>
#include <string.h>
#include "dealt.h"
#include <SoftwareSerial.h>
#include <Servo.h>

Servo myServo;
SoftwareSerial BT(9, 10);

#define digitalpin1 12 //x
#define digitalpin2 13 //y
const int step_move = 200;
const int enable_pin = 8;
const int x_dirpin = 2;
const int x_steppin = 5;
const int y_dirpin = 3;
const int y_steppin = 6;
const int moveSteps = 500;
int sensorvalue1,sensorvalue2;
int set_zero = 1;

float gray_T[11] = {2 , 2.5 , 2.7 , 3 , 3.5 , 5 , 7 , 8 , 10 , 11, 12};
float gray_amplitude[11] = {1.2 , 1.2 , 1.2 , 1 , 1 , 1 , 0.9 ,0.8, 0.5, 0.2 ,0};


AccelStepper stepper_x_axis(1 , x_dirpin , x_steppin); // 使用 DRIVER 模式, 步進引腳為 2, 方向引腳為 5
AccelStepper stepper_y_axis(1 , y_dirpin , y_steppin);
MultiStepper steppers;

int stage = 0;
int ball_y = 0;
int width = 800;
int height = 1000;

void setup() { 


    Serial.begin(9600);
    BT.begin(9600);
    //pin setup--------------------------------------------------------------------------
    pinMode(enable_pin , OUTPUT);
    pinMode(x_steppin , OUTPUT);
    pinMode(x_dirpin , OUTPUT);
    pinMode(y_steppin, OUTPUT);
    pinMode(y_dirpin , OUTPUT);
    pinMode(digitalpin1,INPUT);
    pinMode(digitalpin2,INPUT);
    digitalWrite(enable_pin , LOW);
    myServo.attach(11);
    myServo.write(180);

    

    stepper_x_axis.setMaxSpeed(1000); // 設置最大速度
    stepper_x_axis.setAcceleration(20000); // 設置加速度
    stepper_x_axis.setCurrentPosition(0); // 設置初始位置為 0

    stepper_y_axis.setMaxSpeed(1000); // 設置最大速度
    stepper_y_axis.setAcceleration(20000); // 設置加速度
    stepper_y_axis.setCurrentPosition(0); // 設置初始位置為 0
    
    steppers.addStepper(stepper_x_axis);//加進去steppers
    steppers.addStepper(stepper_y_axis);


}

void loop() {
    // 檢查是否達到目標位置

    long positions[2];
    long positions_zero[2];
    long positions_last = {0};
    positions[0] = 0;
    positions[1] = 0;
    positions_zero[0] = 0;
    positions_zero[1] = 0;
    
    //--------dealt--------
    switch(stage){
      case 0:
        if(BT.available()){
          Serial.println("available");
          String val;
          val = BT.readString();
          Serial.println(val);
          steppers.moveTo(positions_zero);
          steppers.runSpeedToPosition();
          if(val == "start"){
            Serial.println("start");
            myServo.write(180);
            gueli(sensorvalue1, sensorvalue2, positions_zero);
            stage = 1;
            BT.print("command");
          }
        }
        break;
      case 1:
        if(BT.available()){
          Serial.println("BT.available");
          String s;
          s = BT.readString();
          Serial.println(s);
          int T_index = 0;//不能同時設
          int same_index = 0;
          int T_run = 0;
          int same_run = 0;
          int inverse = 0;
          int same[100] = {0};
          int T[100] = {0};
          all_array(same,T,s,same_index,T_index);
        
          int block_size_x = 10;
          int block_size_y = 6; 
          int delta_t = 0;
          
          int t = 0;

          // sin(positions,gray_T,same,same_index,block_size_x,block_size_y,inverse,t,ball_y,width,T);
          myServo.write(100);
          delay(1000);
          stepper_x_axis.setMaxSpeed(200); // 設置最大速度(不可超過1000)
          stepper_x_axis.setAcceleration(20000); // 設置加速度
          stepper_y_axis.setMaxSpeed(200); // 設置最大速度
          stepper_y_axis.setAcceleration(20000); // 設置加速度
          for(same_run = 0 ; same_run < same_index ; same_run++){
            Serial.print("same run : ");
            Serial.println(same_run);
            /*Serial.print("gray_T : ");
            Serial.println(gray_T[T[T_run]]);
            Serial.print("same[same_run] : ");
            Serial.println(same[same_run]);*/
            for(delta_t = 0 ; delta_t < Roundtoint(M_PI*gray_T[T[T_run]]*same[same_run]/6) ; delta_t++){
              positions[0] = t + delta_t;
              if(positions[0] >= width) break;
              if(inverse % 2 == 1){
                 positions[1] = ball_y + Roundtoint((block_size_y / 1.2)* gray_amplitude[T[T_run]] * sin(6*delta_t/gray_T[T[T_run]] + M_PI)); 
                 //Serial.println(int(block_size_y*sin(5*delta_t/gray_T[T[T_run]] + M_PI)));
              }
              else{
                positions[1] = ball_y + Roundtoint((block_size_y / 1.2)* gray_amplitude[T[T_run]] * sin(6*delta_t/gray_T[T[T_run]]));
                //Serial.println(int(block_size_y*sin(5*delta_t/gray_T[T[T_run]])));
              }
              steppers.moveTo(positions);
              steppers.runSpeedToPosition();
            }
            Serial.print("delta_t : ");
            Serial.println(delta_t);
            if(positions[0] >= width){
              break;
              Serial.print("positions : ");
              Serial.print(positions[0]);
            }
            t += Roundtoint(M_PI*gray_T[T[T_run]]*same[same_run] / 6);
            T_run++;
            if(same[same_run] % 2 ==1){
              inverse++;
            }
            Serial.println("outside delta");
          }

          Serial.print("sum of delta:");
          Serial.println(t);

          Serial.println("hello");
          myServo.write(180);
          delay(1000);
          stepper_x_axis.setMaxSpeed(1000); // 設置最大速度(不可超過1000)
          stepper_x_axis.setAcceleration(20000); // 設置加速度
          stepper_y_axis.setMaxSpeed(1000); // 設置最大速度
          stepper_y_axis.setAcceleration(20000); // 設置加速度
          ball_y += block_size_y;
          positions_zero[1] = ball_y;
          positions_zero[0] = 0;
          steppers.moveTo(positions_zero);
          steppers.runSpeedToPosition();
          BT.print("command");
        }
        
        if(ball_y >= height){
          stage = 2;
          Serial.println("stage is 2");
        }
        break;

      case 2:
        set_zero = 1;
        while(set_zero){
          myServo.write(180);
          delay(1000);
          positions_zero[1] = 0;
          steppers.moveTo(positions_zero);
          steppers.runSpeedToPosition();
          if(stepper_x_axis.currentPosition()==0 && stepper_y_axis.currentPosition()==0){
            set_zero = 0;
          }
        }
        
        break;
    }
}



void gueli(int sensorvalue1,int sensorvalue2 , long* positions_zero){
  stepper_x_axis.setSpeed(1000);
  stepper_y_axis.setSpeed(1000);
  stepper_x_axis.moveTo(-2000);
  stepper_y_axis.moveTo(-2000);
  sensorvalue1 = digitalRead(digitalpin1);
  sensorvalue2 = digitalRead(digitalpin2);
  int state = 0;
  int has_set_state_to_1 = 0;
  while(set_zero){
    sensorvalue1 = digitalRead(digitalpin1);
    sensorvalue2 = digitalRead(digitalpin2);
    switch(state){
      case 0 :
        stepper_x_axis.run();
        break;
      case 1 :
        stepper_y_axis.run();
        break;
      case 2:
        stepper_x_axis.stop();
        stepper_y_axis.stop();
        stepper_x_axis.setCurrentPosition(0);
        stepper_y_axis.setCurrentPosition(0);
        positions_zero[0] -= 130;
        positions_zero[1] -= 20;
        steppers.moveTo(positions_zero);
        steppers.runSpeedToPosition();
        stepper_x_axis.setCurrentPosition(0);
        stepper_y_axis.setCurrentPosition(0);
        set_zero = 0;
        Serial.println("set_zero");
        break;
    }
    if(sensorvalue1 == 0 && sensorvalue2 == 1 && has_set_state_to_1 == 0){
        state = 1;
        has_set_state_to_1 = 1;
        Serial.println("set state to 1");
        continue;
    }
    if(sensorvalue1 == 0 && sensorvalue2 == 0){
      state = 2;
      Serial.println("set state to 2");
    }
  }
}

void sin(long* positions,int* gray_T,int* same,int& same_index,int block_size_x,int block_size_y,int& inverse,int& t,int& ball_y,int width,int* T){
  int same_run = 0;
  int T_run = 0;
  int delta_t = 0;
  for(same_run = 0;same_run < same_index;same_run++){
    for(delta_t = 0;delta_t < int(5*M_PI*gray_T[T[T_run]]*same[same_run]);delta_t++){
      positions[0] = t + delta_t;
      if(positions[0] >= width){
        break;
      }
      if(inverse % 2 == 1){
        positions[1] = ball_y + int(block_size_y/2*sin(5*delta_t/gray_T[T[T_run]] + M_PI));
      }
      else{
        positions[1] = ball_y + int(block_size_y/2*sin(5*delta_t/gray_T[T[T_run]]));
      }
      steppers.moveTo(positions);
      steppers.runSpeedToPosition();
    }
    if(positions[0] >= width){
      break;
    }
    t += int(5*M_PI*gray_T[T[T_run]]*same[same_run]);
    T_run++;
    if(same[same_run] % 2 ==1){
      inverse++;
    }

  }
  BT.print("command");
  ball_y += block_size_y;
}

int Roundtoint(float a){
  return int(a + 0.5);
}
