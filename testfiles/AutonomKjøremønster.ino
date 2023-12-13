/////////////////////////////////////////////////////////////
////////////////// Autonom kjøremønster /////////////////////
/////////////////////////////////////////////////////////////

#include <Zumo32U4.h>
#include <Wire.h>


Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4Encoders encoder;
Zumo32U4Motors motors;
Zumo32U4OLED display;
Zumo32U4IMU imu;

float gyroOffsetZ;
float angle = 0;


const float countPerRotation = 75.81 * 12;
const float wheelDiameter = 3.75;
const float maxSpeed = 64;

unsigned long leftEncoderCount = 0;

float rotationCount = 0;
float previousRotationCount = 0;


unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;
unsigned long previousMillis4 = 0; 
unsigned long previousMillis5 = 0;
unsigned long previousMillis6 = 0;
unsigned long previousMillis7 = 0;

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


const int stopDuration = 2000;






int timePassedForValues = 0;
int timePassedForAverage = 0;

bool showingValues = true;
bool firkant = true;




float encoderCount = 0;
float totalDistance = 0;
float previousDistance = 0; 



float cmPerSecond = 0;
float rotationsPerSecond = 0;
float distanceTraveled = 0;
float highestSpeed = 0;
int percentageOfMaxSpeed = 0;
int secondsOverPercentage = 0;
float averageSpeed = 0;
unsigned long previousMillis3 = 0;
float distanceTraveledLast60Seconds = 0;

float calculateRotation()
{
    float rotationCounter = leftEncoderCount / countPerRotation;
    return rotationCounter;
}

float calculateRotationPerSecond()
{
    return rotationCount - previousRotationCount;
}

float calculateCmPerSecond()
{
    return 2 * PI * (wheelDiameter / 2) * rotationsPerSecond;
}

float calculateDistanceTraveled()
{
    return rotationCount * (PI * wheelDiameter) / 100;
}

float calculateAverageSpeed()
{
    return (distanceTraveledLast60Seconds * 100) / 60;
}

float updateHighestSpeed()
{
    if (highestSpeed < cmPerSecond)
    {
        return cmPerSecond;
    }
}

int calculatePercentageOfMaxSpeed()
{
    if (cmPerSecond > 0.7 * maxSpeed)
    {
        return 1;
    }
    return 0;
}



void firkantmodus(){
 
unsigned long currentMillis = millis();
float targetDistance = 40;
float turnDistance = 9;
switch (currentState)
{
case moveForward:
    motors.setSpeeds(200,200);
    Serial.println("Frem");
    if(countDistance(targetDistance)){
    currentState = stopForward;
    }
    break;

    case stopForward:
    angle = 0; 
    motors.setSpeeds(0,0);
    Serial.println("Stop1");
    if(currentMillis-previousMillis4>=stopDuration){
        previousMillis4 = currentMillis;
        currentState = turnRight; 
    }
    break;

    case turnRight:
    motors.setSpeeds(-100,100);
    Serial.println("Snu");
    if(angle >= 85){
    currentState = stopTurn;
        angle = 0;
    }
    break;

    case stopTurn:
    motors.setSpeeds(0,0);
    Serial.println("Stop2");
    if(currentMillis-previousMillis4>=stopDuration){
        previousMillis4 = currentMillis;
        currentState = moveForward; 
    }
    break;

default:
    break;
}
}



void sirkelModus(){
   // unsigned long currentMillis = millis();
    motors.setSpeeds(100,200);

    if(angle >= 360){
        angle = 0;
        motors.setSpeeds(0,0);
        delay(3000);

    }
}


