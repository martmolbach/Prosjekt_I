/////////////////////////////////////////////////////////////
////////////////// Autonom kjøremønster /////////////////////
/////////////////////////////////////////////////////////////

#include <Zumo32U4.h>
#include <Wire.h>


Zumo32U4ButtonA buttonA;

Zumo32U4Encoders encoder;
Zumo32U4Motors motors;
Zumo32U4OLED display;
Zumo32U4IMU imu;

float gyroOffsetZ;
float angle = 0;
const float countPerRotation = 75.81 * 12;
const float wheelDiameter = 3.75;
unsigned long previousMillis = 0; 
unsigned long previousCurrentTime = 0;
const int stopDuration = 2000;


enum State{
    moveForward,
    moveForward2,
    stopForward,
    turnRight,
    stopTurn, 
    turnAround,
    stop,
};
State currentState = moveForward;


void firkantmodus(){
 
    unsigned long currentTime = millis();
    float targetDistance = 0.4;

    switch (currentState)
    {
        case moveForward:
            motors.setSpeeds(200,200);
            //Serial.println("Frem");
            if(countDistance(targetDistance)){
            currentState = stopForward;
            }
            break;

        case stopForward:
            angle = 0; 
            motors.setSpeeds(0,0);
            //Serial.println("Stop1");
            if(currentTime-previousMillis>=stopDuration){
                previousMillis = currentTime;
                currentState = turnRight; 
            }
            break;

        case turnRight:
            motors.setSpeeds(-100,100);
            //Serial.println("Snu");
            if(angle >= 90){
                currentState = stopTurn;
                angle = 0;
            }
            break;

        case stopTurn:
            motors.setSpeeds(0,0);
            //Serial.println("Stop2");
            if(currentTime-previousMillis>=stopDuration){
                previousMillis = currentTime;
                currentState = moveForward; 
            }
            break;

        default:
            break;
    }
}

void sirkelModus(){
    motors.setSpeeds(100,200);

    if(angle >= 360)
    {
        angle = 0;
        motors.setSpeeds(0,0);
        delay(3000);

    }
}


bool countDistance(float targetDistance ) 
{
    static float totalDistance = 0;

    int leftEncoder = encoder.getCountsLeft();
    int rightEncoder = encoder.getCountsRight();
    float encoderCount = (abs(leftEncoder) + abs(rightEncoder)) / 2.0;
    float rotationCounter = encoderCount / countPerRotation;
    float currentDistance = rotationCounter * (PI * wheelDiameter);

    totalDistance += currentDistance;
    Serial.println(totalDistance);

    if (totalDistance >= targetDistance ) {
        totalDistance = 0;
        return true;  
    }

    return false;  
}
   

void updateAngleGyro()
{
    // Figure out how much time has passed since the last update.
    static uint16_t lastUpdate = 0;
    uint16_t m = micros();
    uint16_t dt = m - lastUpdate;
    lastUpdate = m;

    imu.readGyro();

    // Calculate how much the angle has changed, in degrees, and
    // add it to our estimation of the current angle.  The gyro's
    // sensitivity is 0.07 dps per digit.
    angle += ((float)imu.g.z - gyroOffsetZ) * 70 * dt / 1000000000;
}

void printAngles()
{
    display.gotoXY(0, 0);
    display.print(angle);
    display.print(F("  "));
}

void frem_og_tilbake(){
    float targetDistance = 50;
    float turnDistance = 30;
    unsigned long currentTime = millis();
    int distance = 1;

    switch (currentState) {

        case moveForward:
            Serial.println("framover1");
            motors.setSpeeds(200,200);
            if(countDistance(targetDistance)){
                currentState = stopForward;
            }
            break;

        case stopForward:
            Serial.println("stopp1");
            motors.setSpeeds(0,0);
            if(currentTime - previousCurrentTime >= stopDuration){
                previousCurrentTime = currentTime;
                currentState = turnAround;
                angle = 0;
            }
            break;

        case turnAround:
            Serial.println("snu1");
            motors.setSpeeds(-100,100); 
            if(angle >= 180){
                angle = 0;
                currentState = stopTurn;
            }
            break;

        case stopTurn:
            Serial.println("stop2");
            motors.setSpeeds(0,0);
            if(currentTime - previousCurrentTime >= stopDuration){
                previousCurrentTime = currentTime;
                currentState = moveForward2;
            }
            break;

        case moveForward2:
            Serial.println("framover2");
            motors.setSpeeds(200,200);
            if(countDistance(targetDistance))
            currentState = turnAround;
            break;
  
        case stop:
            // Serial.println("stop3");
            motors.setSpeeds(0,0);
            if(currentTime-previousCurrentTime >= stopDuration){
                previousCurrentTime = currentTime;
                currentState = turnAround;
            }
            break;

        default:
            break;
    }
}





void setup()
{
    encoder.init();
    display.init();
    display.clear();
    display.setLayout21x8();
    Serial.begin(115200);
    Wire.begin();

    imu.init();
    imu.enableDefault();
    imu.configureForBalancing();

    display.clear();
    display.print(F("Gyro cal"));
    ledYellow(1);
    delay(500);

    // Calibrate the gyro.
    for (uint16_t i = 0; i < 1024; i++) 
    {
        // Wait for new data to be available, then read it.
        while(!imu.gyroDataReady()) {}
        imu.readGyro();

        // Add the Y axis reading to the total.
        gyroOffsetZ += imu.g.z;
    }

    gyroOffsetZ /= 1024;

    display.clear();
    ledYellow(0);

    // Display the angle until the user presses A.
    while (!buttonA.getSingleDebouncedRelease())
    {
        // Update the angle using the gyro as often as possible.
        updateAngleGyro();

        // Every 20 ms (50 Hz), correct the angle using the
        // accelerometer and also print it.
        static uint8_t lastCorrectionTime = 0;
        uint8_t m = millis();
        if ((uint8_t)(m - lastCorrectionTime) >= 20)
        {
            lastCorrectionTime = m;   
            printAngles();
        }
    }
    delay(500);
    } 





void loop()
{
   //firkantmodus();
    //sirkelModus();
    frem_og_tilbake();
    static byte lastCorrectionTime = 0;
    byte m = millis();
    if ((byte)(m - lastCorrectionTime) >= 20)
    {
        lastCorrectionTime = m;
        printAngles();
    }
    int holderEncoderLeft = encoder.getCountsAndResetLeft();
    int holderEncoderRight = encoder.getCountsAndResetRight(); 
    updateAngleGyro();

    // Update the angle using the gyro as often as possible.

    // Every 20 ms (50 Hz), correct the angle using the
    // accelerometer, print it, and set the motor speeds.
}