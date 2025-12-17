#include <Servo.h>

// ================= Servo objects =================
Servo myservo1;  // base
Servo myservo2;  // upper arm
Servo myservo3;  // lower arm
Servo myservo4;  // claw

// ================= Initial angles =================
int pos1 = 80;
int pos2 = 60;
int pos3 = 130;
int pos4 = 10;

// ================= Joystick pins =================
const int right_X = A2;
const int right_Y = A5;
const int right_key = 7;

const int left_X = A3;
const int left_Y = A4;
const int left_key = 8;

// ================= Joystick values =================
int x1, y1, z1;
int x2, y2, z2;

// ================= Setup =================
void setup()
{
  myservo1.attach(A1);
  myservo2.attach(A0);
  myservo3.attach(6);
  myservo4.attach(9);

  myservo1.write(pos1);
  myservo2.write(pos2);
  myservo3.write(pos3);
  myservo4.write(pos4);
  delay(1500);

  pinMode(right_key, INPUT_PULLUP);
  pinMode(left_key, INPUT_PULLUP);

  Serial.begin(9600);
}

// ================= Main loop =================
void loop()
{
  x1 = analogRead(right_X);
  y1 = analogRead(right_Y);
  z1 = digitalRead(right_key);

  x2 = analogRead(left_X);
  y2 = analogRead(left_Y);
  z2 = digitalRead(left_key);

  zhuazi();
  zhuandong();
  xiaobi();
  dabi();
}

// ================= Claw =================
void zhuazi()
{
  if (x2 < 450)
  {
    pos4 -= 2;
    pos4 = constrain(pos4, 2, 108);
    myservo4.write(pos4);
    delay(5);
  }
  else if (x2 > 570)
  {
    pos4 += 4;
    pos4 = constrain(pos4, 2, 108);
    myservo4.write(pos4);
    delay(5);
  }
}

// ================= Base rotate =================
void zhuandong()
{
  if (x1 < 450)
  {
    pos1--;
    pos1 = constrain(pos1, 1, 180);
    myservo1.write(pos1);
    delay(5);
  }
  else if (x1 > 570)
  {
    pos1++;
    pos1 = constrain(pos1, 1, 180);
    myservo1.write(pos1);
    delay(5);
  }
}

// ================= Upper arm =================
void xiaobi()
{
  if (y1 > 570)
  {
    pos2--;
    pos2 = constrain(pos2, 0, 180);
    myservo2.write(pos2);
    delay(5);
  }
  else if (y1 < 450)
  {
    pos2++;
    pos2 = constrain(pos2, 0, 180);
    myservo2.write(pos2);
    delay(5);
  }
}

// ================= Lower arm =================
void dabi()
{
  if (y2 < 450)
  {
    pos3++;
    pos3 = constrain(pos3, 35, 180);
    myservo3.write(pos3);
    delay(5);
  }
  else if (y2 > 570)
  {
    pos3--;
    pos3 = constrain(pos3, 35, 180);
    myservo3.write(pos3);
    delay(5);
  }
}
