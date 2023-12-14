
#include <Zumo32U4.h>
#include <Wire.h>
#include <EEPROM.h>


Zumo32U4Encoders encoder;
Zumo32U4Buzzer buzzer;
Zumo32U4LineSensors lineSensors;
Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4OLED display;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4IMU imu;


const float countPerRotation = 75.81 * 12;

unsigned long leftEncoderCount = 0;
float rotationCount = 0;
float previousRotationCount = 0;

unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;

int timePassedForValues = 0;
int timePassedForAverage = 0;
int timePassedForBatteryStatus = 0;

bool initiateBatteryService = false;
bool initiateBatteryChange = false;

float distanceTraveled = 0;
float cmPerSecond = 0;
float rotationsPerSecond = 0;
float distanceToStation = 0;
float distanceBeforeLeftTurn = 0;
float distanceBeforeRightTurn = 0;
float distanceBeforeRightTurnCharging = 0;
float taxiPickupDistance = 0;
float highestSpeed = 0;

const int maxSpeed = 400;
const int maxSpeedCmPerSecond = 65;
const int calibrationTime = 3500;
const int NUM_SENSORS = 5;

const int shakeThreshold = 500; // Adjust threshold if needed

int16_t lastError = 0;

unsigned long previousTimeForSpeed = 0;
unsigned long previousTimeForSpeed2 = 0;

unsigned int lineSensorValues[NUM_SENSORS];


unsigned long previousNumber = 0;
unsigned long previousWaitTimeForNext = 0;
unsigned long lastHealthChange = 0;

unsigned long waitTimeForNext = 0;

unsigned long allowHealthChangeMillis = 0;
unsigned long healthChangeMillis = 0;
unsigned long previousBatteryAlert = 0;
unsigned long underTenPercentAlertGivenMillis = 0;
unsigned long underTenLightBlink = 0;
unsigned long previousBatteryStatusShown = 0;
unsigned long setupDoneTimer = 0;
unsigned long leftTurnDuration = 0;
unsigned long rightTurnDuration = 0;
unsigned long serviceDoneTimer = 0;
unsigned long rightTurnDuration2 = 0; // TESTER
unsigned long lastRightTurnDetected = 0; // TESTER
unsigned long lastRightTurnChargingDetected = 0; // TESTER
unsigned long taxiStart = 0;
unsigned long taxiStop = 0;

unsigned long timePassedAfterSetup = 0;

int batteryLevel = 0;
int batteryHealth = 0;
int money = 0;
int previousChange = 0;
int randomTaxiPercentage = 0;
int randomTaxiDistance = 0;
int secondsOverPercentage = 0;

const int batteryHealthAddress = 0;
const int batteryLevelAddress = 1;
const int moneyAddress = 2;


bool holdWhileTillShaken = false; // TESTER SHAKE
bool batteryLevelUnderFivePercentReductionGiven = false;

int totalCarShakes = 0; 
int totalMoneySpent = 0;
int totalMoneySpentLastTransaction = 0;


int randomNumber1 = 0;
int randomNumber2 = 0;
int randomNumber3 = 0;
int randomNumber4 = 0;



////// GYRO 
int forwardCounter = 0;
float gyroOffsetZ;
float angle = 0;
const float wheelDiameter = 3.75;
unsigned long previousCurrentTime2 = 0; 
unsigned long previousCurrentTime = 0;
const int stopDuration = 2000;
/// GYROS KEBAB


enum State{
    moveForward,
    moveForward2,
    stopForward,
    turnRight,
    stopTurn, 
    turnAround,
    stop,
    finish
};
State currentState = moveForward;

enum drivingState {
    LINEFOLLOWER_NORMAL,
    LINEFOLLOWER_CHARGING,
    LEFTTURN,
    RIGHTTURN,
    RIGHTTURN_CHARGING,
    CHARGING_MODE,
    HOLE_IN_ROAD,
    TAXIMODE_PICKUP,
    TAXIMODE,
    TAXIMODE_STOP
};
drivingState drivingStateVar = LINEFOLLOWER_NORMAL;


enum batteryState {
    INITIATE_BATTERY_CHANGE,
    INITIATE_BATTERY_SERVICE,
    NORMAL_BATTERY_STATE
};
batteryState batteryStateVar = NORMAL_BATTERY_STATE;

