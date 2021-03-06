#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "consts.c"
#include "states.h"

/**************************************************************
   Constants defined to replace parameters for testing purposes
 **************************************************************/

#define IMU_speed
#define motor_current_thresh_peak 250 //amps
#define motor_current_thresh_cont 200//Amps
#define motor_voltage_thresh 50.4  //V
#define battery_temp_thresh 60  //degress C
#define motor_temp_thresh 180 //degrees C
#define battery_pressure_thresh 10 //psi

#define IMU_speed
#define motor_current_peak 250 //amps
#define motor_current_thresh_cont 200//Amps
#define motor_voltage_thresh 50.4  //V
#define battery_temp_thresh 60  //degress C
#define motor_temp_thresh 180 //degrees C
#define battery_pressure_thresh 10 //psi


struct StateDetails
{
  const bool brake;
  const bool motor;
  const bool led_indicator;
};


/****     PIN LAYOUT      ****/
int led_0 = 50;
int led_1 = 51;
int led_2 = 52;

int motor_pin = 3;
int brake_pin = 4;

int RTL_switch = 10;
int power_led = 11;
int network_led = 12;
int safety_led = 13;

int input = 0;
int pwm = 5;

float pressure;
unsigned long pulseWidth;

bool initialBrake;

// Create current/previous states allows for one shot programming later in the code
state currentState, prevState;
// Time State initiated, used for time based thresholding
unsigned long save;
unsigned long *TSI = &save;
// the time that the loop initiated
unsigned long startTime;


/**************************************************************
   MAIN CODE BLOCK
 **************************************************************/


void setup(void)
{
  // initialize power first
  pinMode(power_led, OUTPUT);
  digitalWrite(power_led, HIGH);
  
  pinMode(RTL_switch, INPUT);
  //  pinMode(RTL_switch_reset, OUTPUT);

  pinMode(led_0, OUTPUT);
  pinMode(led_1, OUTPUT);
  pinMode(led_2, OUTPUT);
  pinMode(motor_pin, OUTPUT);
  pinMode(brake_pin, OUTPUT);

  pinMode(network_led, OUTPUT);

  pinMode(RTL_switch, INPUT);

  pinMode(pwm, OUTPUT);

  //init all lights to high
  digitalWrite(led_0, HIGH);
  digitalWrite(led_1, HIGH);
  digitalWrite(led_2, HIGH);
  digitalWrite(network_led, LOW);

  // For STA switch case
  digitalWrite(motor_pin,LOW);
  digitalWrite(brake_pin,LOW);

  // initiate default state on start up
  //  receivedState = state::STA;
  currentState = state::STA;
  prevState = state::STATES;

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  // TODO reset fault switch
  Serial3.begin(115200);
  while (!Serial3) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  //

  delay(1000);
  //  dispState(state::LAUNCH);
  for (int i = 0; i <= state::STATES; i++) {
    dispState(i);
    delay(500);
  }
  digitalWrite(network_led, HIGH);
}



void motorStartUp() {
  if (Serial.available()) {
    Serial.print (1);
    input = Serial.parseInt();
    Serial.read();
  }
  if ((input > 100) && (input < 255)) {
    analogWrite (pwm, input);

  }
  else {
    analogWrite (pwm, 0);
  }
  
}


void loop(void) {
  // Update state based on network info
  state recvState = getState(Serial3);
  currentState = checkState(recvState, currentState, TSI, motor_pin, brake_pin);
  dispState(currentState);
  
}