bool countDistance(float targetDistance ) {
  static float totalDistance = 0;
  
  int leftEncoder = encoder.getCountsLeft();
  int rightEncoder = encoder.getCountsRight();
  float encoderCount = (abs(leftEncoder) + abs(rightEncoder)) / 2.0;
  float rotationCounter = encoderCount / countPerRotation;
  float currentDistance = rotationCounter * (PI * wheelDiameter);

  
  totalDistance += currentDistance;
    //Serial.println(totalDistance);
  
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

template <typename Ta, typename Tb> float vector_dot(const Zumo32U4IMU::vector<Ta> *a, const Zumo32U4IMU::vector<Tb> *b)
{
  return (a->x * b->x) + (a->y * b->y) + (a->z * b->z);
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
unsigned long currentMillis = millis();
int distance = 1;

switch (currentState)
{

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
        if(currentMillis-previousMillis6 >= stopDuration){
        previousMillis6 = currentMillis;
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
    if(currentMillis - previousMillis6 >= stopDuration){
        previousMillis6 = currentMillis;
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
        if(currentMillis-previousMillis6 >= stopDuration){
            previousMillis6 = currentMillis;
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

    while (!buttonB.getSingleDebouncedRelease())
    {
        sirkelModus();
    }

    while (!buttonC.getSingleDebouncedRelease()){

        firkantmodus();
    }

  delay(500);








} 



void displayValues()
{
    display.gotoXY(0, 0);
    display.print("Speed:");
    display.gotoXY(0, 1);
    display.print(cmPerSecond);
    display.print("cm/s");
    display.gotoXY(0, 3);
    display.print(angle);
    display.print(F("  "));
    display.gotoXY(0, 6);
    display.print("Distance:");
    display.gotoXY(0, 7);
    display.print(distanceTraveled); // 3.75 er dm til hjulet
    display.print("m");
    display.gotoXY(16, 0);
    display.print("Money");
    display.gotoXY(11, 1);
    display.print("Used:");
    display.gotoXY(17, 1);
    display.print("4kr");
    display.gotoXY(9, 2);
    display.print("Earned:");
    display.gotoXY(17, 2);
    display.print("2kr");
    display.gotoXY(10, 3);
    display.print("Total:");
    display.gotoXY(17, 3);
    display.print("7kr");
    display.gotoXY(16, 6);
    display.print("Time:");
    display.gotoXY(16, 7);
    display.print(timePassedForValues);
    display.print("sec");
}

void displayAverages()
{

    display.gotoXY(0, 0);
    display.print("Average speed:");
    display.gotoXY(0, 1);
    display.print(averageSpeed); // averageSpeed
    display.print("cm/s");

    display.gotoXY(0, 3);
    display.print("Highest speed:");
    display.gotoXY(0, 4);
    display.print(highestSpeed); // highestSpeed
    display.print("cm/s");

    display.gotoXY(0, 6);
    display.print("Over 70% max:");
    display.gotoXY(0, 7);
    display.print(secondsOverPercentage); // secondsOverPercentage
    display.print("sec");

    display.gotoXY(16, 6);
    display.print("Time: ");
    display.gotoXY(16, 7);
    display.print(timePassedForAverage);
    display.print("sec");
}

void loop()
{
    
    
   //firkantmodus();

    sirkelModus();
    //frem_og_tilbake();
    

    // Update the angle using the gyro as often as possible.
    updateAngleGyro();

  // Every 20 ms (50 Hz), correct the angle using the
  // accelerometer, print it, and set the motor speeds.
    static byte lastCorrectionTime = 0;
    byte m = millis();
     if ((byte)(m - lastCorrectionTime) >= 20)
     {
    lastCorrectionTime = m;
    
    printAngles();
    
     }
   
    
  
   

    int holderEncoderLeft = encoder.getCountsAndResetLeft();
    int holderEncoderRight = encoder.getCountsAndResetRight();

    leftEncoderCount += (abs(holderEncoderLeft) + abs(holderEncoderRight)) / 2;
    rotationCount = calculateRotation();

    //Serial.println(rotationCount);

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 100)
    {
        previousMillis = currentMillis;
        rotationsPerSecond = calculateRotationPerSecond() * 10;
        previousRotationCount = rotationCount;
        percentageOfMaxSpeed += calculatePercentageOfMaxSpeed();
    }

    if (currentMillis - previousMillis3 > 60000)
    {
        distanceTraveledLast60Seconds = distanceTraveled - distanceTraveledLast60Seconds;
        previousMillis3 = currentMillis;
        secondsOverPercentage = percentageOfMaxSpeed / 10;
        averageSpeed = calculateAverageSpeed();
    }

    cmPerSecond = calculateCmPerSecond();
    highestSpeed = updateHighestSpeed();
    distanceTraveled = calculateDistanceTraveled();

    unsigned long currentMillis2 = millis();

    timePassedForValues = (currentMillis2 - previousMillis2) / 1000;

    // Viser values i 60 sekunder
    if (showingValues == true)
    {
        //Serial.print("Viser vanlige verdier i 10 sek: ");
        //Serial.println(currentMillis2 - previousMillis2);
        displayValues();
    }

    // Clearer Display for å vise average, etter 60 sek
    if (showingValues == true && currentMillis2 - previousMillis2 >= 60000)
    {
        display.clear();
        showingValues = false;
        previousMillis2 = currentMillis2;
    }

    if (showingValues == false)
    {
        // Viser Average i 3 sekunder
        //Serial.print("Viser Average i neste 3 sek: ");
        displayAverages();
        //Serial.println(currentMillis2 - previousMillis2);
        timePassedForAverage = (currentMillis2 - previousMillis2) / 1000;

        // Clearer Display for neste iterasjon, etter 3 sek
        if (showingValues == false && currentMillis2 - previousMillis2 >= 3000)
        {
            display.clear();
            highestSpeed = 0;
            secondsOverPercentage = 0;
            averageSpeed = 0;
            showingValues = true;
            previousMillis2 = currentMillis2;
        }
    }
    
}