enum randomState {
    HEALTH_HAS_CHANGED,
    NORMAL_RANDOM_STATE
};
randomState randomStateVar = NORMAL_RANDOM_STATE;

enum underTenPercentAlert {
    UNDER_TEN_PERCENT,
    UNDER_TEN_PERCENT_GIVEN_MILLIS,
    UNDER_TEN_PERCENT_ALERT_GIVEN,
    NORMAL_UNDER_TEN_PERCENT_ALERT
};
underTenPercentAlert underTenPercentAlertVar = NORMAL_UNDER_TEN_PERCENT_ALERT;

enum valuesShown {
    SHOWING_VALUES,
    SHOWING_AVERAGES,
    SHOWING_BATTERY_STATUS,
    NORMAL_VALUES_SHOWN,
    SHOWING_AFTER_CHARGING,
    TAXI_MODE
};
valuesShown valuesShownVar = SHOWING_VALUES;

enum zeroPercentStage {
    ZERO_PERCENT_NORMAL,
    STUCK_IN_WHILE,
    FIVE_SHAKES_DETECTED,
    LEAVE_WHILE
};
zeroPercentStage zeroPercentStageVar = ZERO_PERCENT_NORMAL;


// SETUP //
void setup() {  // Millis() is counting during setup
    display.init();
    display.clear();
    display.print("Welcome");
    buzzer.play(">g32>>c32");
    delay(1000);
    display.clear();
    display.setLayout21x8();
    Wire.begin();
    imu.init();
    imu.enableDefault();   
    imu.configureForBalancing();
    encoder.init();
    waitForAorB();
    lineSensors.initFiveSensors();
    encoder.init();
    display.clear();
    unsigned long startTime = millis();
    display.gotoXY(4,3);
    display.print("Calibrating...");
    calibrateSensors(startTime);
    display.clear();
    display.gotoXY(5,3);
    display.print("Calibration");
    display.gotoXY(9,4);
    display.print("Done");
    randomSeed(millis());
    randomTaxiPercentage = random(30, 90);
    randomTaxiDistance = random(1, 3);
    delay(2000);
    display.clear();
    setupDoneTimer = millis();
}
// SETUP DONE //



float calculateRotation() {
    float rotationCounter = leftEncoderCount / countPerRotation;
    return rotationCounter;
}

float updateHighestSpeed() {
    if (cmPerSecond > highestSpeed) {
        return cmPerSecond;
    }
}

float calculateRotationPerSecond() {
    return rotationCount - previousRotationCount;
}

bool randomOccured(){
    randomNumber1 = random(0, 10000);
    randomNumber2 = random(0, 10000);
    randomNumber3 = random(0, 10000);
    randomNumber4 = random(0, 10000);
    return (randomNumber1 == randomNumber2 == randomNumber3 == randomNumber4);
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

void calibrateSensors(unsigned long startTime) {
    while(millis() - startTime < calibrationTime){
        lineSensors.calibrate();
        motors.setSpeeds(200,-200);
    }
    motors.setSpeeds(0,0);
}

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
            if(currentTime-previousCurrentTime2>=stopDuration){
                previousCurrentTime2 = currentTime;
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
            if(currentTime-previousCurrentTime2>=stopDuration){
                forwardCounter = forwardCounter + 1;
                previousCurrentTime2 = currentTime;
                currentState = moveForward; 
            }
            break;

        default:
            break;
    }
}

void sirkelModus(){
    motors.setSpeeds(100,200);

    // if(angle >= 360)
    // {
    //     angle = 0;
    //     motors.setSpeeds(0,0);
    //     delay(3000);

    // }
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
    display.gotoXY(0, 5);
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
            currentState = stop;
            break;
  
        case stop:
            // Serial.println("stop3");
            motors.setSpeeds(0,0);
            if(currentTime-previousCurrentTime >= stopDuration){
                previousCurrentTime = currentTime;
                currentState = finish;
                //currentState = turnAround;
            }
            break;

        case finish:
            break;

        default:
            break;
    }
}

