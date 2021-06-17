#include <math.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define SERVOMIN 150  // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX 900  // This is the 'maximum' pulse length count (out of 4096)
#define USMIN 600     // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX 2400    // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

const int thighLength = 80;
const int shinLength = 80;

int servoAngles[] = {150, 150, 180, 0, 30, 45, 0, 0, 40, 160, 170, 0, 140, 30, 45, 0};
int microAngles[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// int lfh; // Left Front hip Motor
// int lfs; // Left Front shoulder Motor
// int lfk; // Left Front knee Motor
// int rfh; // right Front hip Motor
// int rfs; // right Front shoulder Motor
// int rfk; // right Front knee Motor
// int lbh; // Left back hip Motor
// int lbs; // Left back shoulder Motor
// int lbk; // Left back knee Motor
// int rbh; // Right back hip Motor
// int rbs; // Right back shoulder Motor
// int rbk; // Right back knee Motor

String data;
char d1;
String d2;
float heightX;
float heightZ;
int transX;
int transZ;
int input;

void stand()
{
  pwm.writeMicroseconds(0, 2050);
  pwm.writeMicroseconds(1, 2000);
  pwm.writeMicroseconds(2, 1200);

  pwm.writeMicroseconds(4, 900);
  pwm.writeMicroseconds(5, 1200);
  pwm.writeMicroseconds(6, 1600);

  pwm.writeMicroseconds(8, 1000);
  pwm.writeMicroseconds(9, 2000);
  pwm.writeMicroseconds(10, 1200);

  pwm.writeMicroseconds(12, 2050);
  pwm.writeMicroseconds(13, 1200);
  pwm.writeMicroseconds(14, 1600);
}

// int degreeToMicroSec(int degree)
// {
//   int microSec = map(degree, 0, 180, 500, 2500);
//   return microSec;
// }

int microSecondConverter()
{
  for (int i = 0; i <= 15; ++i)
  {
    microAngles[i] = map(servoAngles[i], 0, 180, 500, 2500);
  }
}

void kinematics(int leg, float x, float z)
{
  // Calculating joint angles for translation in x - direction

  float shoulderAngle2 = atan(x / z);
  float shoulderAngle2Degrees = shoulderAngle2 * (180 / PI); // converting the calculated angle to degrees from radians

  float z1 = z / cos(shoulderAngle2);

  // Calculating joint angles for translation in z - direction (Radians)

  float shoulderNum = sq(thighLength) + sq(z1) - sq(shinLength);
  float shoulderDen = 2 * thighLength * z1;
  float shoulderAngle = acos(shoulderNum / shoulderDen);
  float kneeAngle = PI - (2 * shoulderAngle);

  // Converting the calculated joint angles to degrees

  float shoulderAngle1Degrees = shoulderAngle * (180 / PI);
  float kneeAngleDegrees = 45 + kneeAngle * (180 / PI);

  // Storing the joint angles for each leg in the array servoAngles[]

  switch (leg)
  {
    case 1:
      servoAngles[1] = 180 - shoulderAngle1Degrees + shoulderAngle2Degrees;
      servoAngles[2] = kneeAngleDegrees;
      microSecondConverter();
      pwm.writeMicroseconds(1, microAngles[1]);
      pwm.writeMicroseconds(2, microAngles[2]);
      break;

    case 2:
      servoAngles[5] = shoulderAngle1Degrees - shoulderAngle2Degrees;
      servoAngles[6] = 180 - kneeAngleDegrees;
      microSecondConverter();
      pwm.writeMicroseconds(5, microAngles[5]);
      pwm.writeMicroseconds(6, microAngles[6]);
      break;

    case 3:
      servoAngles[9] = 180 + 15 - shoulderAngle1Degrees + shoulderAngle2Degrees;
      servoAngles[10] = kneeAngleDegrees;
      microSecondConverter();
      pwm.writeMicroseconds(9, microAngles[9]);
      pwm.writeMicroseconds(10, microAngles[10]);
      break;

    case 4:
      servoAngles[13] = shoulderAngle1Degrees - shoulderAngle2Degrees;
      servoAngles[14] = 180 - kneeAngleDegrees;
      microSecondConverter();
      pwm.writeMicroseconds(13, microAngles[13]);
      pwm.writeMicroseconds(14, microAngles[14]);
      break;
  }
}

void setup()
{
  Serial.begin(9600);
  pwm.begin();
  pwm.setPWMFreq(60);
  pinMode(13, OUTPUT);
  Serial.println("----------------------------------------------- INITIALIZING -----------------------------------------------");
  stand();
}

void move_forward()
{
  for (int x = 30; x >= -30; x -= 5)
  {
    int radius = 30;
    int center = 120;
    int y = sqrt(sq(radius) - sq(x)) + center;

    kinematics(1, x, y);
    kinematics(4, x, y);
    kinematics(2, -x, center);
    kinematics(3, -x, center);
    delay(0.01);
  }

  for (int x = 30; x >= -30; x -= 5)
  {
    int radius = 30;
    int center = 120;
    int y = sqrt(sq(radius) - sq(x)) + center;

    kinematics(2, x, y);
    kinematics(3, x, y);
    kinematics(1, -x, center);
    kinematics(4, -x, center);
    delay(0.01);
  }
}

void move_backward()
{
  for (int x = -30; x <= 30; x += 5)
  {
    int radius = 30;
    int center = 120;
    int y = sqrt(sq(radius) - sq(x)) + center;

    kinematics(1, x, y);
    kinematics(4, x, y);
    kinematics(2, -x, center);
    kinematics(3, -x, center);
    delay(1);
  }

  for (int x = -30; x <= 30; x += 5)
  {
    int radius = 30;
    int center = 120;
    int y = sqrt(sq(radius) - sq(x)) + center;

    kinematics(2, x, y);
    kinematics(3, x, y);
    kinematics(1, -x, center);
    kinematics(4, -x, center);
    delay(1);
  }
}

void startRobot()
{

  if (Serial.available())
  {
    data = Serial.readStringUntil('|');
    d1 = data.charAt(0);

    if (d1 == 'f') {
      move_forward();
      Serial.println("Walking Forward");
    }

    else if (d1 == 'r') {
      move_backward();
      Serial.println("Walking Backward");
    }

    else if ( d1 == 'x')
    {
      d2 = data.substring(1);
      heightX = d2.toFloat();
      int newX = map(heightX * 100, -100, 100, 30, -30);
      Serial.println(newX);

      if (newX != 0)
      {
        transX = newX;
        for (int i = 0; i < 16; ++i)
        {
          kinematics(i, transX, transZ);
        }
      }
    }

    else if ( d1 == 'z')
    {
      d2 = data.substring(1);
      heightZ = d2.toFloat();
      int newZ = map(heightZ * 100, -100, 100, 120, 150);
      Serial.println(newZ);

      if (newZ != 0)
      {
        transZ = newZ;
        for (int i = 0; i < 16; ++i)
        {
          kinematics(i, transX, transZ);
        }
      }
    }
  }
}

void loop()
{
  startRobot();
}
