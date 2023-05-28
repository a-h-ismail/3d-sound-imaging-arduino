#include <Stepper.h>
#include <Servo.h>

const int trigPin = 9;
const int echoPin = 10;
const int stepsPerRevolution = 2048;
bool isReversed = false;
int Distance = 0;
int Angle = 0;
int Elevation = 0;

Stepper myStepper(stepsPerRevolution, 8, 7, 6, 5);
Servo myServo;

void setup()
{
    Serial.begin(9600);
    myServo.attach(13);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
}

void loop()
{
    for (int i = 0; i < 12; i++)
    {
        myStepper.step(28);
        delayMicroseconds(250000);
        Elevation = i * 5;
        servoSweep();
        Serial.print(i);
    }
    myStepper.step(-12);
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
    Serial.print("Distance: ");
    Serial.print(Distance);
    Serial.print(", Angle: ");
    Serial.print(Angle);
    Serial.print(", Elevation: ");
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
            Distance = getDistance();
            printCoordinates();
            delay(500);
        }
        isReversed = true;
    }
    else
    {
        for (Angle = 90; Angle >= 0; Angle -= 5)
        {
            myServo.write(Angle);
            Distance = getDistance();
            printCoordinates();
            delay(500);
        }
        isReversed = false;
    }
}