void waitForAorB() {
    while(true){
        display.gotoXY(2,1);
        display.print("Press A for start");
        display.gotoXY(2,3);
        display.print("Press B for reset");
        display.gotoXY(2,5);
        display.print("Press C for patterns");
        if (buttonA.getSingleDebouncedRelease()){
            display.clear();
            break;
        }
        else if (buttonB.getSingleDebouncedRelease()){
            EEPROM.write(batteryHealthAddress, 100); // CHANGE THIS
            EEPROM.write(batteryLevelAddress, 100); // CHANGE THIS
            EEPROM.write(moneyAddress, 70);         // CHANGE THIS
            display.clear();
            break;
        }
        else if (buttonC.getSingleDebouncedRelease()){
            display.clear();
                display.clear();
                display.print(F("Gyroscope calibrating"));
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
                display.print("Press A to continue");
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
            while(true) { 
                display.gotoXY(2,0);
                display.print("Press A for line");
                display.gotoXY(2,2);
                display.print("Press B for square");
                display.gotoXY(2,4);
                display.print("Press C for circle");
                display.gotoXY(2,6);
                display.print("Press A+B for exit");
                if (buttonA.getSingleDebouncedRelease()){
                    display.clear();
                    while(true){
                        frem_og_tilbake();
                        static byte lastCorrectionTime = 0;
                        byte m = millis();
                        if ((byte)(m - lastCorrectionTime) >= 20)
                        {
                            lastCorrectionTime = m;
                            printAngles();
                        }
                        if(currentState == finish){
                            currentState = moveForward;
                            break;
                        }
                        int holderEncoderLeft = encoder.getCountsAndResetLeft();
                        int holderEncoderRight = encoder.getCountsAndResetRight(); 
                        updateAngleGyro();
                    }
                    display.clear();
                }
                else if (buttonB.getSingleDebouncedRelease()){
                    display.clear();
                    while(true){
                        firkantmodus();
                        updateAngleGyro();
                        static byte lastCorrectionTime = 0;
                        byte m = millis();
                        if ((byte)(m - lastCorrectionTime) >= 20)
                        {
                            lastCorrectionTime = m;
                            printAngles();
                        }
                        if (forwardCounter == 4){
                            break;
                        }
                        int holderEncoderLeft = encoder.getCountsAndResetLeft();
                        int holderEncoderRight = encoder.getCountsAndResetRight(); 
                    }
                }
                else if (buttonC.getSingleDebouncedRelease()){
                    display.clear();
                    while(true){
                        sirkelModus();
                        static byte lastCorrectionTime = 0;
                        byte m = millis();
                        if ((byte)(m - lastCorrectionTime) >= 20)
                        {
                            lastCorrectionTime = m;
                            printAngles();
                        }
                        if(angle >= 360)
                        {
                            angle = 0;
                            motors.setSpeeds(0,0);
                            break;
                            //delay(3000);

                        }
                        int holderEncoderLeft = encoder.getCountsAndResetLeft();
                        int holderEncoderRight = encoder.getCountsAndResetRight(); 
                        updateAngleGyro();
                    }
                    display.clear();
                }
                else if ((buttonA.isPressed()) && (buttonB.isPressed())){
                    display.clear();
                    delay(1000);
                    break;
                }
            }
        }
    }
}

