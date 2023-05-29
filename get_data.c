#include <Stepper.h>
#include <Servo.h>

const int trigPin = 6;
const int echoPin = 7;
const int stepsPerRevolution = 2048;
bool isReversed = false;
int Distance = 0;
int Angle = 0;
int Elevation = 0;

Stepper myStepper(stepsPerRevolution, 8, 10, 9, 11);
Servo myServo;

void setup()
{
    Serial.begin(9600);
    myServo.attach(13);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);

    myStepper.setSpeed(10);
}

void loop()
{
    delay(5000);
    Serial.print("start\n \n");
    for (int i = 0; i < 18; i++)
    {
        // 28 is the approximative steps for a 5 degree move
        myStepper.step(28);
        delay(500);
        Elevation = 5 * i;
        servoSweep();
        getDistance();
    }
    Serial.print("\n The End \n \n \n");
    myStepper.step(-504);
    Elevation = 0;
}

int getDistance()
{
    // Generate a pulse by switching from low to high for 10 microseconds
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // The ultrasonic sensor echo pin remains high until the sound echo is received
    // Get the duration of the high pulse on the echo pin
    int duration = pulseIn(echoPin, HIGH);

    // Speed of sound considered to be 340m/s (divided by 2 because it is a round trip)
    return duration * 0.034 / 2;
}

void printCoordinates()
{
    Serial.print(Distance);
    Serial.print(",");
    Serial.print(Angle);
    Serial.print(",");
    Serial.println(Elevation);
}

void servoSweep()
{
    // The servo sweeps back and forth between 0 and 90 degrees
    // isReversed determines the rotation direction
    if (isReversed == false)
    {
        for (Angle = 0; Angle <= 90; Angle += 5)
        {
            myServo.write(Angle);
            // Wait the servo to get in position
            delay(200);
            Distance = getDistance();
            printCoordinates();
            
        }
        isReversed = true;
    }
    else
    {
        for (Angle = 90; Angle >= 0; Angle -= 5)
        {
            myServo.write(Angle);
            delay(250);
            Distance = getDistance();
            printCoordinates();
        }
        isReversed = false;
    }
}