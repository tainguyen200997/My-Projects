#define btDelay int(300)
#define sensor A0
#define ENA 5
#define IN1  6
#define IN2  7
#define mod 13
#define up 11
#define dw 12
#define CLK A5
#define DIO A4
#define tt 5

#include <TM1637Display.h>
extern TM1637Display display(CLK,DIO);  //set up the 4-Digit Display.
#include <Arduino_FreeRTOS.h>

boolean tt1, cb;
volatile boolean mode;
volatile boolean tCheck = true;
volatile int t, cap, giay, pwmOutput;

void TaskProcess( void *pvParameters );
void readMode( void *pvParameters );
void TaskDisplay(void *pvParameters);

void setup()
{
//  Serial.begin(9600);
  giay = 50;
  cap = 3;
  tt1 = 1;
  xTaskCreate(
    TaskProcess
    ,  (const portCHAR *)"main"  // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  0  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

  xTaskCreate(
    readMode
    ,  (const portCHAR *) "readButton"
    ,  128  // Stack size
    ,  NULL
    ,  0  // Priority
    ,  NULL );
  xTaskCreate(
    increase
    ,  (const portCHAR *) "up"
    ,  128  // Stack size
    ,  NULL
    ,  0  // Priority
    ,  NULL );
  xTaskCreate(
    decrease
    ,  (const portCHAR *) "down"
    ,  128  // Stack size
    ,  NULL
    ,  0  // Priority
    ,  NULL );
  xTaskCreate(
    TaskDisplay
    ,  (const portCHAR *) "showDigit"
    ,  128  // Stack size
    ,  NULL
    ,  0  // Priority
    ,  NULL );

}
void loop() {
  //none
}

void TaskProcess( void *pvParameters)  // This is a Task.
{
  (void) pvParameters;
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  //  pinMode(12, OUTPUT);
  pinMode(sensor, INPUT);
  //  pinMode(tt, INPUT)
  for (;;) {
    pwmOutput = 55 + cap * 40;
    if (digitalRead(sensor) == 0)
    {
      motor_1_Dung();
      delay(giay * 100);
      motor_1_Chay();
      delay(2000 / cap);
    }
    motor_1_Chay();
  }
}
void readMode( void *pvParameters)  // This is a Task.
{
  pinMode(mod, INPUT_PULLUP);
  for (;;) {
    if (!digitalRead(mod)) {
      mode = !mode;
      delay(btDelay);
    }
  }
}
void increase(void *pvParameters)
{
  pinMode(up, INPUT_PULLUP);
  for (;;) {
    if (!digitalRead(up)) {
      if (mode == 0)
      {
        cap++;
        if (cap == 6) cap = 1;
      }
      else {
        giay = giay + 5;
        if (giay == 100) giay = 0;
      }
      delay(btDelay);
    }
  }
}
void decrease(void *pvParameters)
{
  pinMode(dw, INPUT_PULLUP);
  for (;;) {
    if (!digitalRead(dw)) {
      if (mode == 0)
      {
        cap--;
        if (cap < 1) cap = 5;
      }
      else if (mode == 1)
      {
        giay = giay - 5;
        if (giay == 0) giay = 95;
      }
      delay(btDelay);
    }
  }
}
void TaskDisplay(void *pvParameters) {
  display.setBrightness(0x0a);
  for (;;) {
    //    display.showNumberDecEx(mode * 1000 + cap * 100 + giay);
    display.showNumberHexEx((mode ? 0 : 61440) + cap * 256 + (giay / 10) * 16 + giay % 10);
  }
}

void motor_1_Dung()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite (ENA, 0);
}
void motor_1_Chay()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite (ENA, pwmOutput) ;
}