void chargingOptions(){
    while(true){
        // Serial.print("Money: ");
        // Serial.print(money);
        // Serial.print("\t");
        // Serial.print("Health: ");
        // Serial.print(batteryHealth);
        // Serial.print("\t");
        // Serial.print("Level: ");
        // Serial.println(batteryLevel);
        display.gotoXY(2,1);
        display.print("Press A for charging");
        display.gotoXY(2,3);
        display.print("Press B for service");
        display.gotoXY(2,5);
        display.print("Press C for new battery");
        if (buttonA.getSingleDebouncedRelease()){
            display.clear();
            // 1kr = 1%
            while ((batteryLevel < 100) && money > 0){
                display.gotoXY(2,3);
                batteryLevel = batteryLevel + 1;

                totalMoneySpent = totalMoneySpent + 1;
                totalMoneySpentLastTransaction = totalMoneySpentLastTransaction + 1;
                money = money - 1;
            }

            if ((batteryLevel >= 100)){
                display.gotoXY(2,3);
                display.print("Battery full");
                delay(1000);
                display.clear();
            }
            else if ((money <= 0)){
                display.gotoXY(2,3);
                display.print("Not enough money");
                delay(1000);
                display.clear();
            }
            display.clear();
        }
        else if (buttonB.getSingleDebouncedRelease()){ 
            display.clear();
            if (batteryHealth < 20){
                display.gotoXY(2,1);
                display.print("Your battery is too damaged");
                display.gotoXY(2,3);
                display.print("Replacement needed");
                delay(1000);
                display.clear();
            }
            if ((batteryHealth >= 20) && (batteryHealth <= 30) && (money >= 20)){
                batteryHealth = batteryHealth + 70;
                money = money - 20;

                totalMoneySpent = totalMoneySpent + 20;
                totalMoneySpentLastTransaction = totalMoneySpentLastTransaction + 20;
                display.clear();
            }
            else {
                display.gotoXY(2,1);
                display.print("No service needed yet");
                delay(1000);
                display.clear();   
            }
            display.clear();
        }
        else if (buttonC.getSingleDebouncedRelease()){
            display.clear();
            if ((money >= 100) && (batteryHealth < 100)){
                batteryHealth = 100;
                batteryLevel = 100;
                money = money - 100;

                totalMoneySpent = totalMoneySpent + 100;
                totalMoneySpentLastTransaction = totalMoneySpentLastTransaction + 100;
                display.clear();
            }
            else if ((money < 100)){
                display.gotoXY(2,1);
                display.print("Not enough money");
                delay(1000);
                display.clear();
            }
            else if (batteryHealth >= 100){
                display.gotoXY(2,1);
                display.print("Battery is already new");
                delay(1000);
                display.clear();
            }
        }
        else if (buttonA.isPressed() && buttonB.isPressed()){
            display.clear();
            delay(1000);
            break;
        }
    }
}

void displayAverageDistance(){
    display.gotoXY(0, 0);
    display.print("Average speed:");
    display.gotoXY(0, 1);
    display.print("KUK"); // averageSpeed
    display.print("cm/s");

    display.gotoXY(0, 3);
    display.print("Highest speed:");
    display.gotoXY(0, 4);
    display.print(highestSpeed); // highestSpeed
    display.print("cm/s");

    display.gotoXY(0, 6);
    display.print("Over 70% max:");
    display.gotoXY(0, 7);
    display.print("SECOND OVER PERCENTAGE"); // secondsOverPercentage
    display.print("sec");

    display.gotoXY(16, 6);
    display.print("Time: ");
    display.gotoXY(16, 7);
    display.print(timePassedForValues);
    display.print("s");
}



void displayBatteryStatus(){
    display.gotoXY(0,0);
    display.print("Battery");
    display.gotoXY(0,2);
    display.print("Level:");
    display.print(batteryLevel);
    display.print("%");
    display.gotoXY(0,3);
    display.print("Health:");
    display.print(batteryHealth);
    display.print("%");
    display.gotoXY(16,6);
    display.print("Time:");
    display.gotoXY(16,7);
    //display.print(timePassedForBatteryStatus);
    display.print(timePassedForValues);
    display.print("s");
}

void displayAfterCharging(){
    display.gotoXY(0,0);
    display.print("Battery");
    display.gotoXY(0,2);
    display.print("Level:");
    display.print(batteryLevel);
    display.print("%");
    display.gotoXY(0,3);
    display.print("Health:");
    display.print(batteryHealth);
    display.print("%");
    display.gotoXY(5,3);
    display.print("Money spent:");
    display.gotoXY(5,4);
    display.print(totalMoneySpentLastTransaction);
    display.gotoXY(16,6);
    display.print("Time:");
    display.gotoXY(16,7);
    //display.print(timePassedForBatteryStatus);
    display.print(timePassedForValues);
    display.print("s");

}

void displayValues() {
    display.gotoXY(0,0);
    display.print("Speed:");
    display.gotoXY(0,1);
    display.print(cmPerSecond);
    display.gotoXY(5,1);
    display.print("cm/s");
    display.gotoXY(0, 6);
    display.print("Distance:");
    display.gotoXY(0,7);
    display.print(distanceTraveled); // 3.75 er dm til hjulet
    display.print("m");
    // display.gotoXY(13,0);
    // display.print("Battery");
    // display.gotoXY(10,1);
    // display.print("Health:");
    // display.print(batteryHealth);
    // display.gotoXY(11,2);
    // display.print("Level:");
    // display.print(batteryLevel);
    display.gotoXY(16,0);
    display.print("Money");
    display.gotoXY(13,1);
    display.print("Use:");
    display.gotoXY(17,1);
    display.print(totalMoneySpent);
    display.gotoXY(12,2);
    display.print("Earn:");
    display.gotoXY(17,2);
    display.print("10kr");
    display.gotoXY(13,3);
    display.print("Tot:");
    display.gotoXY(17,3);
    display.print(money);
    display.print("kr");
    display.gotoXY(16,6);
    display.print("Time:");
    display.gotoXY(16,7);
    display.print(timePassedForValues);
    display.print("s");
}

void lineFollower(){

        int16_t position = lineSensors.readLine(lineSensorValues);

        int error = position - 2000;

        int16_t speedDifference = error / 4 + 6 * (error - lastError);

        lastError = error;

        int16_t leftSpeed = maxSpeed + speedDifference;
        int16_t rightSpeed = maxSpeed - speedDifference;

        leftSpeed = constrain(leftSpeed, 0, maxSpeed);
        rightSpeed = constrain(rightSpeed, 0, maxSpeed);

        motors.setSpeeds(leftSpeed, rightSpeed);
        //motors.setSpeeds(350,350);
        //motors.setSpeeds(100,100);
    
}

void underTenPercentAlertFunc(){
    buzzer.play("!T104 L8 a.");
    display.gotoXY(2,0);
    display.print("Battery");
    display.gotoXY(3,1);
    display.print("Level");
    display.gotoXY(3,2);
    display.print("under");
    display.gotoXY(4,3);
    display.print("10%");
    ledRed(0);
    ledYellow(0);
    ledGreen(0);
    if(millis() - underTenLightBlink >= 500){
        ledRed(1);
        ledYellow(1);
        ledGreen(1);
        underTenLightBlink = millis();
    }
    if(millis() - underTenPercentAlertGivenMillis >= 2000){
        display.clear();
        display.setLayout21x8();
        valuesShownVar = SHOWING_VALUES;
        underTenPercentAlertVar = UNDER_TEN_PERCENT_ALERT_GIVEN;
    } 
}

void underFivePercentAlertFunc(){
    display.clear();
    buzzer.playNote(NOTE_A(4), 1000, 15);
    motors.setSpeeds(0,0);
    delay(1000);
    buzzer.stopPlaying();
    delay(500);
    buzzer.playNote(NOTE_A(4), 1000, 15);
    delay(1000);
    previousBatteryAlert = millis();
}

void randomEvent(){
    unsigned long start = millis();
    if (randomOccured() && randomStateVar == NORMAL_RANDOM_STATE){
        // Reduce battery 50%
        batteryHealth = batteryHealth / 2;
        //batteryLevel = batteryLevel / 2;
        EEPROM.update(batteryHealthAddress,batteryHealth);
        //EEPROM.update(batteryLevelAddress,batteryLevel);
        display.clear();
        lastHealthChange = start;
        start = start - previousNumber;
        previousNumber = start;
        randomStateVar = HEALTH_HAS_CHANGED;
    }

    if (millis() - lastHealthChange >= 240000 && randomStateVar == HEALTH_HAS_CHANGED){
        randomStateVar = NORMAL_RANDOM_STATE;
        previousWaitTimeForNext = waitTimeForNext;
    }
}

void calculateSpeed(){
    unsigned long currentMillis = millis();
    int holderEncoderLeft = encoder.getCountsAndResetLeft();
    int holderEncoderRight = encoder.getCountsAndResetRight();

    leftEncoderCount += (abs(holderEncoderLeft) + abs(holderEncoderRight)) / 2;
    rotationCount = calculateRotation();

    if (currentMillis - previousMillis >= 100) {
        previousMillis = currentMillis;
        rotationsPerSecond = calculateRotationPerSecond() * 10;
        previousRotationCount = rotationCount;
    }

    cmPerSecond = rotationsPerSecond * PI * 3.75; // RPS * omkrets
}

// LOOP //
void loop() {

    batteryHealth = EEPROM.read(batteryHealthAddress);
    batteryLevel = EEPROM.read(batteryLevelAddress);
    money = EEPROM.read(moneyAddress);


    distanceTraveled = rotationCount * 3.75* PI/100;
    highestSpeed = updateHighestSpeed();

    if (drivingStateVar == LINEFOLLOWER_NORMAL){
        Serial.print("LINEFOLLOWER_NORMAL");
        Serial.print("\t");
        Serial.println(batteryLevel);
        lineFollower();
    }


    if((lineSensorValues[0] > 600) && (lineSensorValues[1] > 600) /* && (lineSensorValues[2] > 600) && (lineSensorValues[3] > 200) && (lineSensorValues[4] < 200)*/ && ((drivingStateVar == LINEFOLLOWER_NORMAL) || (drivingStateVar == TAXI_MODE ))){ // EGNT BARE 0 og 4
        Serial.print("LINEFOLLOWER_NORMAL");
        Serial.print("\t");
        Serial.println(batteryLevel);
        drivingStateVar = LEFTTURN;
        distanceBeforeLeftTurn = distanceTraveled;
        leftTurnDuration = millis();
    }

    if (drivingStateVar == LEFTTURN){
        Serial.print("LEFTTURN");
        Serial.print("\t");
        Serial.println(batteryLevel);
        motors.setSpeeds(-100, 100);
    
        if (distanceTraveled - distanceBeforeLeftTurn >= 0.1){
            drivingStateVar = LINEFOLLOWER_NORMAL; 
        }

    }
    if((lineSensorValues[0] < 200) && (lineSensorValues[1] > 700) && (lineSensorValues[2] > 700) && (lineSensorValues[3] > 700) && (lineSensorValues[4] < 200) && ((drivingStateVar == LINEFOLLOWER_NORMAL) || (drivingStateVar == TAXI_MODE)) && ((batteryLevel > 10) /*|| (batteryHealth > 10)*/)){
        Serial.print("I DO NOT NEED CHARGING");
        Serial.print("\t");
        Serial.println(batteryLevel);
        rightTurnDuration = millis();
        distanceBeforeRightTurn = distanceTraveled;

        drivingStateVar = RIGHTTURN;
    }

    if((lineSensorValues[0] < 200) && (lineSensorValues[1] > 100) && (lineSensorValues[2] > 500) && (lineSensorValues[3] < 300) && (lineSensorValues[4] > 700) && (drivingStateVar == LINEFOLLOWER_NORMAL) && ((batteryLevel < 10) /*|| (batteryHealth < 10)*/)){
        Serial.print("I NEED CHARGING");
        Serial.print("\t");
        Serial.println(batteryLevel);
        distanceBeforeRightTurnCharging = distanceTraveled;

        drivingStateVar = RIGHTTURN_CHARGING;
    }

    if((lineSensorValues[0] < 200) && (lineSensorValues[1] > 100) && (lineSensorValues[2] > 500) && (lineSensorValues[3] < 300) && (lineSensorValues[4] > 700) && (drivingStateVar == LINEFOLLOWER_NORMAL) && ((batteryHealth < 30) /*|| (batteryHealth < 10)*/)){
        Serial.print("I NEED BATTERY SERVICE");
        Serial.print("\t");
        Serial.println(batteryLevel);
        distanceBeforeRightTurnCharging = distanceTraveled;

        drivingStateVar = RIGHTTURN_CHARGING;
    }

    if ((drivingStateVar == RIGHTTURN)){ // Change to  x > 10
        motors.setSpeeds(-100, 100);
        Serial.print("RIGHTTURN");
        Serial.print("\t");
        Serial.println(batteryLevel);
        if (distanceTraveled - distanceBeforeRightTurn >= 0.05){
            lastRightTurnDetected = millis();
            drivingStateVar = LINEFOLLOWER_NORMAL;
        }
    }

    if ((drivingStateVar == RIGHTTURN_CHARGING)){// Change to  x < 10
        motors.setSpeeds(100, -100);
        Serial.print("RIGHTTURN_CHARGING");
        Serial.print("\t");
        Serial.println(batteryLevel);

        if (distanceTraveled - distanceBeforeRightTurnCharging >= 0.075){
            lastRightTurnChargingDetected = millis();
            distanceToStation = distanceTraveled;
            lastRightTurnDetected = millis();
            drivingStateVar = LINEFOLLOWER_CHARGING;
        }
    }


    if (drivingStateVar == LINEFOLLOWER_CHARGING){
        lineFollower();

        Serial.print("LINEFOLLOWER_CHARGING");
        Serial.print("\t");
        Serial.println(batteryLevel);

        if (distanceTraveled - distanceToStation >= 0.65){
            drivingStateVar = CHARGING_MODE;
        }
    }

    if (drivingStateVar == CHARGING_MODE){
        motors.setSpeeds(0,0);
        Serial.print("CHARGING_MODE");
        Serial.print("\t");
        Serial.println(batteryLevel);
        chargingOptions();
        EEPROM.update(moneyAddress,money);
        EEPROM.update(batteryLevelAddress,batteryLevel);
        EEPROM.update(batteryHealthAddress,batteryHealth);
        randomSeed(millis());
        randomTaxiPercentage = random(30, 90); 
        randomTaxiDistance = random(1, 3);
        serviceDoneTimer = millis();
        valuesShownVar = SHOWING_AFTER_CHARGING;
        drivingStateVar = LINEFOLLOWER_NORMAL;
        display.clear();
        
    }

    if ((batteryLevel == randomTaxiPercentage) && (drivingStateVar == LINEFOLLOWER_NORMAL)){
        taxiStart = millis();
        display.clear();
        drivingStateVar = TAXIMODE_PICKUP;
    }

    if (drivingStateVar == TAXIMODE_PICKUP){
        valuesShownVar = TAXI_MODE;
        motors.setSpeeds(0,0);
        if (millis() - taxiStart >= 2000){
            taxiPickupDistance = distanceTraveled;
            drivingStateVar = TAXIMODE;
        }
    }
    if (drivingStateVar == TAXIMODE){
        lineFollower();
        if (distanceTraveled - taxiPickupDistance >= randomTaxiDistance){
            taxiStop = millis();
            drivingStateVar = TAXIMODE_STOP;
        }
    }
    if (drivingStateVar == TAXIMODE_STOP){
        motors.setSpeeds(0,0);
        if (millis() - taxiStop >= 2000){
            money = money + 100;
            EEPROM.update(moneyAddress,money);
            taxiPickupDistance = distanceTraveled;
            valuesShownVar = SHOWING_VALUES;
            drivingStateVar = LINEFOLLOWER_NORMAL;
        }
    }



    calculateSpeed();


    timePassedForValues = ((millis() - setupDoneTimer)/ 1000);
    timePassedAfterSetup = millis() - setupDoneTimer;

    switch(valuesShownVar){
        case SHOWING_VALUES:
            displayValues();
            if (timePassedAfterSetup - previousMillis2 >= 60000){
                display.clear();
                previousMillis2 = timePassedAfterSetup;
                previousBatteryStatusShown = timePassedAfterSetup;
                valuesShownVar = SHOWING_AVERAGES;
                break;
            }
            if (timePassedAfterSetup - previousBatteryStatusShown >= 10000) /*EXCEPT FOR 60 SECONDS*/{
                display.clear();
                previousBatteryStatusShown = timePassedAfterSetup;
                valuesShownVar = SHOWING_BATTERY_STATUS;
            }
            break;

        case SHOWING_AVERAGES:
            displayAverageDistance();
            timePassedForAverage = (timePassedAfterSetup - previousMillis2) / 1000;
            if (timePassedAfterSetup - previousMillis2 >= 3000){
                display.clear();
                previousMillis2 = timePassedAfterSetup;
                highestSpeed = 0;
                valuesShownVar = SHOWING_VALUES;
            }
            break;

        case SHOWING_BATTERY_STATUS:
            displayBatteryStatus();
            timePassedForBatteryStatus = (timePassedAfterSetup - previousBatteryStatusShown) / 1000;
            if (timePassedAfterSetup - previousBatteryStatusShown >= 3000){
                display.clear();
                //previousBatteryStatusShown = timePassedAfterSetup; 
                valuesShownVar = SHOWING_VALUES; // Vil starte Showing Values timer fra der den slapp av. Nå starter den 3 sek etter
            }
        
            break;

        case SHOWING_AFTER_CHARGING:
            displayAfterCharging();
            if (millis() - serviceDoneTimer >= 15000){
                display.clear();
                totalMoneySpentLastTransaction = 0;
                valuesShownVar = SHOWING_VALUES;
            }
            break;

        case TAXI_MODE:
            display.print("TAXI MODE");

        default:
            break;
    }

    ////////////////////////////////
    ////////// DISPLAY END /////////
    ////////////////////////////////

    ///////////////////////////////////
    ////////// BatteryHealth //////////
    ///////////////////////////////////


    randomEvent();

    // antall ladesykluser
    // Antall ganger batteriet har blitt under 5%
    if ((batteryLevel <= 5) && (batteryHealth > 10) && (batteryLevelUnderFivePercentReductionGiven == false)){
        batteryHealth = batteryHealth - 10;
        EEPROM.update(batteryHealthAddress,batteryHealth);
        batteryLevelUnderFivePercentReductionGiven = true;
    }
    // Gjennomsnittsveriene
    // Level 1 = service / Koster X antall kroner / Forbedrer med X antall prosent
    // Level 0 = nytt batteri / Koster Y antall kroner mer enn service / Resetter til 100

    

    ///////////////////////////////////
    //////// BatteryHealth End ////////
    ///////////////////////////////////


    //////////////////////////////////
    ////////// BatteryLevel //////////
    //////////////////////////////////


    if (cmPerSecond > maxSpeedCmPerSecond*0.7){
        if(millis() - previousTimeForSpeed >= 1000) {
            batteryLevel = batteryLevel - 1;
            EEPROM.update(batteryLevelAddress,batteryLevel);
            previousTimeForSpeed = millis();
        }
    }

    else if (cmPerSecond <= maxSpeedCmPerSecond*0.7 && cmPerSecond > 0){
        if(millis() - previousTimeForSpeed2 >= 10000) {
            batteryLevel = batteryLevel - 1;
            EEPROM.update(batteryLevelAddress,batteryLevel);
            previousTimeForSpeed2 = millis();
        }
    }


    if (batteryLevel <= 10 && underTenPercentAlertVar == NORMAL_UNDER_TEN_PERCENT_ALERT){
        underTenPercentAlertVar = UNDER_TEN_PERCENT;
    }

    switch (underTenPercentAlertVar){
        case UNDER_TEN_PERCENT:
            underTenPercentAlertGivenMillis = millis();
            valuesShownVar = NORMAL_VALUES_SHOWN;
            display.clear();
            display.setLayout11x4();
            underTenPercentAlertVar = UNDER_TEN_PERCENT_GIVEN_MILLIS;
            break;

        case UNDER_TEN_PERCENT_GIVEN_MILLIS:
            underTenPercentAlertFunc();
            break;

        default:
            break;
    }

    if ((batteryLevel <= 5) && (millis() - previousBatteryAlert >= 10000)){
        underFivePercentAlertFunc();
    }



    if((batteryLevel <= 1 || batteryHealth <= 1) && (zeroPercentStageVar == ZERO_PERCENT_NORMAL)){
        holdWhileTillShaken = true;
        zeroPercentStageVar = STUCK_IN_WHILE;
    }
    
    if(zeroPercentStageVar == STUCK_IN_WHILE){
        while(holdWhileTillShaken == true){
            motors.setSpeeds(0,0);
            display.clear();
            imu.read();

            static bool wasShaken = false;

            float shakeValue = ((imu.a.x - imu.g.x) / (imu.a.y - imu.g.y));

            if (shakeValue > shakeThreshold) {
            if (!wasShaken) {
                wasShaken = true;
                totalCarShakes += 1;
            }
            }

            else {
                if (wasShaken) {
                    wasShaken = false;
                }
            } 

            if (totalCarShakes >= 5){
                batteryHealth = batteryHealth + 20;
                batteryLevelUnderFivePercentReductionGiven = false;
                display.clear();
                display.print("Emergency Power");
                display.gotoXY(0,1);
                display.print("Put me down");
                delay(2000);
                display.clear();
                totalCarShakes = 0;
                zeroPercentStageVar = LEAVE_WHILE;
                holdWhileTillShaken = false;
                break;
            }
        }
    }

    if(batteryHealth < 10){
        initiateBatteryService = true;
    }
    if((initiateBatteryService == true || initiateBatteryChange == true) && lineSensorValues[4] > 900){

    }

    if (batteryHealth < 5){
        initiateBatteryChange = true;
    }
}