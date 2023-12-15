#include <Zumo32U4.h>
#include <Wire.h>
#include <EEPROM.h>

// Initialize all necessary objects from the Zumo32U4 library
Zumo32U4Encoders encoder;
Zumo32U4Buzzer buzzer;
Zumo32U4LineSensors lineSensors;
Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4OLED display;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4IMU imu;

#include "enums.h" // Contains all enums used in the program

// SPEDOMETER //

const float countPerRotation = 75.81 * 12; // encoder counts per rotation
unsigned long previousMillis = 0;          // Variable to store previous time
unsigned long leftEncoderCount = 0;        // Encoder count on left wheel
float rotationCount = 0;                   // Rotation count on left wheel
float previousRotationCount = 0;           // Previous rotation count left wheel
float distanceTraveled = 0;                // Distance traveled in meters
float cmPerSecond = 0;                     // Speed in cm per second
float rotationsPerSecond = 0;              // Rotations of wheels per second
float highestSpeed = 0;                    // Highest speed in cm per second
// Speedometer End //

// Display //
unsigned long previousMillis2 = 0;            // Variable to store previous time
unsigned long previousBatteryStatusShown = 0; // Variable to store previous shown battery status
int timePassedForValues = 0;                  // Time passed for values to be shown
int timePassedForAverage = 0;                 // Time passed for average values to be shown
int timePassedForBatteryStatus = 0;           // Time passed for battery status to be shown
// Display end //

// LineFollower || DrivingState //
float distanceToStation = 0;                // Distance traveled before reaching charging station
float distanceBeforeLeftTurn = 0;           // Distance traveled before right turn
float distanceBeforeRightTurn = 0;          // Distance traveled before left turn
float distanceBeforeRightTurnCharging = 0;  // Distance traveled before right turn charging
float taxiPickupDistance = 0;               // Distance traveled before taxi pickup
const int maxSpeed = 400;                   // max speed for Zumo32U4
const int maxSpeedCmPerSecond = 65;         // max speed in cm per second
int16_t lastError = 0;                      // Variable to store last error in PID-Line sensor
const int calibrationTime = 3500;           // Calibration time of line sensor (ms)
const int NUM_SENSORS = 5;                  // Number of line sensors
unsigned int lineSensorValues[NUM_SENSORS]; // Array to store line sensor values

// LineFollower end || DrivingState end //

// Setup Timing //
unsigned long setupDoneTimer = 0;       // Timer for when setup is done
unsigned long timePassedAfterSetup = 0; // time passed after setup
// Setup Timing end //

// Battery //
const int shakeThreshold = 500;     // Shake threshold for battery
const int batteryHealthAddress = 0; // EEPROM address for battery health
const int batteryLevelAddress = 1;  // EEPROM address for battery level
const int moneyAddress = 2;         // EEPROM address for money

unsigned long previousTimeForSpeed = 0;            // Variable to store previous time
unsigned long previousTimeForSpeed2 = 0;           // Variable to store previous time
unsigned long previousNumber = 0;                  // Variable to store previous number
unsigned long lastHealthChange = 0;                // Variable to store last battery health change
unsigned long previousWaitTimeForNext = 0;         // Variable to store previous wait time for next random
unsigned long waitTimeForNext = 0;                 // Variable to store wait time for next random
unsigned long previousBatteryAlert = 0;            // Variable to store previous battery alert
unsigned long underTenPercentAlertGivenMillis = 0; // Unsigned long variable to store and update display alerts when battery is under 10%
unsigned long underTenLightBlink = 0;              // Unsigned long variable to store and update blink of lights when battery is under 10%
unsigned long serviceDoneTimer = 0;                // Timer for when service is done

bool batteryLevelUnderFivePercentReductionGiven = false; // Boolean variable to update if battery reduction under 5% has been given
bool batteryHasCharged = false;                          // Boolean variable to update if battery has charged
bool holdWhileTillShaken = false;                        // Boolean variable to update if battery is being shaken

int batteryLevel = 0;   // Battery level
int batteryHealth = 0;  // Battery health
int totalCarShakes = 0; // Total times car has been shaken
int money = 0;          // balance of money

// Random Numbers//
int randomNumber1 = 0;
int randomNumber2 = 0;
int randomNumber3 = 0;
int randomNumber4 = 0;

int totalMoneySpentLastTransaction = 0; // Total amount of money spent under last charging / Battery health service
int totalMoneySpent = 0;                // Total amount of money spent
int moneyEarned = 0;                    // Amount of money earned
// Battery end //

// Taxi //
unsigned long taxiStart = 0; // Timer for when taxi starts
unsigned long taxiStop = 0;  // Timer for when taxi stops
int randomTaxiPercentage = 0; // Random percentage for when the taxi starts (Compared to the battery level)
int randomTaxiDistance = 0; // Random distance the taxi travels before it stops and sets of passengers
// Taxi end //

// GYRO //
const int stopDuration = 2000;  // Duration the car stops 
int forwardCounter = 0;         // Counter for how many times the car has moved forward
float gyroOffsetZ;              // Gyro offset
float angle = 0;                // Angle of gyro
const float wheelDiameter = 3.75;       // Diameter of wheel
unsigned long previousCurrentTime2 = 0; // Variable to store previous time
unsigned long previousCurrentTime = 0;  // Variable to store previous time
// GYRO END //

State currentState = moveForward;   // Starting state for square driving
drivingState drivingStateVar = LINEFOLLOWER_NORMAL; // Starting state for driving
batteryState batteryStateVar = NORMAL_BATTERY_STATE; // Starting state for battery
randomState randomStateVar = NORMAL_RANDOM_STATE; // Starting state for random
underTenPercentAlert underTenPercentAlertVar = NORMAL_UNDER_TEN_PERCENT_ALERT; // Starting state for under ten percent alert
valuesShown valuesShownVar = SHOWING_VALUES; // Starting state for values shown
zeroPercentStage zeroPercentStageVar = ZERO_PERCENT_NORMAL; // Starting state for zero percent stage

// SETUP //
void setup()
{
    // Welcome sequence //
    display.init(); // Initialize display
    display.clear(); // Clear display
    display.print("Welcome");
    buzzer.play(">g32>>c32");
    delay(1000);


    Wire.begin(); // Initialize I2C
    imu.init();   // Initialize IMU
    imu.enableDefault(); // Enable default IMU
    imu.configureForBalancing(); // Configure IMU for balancing
    encoder.init(); // Initialize encoder

    display.clear();
    display.setLayout21x8(); // Set layout for display
    waitForAorB(); // Menu function

    lineSensors.initFiveSensors(); // Initialize line sensors
    display.clear();
    display.gotoXY(4, 3);
    display.print("Calibrating...");
    unsigned long startTime = millis(); // Variable to store start time for calibration
    calibrateSensors(startTime); // Calibrate line sensors
    display.clear();
    display.gotoXY(5, 3);
    display.print("Calibration");
    display.gotoXY(9, 4);
    display.print("Done");
    display.clear();

    encoder.init();
    randomSeed(millis()); // Random seed for random numbers
    randomTaxiPercentage = random(30, 90); // Random percentage for when the taxi starts (used with battery level)
    randomTaxiDistance = random(1, 3);   // Random distance the taxi travels before it stops and sets of passenger
    delay(2000);
    setupDoneTimer = millis();  // Timer for when setup is done
}
// SETUP DONE //

// calculate rotation function //
float calculateRotation()
{
    float rotationCounter = leftEncoderCount / countPerRotation; 
    return rotationCounter;
}

// highest speed function //
float updateHighestSpeed()
{
    if (cmPerSecond > highestSpeed)
    {
        return cmPerSecond;
    }
}

// calculate rotation per second function //
float calculateRotationPerSecond()
{
    return rotationCount - previousRotationCount;
}

// function for if random numbers are the same //
bool randomOccured()
{
    randomNumber1 = random(0, 10000);
    randomNumber2 = random(0, 10000);
    randomNumber3 = random(0, 10000);
    randomNumber4 = random(0, 10000);
    return (randomNumber1 == randomNumber2 == randomNumber3 == randomNumber4);
}

// function for distance //
bool countDistance(float targetDistance)
{
    static float totalDistance = 0;

    int leftEncoder = encoder.getCountsLeft(); // Encoder count on left wheel
    int rightEncoder = encoder.getCountsRight(); // Encoder count on right wheel
    float encoderCount = (abs(leftEncoder) + abs(rightEncoder)) / 2.0; // Encoder count on both wheels
    float rotationCounter = encoderCount / countPerRotation; // Rotation count on both wheels
    float currentDistance = rotationCounter * (PI * wheelDiameter); // Distance traveled

    totalDistance += currentDistance;

    if (totalDistance >= targetDistance)
    {
        totalDistance = 0;
        return true; // Return true if distance is reached
    }

    return false; // Return false if distance is not reached
}

// function for calibration of line sensors //
void calibrateSensors(unsigned long startTime)
{
    while (millis() - startTime < calibrationTime) // Calibrate for 3.5 seconds
    {
        lineSensors.calibrate(); // Calibrate line sensors
        motors.setSpeeds(200, -200); // Rotate
    }
    motors.setSpeeds(0, 0); // Stop
}

// Function for driving in square pattern //
void squareDriving()
{

    unsigned long currentTime = millis(); // Variable to store current time
    float targetDistance = 0.4; // Target distance for square driving

    // switch case for square driving //
    switch (currentState)
    {
    case moveForward: // Move forward
        motors.setSpeeds(200, 200);
        if (countDistance(targetDistance)) // If distance is reached
        {
            currentState = stopForward; // Stop
        }
        break;

    case stopForward: // Stop
        angle = 0; // Reset angle
        motors.setSpeeds(0, 0); 
        if (currentTime - previousCurrentTime2 >= stopDuration) 
        {
            previousCurrentTime2 = currentTime;
            currentState = turnRight; // Turn right
        }
        break;

    case turnRight: // Turn right
        motors.setSpeeds(-100, 100); 
        if (angle >= 90) // if angle is 90 degrees
        {
            currentState = stopTurn; // Stop
            angle = 0; // Reset angle
        }
        break;

    case stopTurn: // Stop
        motors.setSpeeds(0, 0); 
        if (currentTime - previousCurrentTime2 >= stopDuration)
        {
            forwardCounter = forwardCounter + 1; // Add one to forward counter
            previousCurrentTime2 = currentTime; 
            currentState = moveForward; // Move forward
        }
        break;

    default:
        break;
    }
}

// Function for driving in circle pattern //
void sirkelModus()
{
    motors.setSpeeds(100, 200); // Drive in circle
}

// Function for updating of gyro angle //
void updateAngleGyro()
{
    static uint16_t lastUpdate = 0; // Variable to store last update
    uint16_t m = micros(); // Variable to store micros
    uint16_t dt = m - lastUpdate; // Variable to store difference between micros and last update
    lastUpdate = m;  // last update is equal to micros
    imu.readGyro(); // Read gyro
    angle += ((float)imu.g.z - gyroOffsetZ) * 70 * dt / 1000000000; // Update angle
}

// Function for printing of gyro angle //
void printAngles()
{
    display.gotoXY(0, 5);
    display.print(angle);
    display.print(F("  "));
}

// Function for driving forwards and backwards //
void forwardsBackwards()
{
    float targetDistance = 50;  // Target distance for forwards and backwards
    float turnDistance = 30;   // Distance for turning
    unsigned long currentTime = millis(); // Variable to store current time
    int distance = 1; // Distance for forwards and backwards

    // switch case for forwards and backwards //
    switch (currentState)
    {

    case moveForward: // Move forward
        motors.setSpeeds(200, 200);
        if (countDistance(targetDistance)) 
        {
            currentState = stopForward; // Stop
        }
        break;

    case stopForward: // Stop
        motors.setSpeeds(0, 0);
        if (currentTime - previousCurrentTime >= stopDuration)
        {
            previousCurrentTime = currentTime; 
            currentState = turnAround; // Turn around
            angle = 0; // Reset angle
        }
        break;

    case turnAround: // Turn around
        motors.setSpeeds(-100, 100);
        if (angle >= 180)
        {
            angle = 0;
            currentState = stopTurn; // Stop
        }
        break;

    case stopTurn: // Stop
        motors.setSpeeds(0, 0);
        if (currentTime - previousCurrentTime >= stopDuration)
        {
            previousCurrentTime = currentTime;
            currentState = moveForward2; // Move forward
        }
        break;

    case moveForward2: // Move forward
        motors.setSpeeds(200, 200);
        if (countDistance(targetDistance))
            currentState = stop; // Stop
        break;

    case stop: // Stop
        motors.setSpeeds(0, 0);
        if (currentTime - previousCurrentTime >= stopDuration)
        {
            previousCurrentTime = currentTime;
            currentState = finish; // Finish
        }
        break;

    case finish: // Finish 
        break; // Break

    default:
        break;
    }
}

// Function for menu //
void waitForAorB()
{
    while (true)
    {
        display.gotoXY(2, 1);
        display.print("Press A for start");
        display.gotoXY(2, 3);
        display.print("Press B for reset");
        display.gotoXY(2, 5);
        display.print("Press C for patterns");
        if (buttonA.getSingleDebouncedRelease()) // button A pressed
        {
            display.clear();
            break; // Start calibration
        }
        else if (buttonB.getSingleDebouncedRelease()) // Button B pressed
        {
            EEPROM.write(batteryHealthAddress, 100); // Reset battery health to 100
            EEPROM.write(batteryLevelAddress, 100); // Reset battery level to 100
            EEPROM.write(moneyAddress, 70); // Reset money to 70
            display.clear();
            break; // start calibration
        }
        else if (buttonC.getSingleDebouncedRelease()) // Button C pressed. Driving patterns
        {
            display.clear();
            display.print(F("Gyroscope calibrating"));
            delay(500);

            // calibrate gyro //
            for (uint16_t i = 0; i < 1024; i++)
            {
                while (!imu.gyroDataReady()) // wait for data ready
                {
                }
                imu.readGyro(); // read gyro
                gyroOffsetZ += imu.g.z; // add gyro offset
            }

            gyroOffsetZ /= 1024; // Divide gyro offset by 1024
            display.clear();
            display.print("Press A to continue");
        
            while (!buttonA.getSingleDebouncedRelease()) // wait for button A press
            {
                updateAngleGyro(); // update angle function called
                static uint8_t lastCorrectionTime = 0; // Variable to store last correction time
                uint8_t m = millis(); // Variable to store millis
                if ((uint8_t)(m - lastCorrectionTime) >= 20)
                {
                    lastCorrectionTime = m;
                    printAngles();
                }
            }
            delay(500);
            while (true) // while loop for driving pattern options
            {
                display.gotoXY(2, 0);
                display.print("Press A for line");
                display.gotoXY(2, 2);
                display.print("Press B for square");
                display.gotoXY(2, 4);
                display.print("Press C for circle");
                display.gotoXY(2, 6);
                display.print("Press A+B for exit");
                if (buttonA.getSingleDebouncedRelease()) // button A pressed
                {
                    display.clear();
                    while (true)
                    {
                        forwardsBackwards(); // forwards and backwards function called
                        static byte lastCorrectionTime = 0; // Variable to store last correction time
                        byte m = millis(); // Variable to store millis
                        if ((byte)(m - lastCorrectionTime) >= 20) // if time passed is more than 20
                        {
                            lastCorrectionTime = m; // last correction time is equal to millis
                            printAngles(); // print angles function called
                        }
                        if (currentState == finish) // if current state is finish, break out of while loop
                        {
                            currentState = moveForward; // set current state to forward before leaving while loop
                            break;
                        }
                        int holderEncoderLeft = encoder.getCountsAndResetLeft(); 
                        int holderEncoderRight = encoder.getCountsAndResetRight();
                        updateAngleGyro();
                    }
                    display.clear();
                }
                else if (buttonB.getSingleDebouncedRelease()) // button B pressed
                {
                    display.clear();
                    while (true)
                    {
                        squareDriving(); // square driving function called

                        // same as button A //
                        updateAngleGyro();
                        static byte lastCorrectionTime = 0;
                        byte m = millis();
                        if ((byte)(m - lastCorrectionTime) >= 20)
                        {
                            lastCorrectionTime = m;
                            printAngles();
                        }
                        if (forwardCounter == 4)
                        {
                            break;
                        }
                        int holderEncoderLeft = encoder.getCountsAndResetLeft();
                        int holderEncoderRight = encoder.getCountsAndResetRight();
                    }
                }
                else if (buttonC.getSingleDebouncedRelease()) // button C pressed
                {
                    display.clear();
                    while (true)
                    {
                        sirkelModus(); // circle pattern function called
                        static byte lastCorrectionTime = 0;
                        byte m = millis();
                        if ((byte)(m - lastCorrectionTime) >= 20)
                        {
                            lastCorrectionTime = m;
                            printAngles();
                        }
                        if (angle >= 360) // if full circle complete, break out of while loop
                        {
                            angle = 0;
                            motors.setSpeeds(0, 0);
                            break;
                        }
                        int holderEncoderLeft = encoder.getCountsAndResetLeft();
                        int holderEncoderRight = encoder.getCountsAndResetRight();
                        updateAngleGyro();
                    }
                    display.clear();
                }
                else if ((buttonA.isPressed()) && (buttonB.isPressed())) // if button A and button B pressed
                {
                    display.clear();
                    delay(1000);
                    break; // break out of while loop
                }
            }
        }
    }
}

// Function for charging station //
void chargingOptions()
{
    while (true)
    {
        display.gotoXY(2, 1);
        display.print("Press A for charging");
        display.gotoXY(2, 3);
        display.print("Press B for service");
        display.gotoXY(2, 5);
        display.print("Press C for new battery");
        if (buttonA.getSingleDebouncedRelease()) // button A pressed
        {
            display.clear();
            // if battery level under 100 and money over 0. Charge until battery full or money is used up
            while ((batteryLevel < 100) && (money > 0)) 
            {
                display.gotoXY(2, 3);
                batteryLevel = batteryLevel + 1; // add one to battery level

                totalMoneySpent = totalMoneySpent + 1; // add one to total money spent
                totalMoneySpentLastTransaction = totalMoneySpentLastTransaction + 1; // add one to total money spent last transaction
                money = money - 1; // subtract one from money
                batteryHasCharged = true; // set battery has charged to true
            }

            if (batteryHasCharged == true)
            {
                batteryHealth = batteryHealth - 10; // subtract 10 from battery health
                batteryHasCharged = false; // set battery has charged to false
            }

            if ((batteryLevel >= 100))
            {
                display.gotoXY(2, 3);
                display.print("Battery full");
                delay(1000);
                display.clear();
            }

            else if ((money == 0))
            {
                display.gotoXY(2, 3);
                display.print("Not enough money");
                delay(1000);
                display.clear();
            }
            display.clear();
        }
        else if (buttonB.getSingleDebouncedRelease()) // button B pressed
        {
            display.clear();

            if (batteryHealth < 20)
            {
                display.gotoXY(2, 1);
                display.print("Your battery is too damaged");
                display.gotoXY(2, 3);
                display.print("Replacement needed");
                delay(1000);
                display.clear();
            }
            // if batteryhealth inside threshold and enough money, service accepted
            if ((batteryHealth >= 20) && (batteryHealth <= 30) && (money >= 20))
            {
                batteryHealth = batteryHealth + 70; // add 70 to battery health
                money = money - 20; // subtract 20 from money

                totalMoneySpent = totalMoneySpent + 20; // add 20 to total money spent 
                totalMoneySpentLastTransaction = totalMoneySpentLastTransaction + 20; // add 20 to total money spent last transaction
                display.clear();
            }

            else
            {
                display.gotoXY(2, 1);
                display.print("No service needed yet");
                delay(1000);
                display.clear();
            }
            display.clear();
        }

        else if (buttonC.getSingleDebouncedRelease()) // button C pressed
        {
            display.clear();
            // if money over 100 and battery health under 100, new battery accepted
            if ((money >= 100) && (batteryHealth < 100))
            {
                batteryHealth = 100; // set battery health to 100
                batteryLevel = 100; // set battery level to 100
                money = money - 100; // subtract 100 from money

                totalMoneySpent = totalMoneySpent + 100; // add 100 to total money spent
                totalMoneySpentLastTransaction = totalMoneySpentLastTransaction + 100; // add 100 to total money spent last transaction
                display.clear();
            }
            else if ((money < 100)) 
            {
                display.gotoXY(2, 1);
                display.print("Not enough money");
                delay(1000);
                display.clear();
            }
            else if (batteryHealth >= 100)
            {
                display.gotoXY(2, 1);
                display.print("Battery is already new");
                delay(1000);
                display.clear();
            }
        }
        else if (buttonA.isPressed() && buttonB.isPressed()) // if button A and button B pressed
        {
            display.clear();
            delay(1000);
            break; // break out of while loop
        }
    }
}

// Function for displaying average distance //
void displayAverageDistance()
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
    display.print(timePassedForValues); // time passed
    display.print("s");
}

// Function for displaying battery status //
void displayBatteryStatus()
{
    display.gotoXY(0, 0);
    display.print("Battery");

    display.gotoXY(0, 2);
    display.print("Level:");
    display.print(batteryLevel); // batteryLevel
    display.print("%");

    display.gotoXY(0, 3);
    display.print("Health:");
    display.print(batteryHealth); // batteryHealth 
    display.print("%");

    display.gotoXY(16, 6);
    display.print("Time:");
    display.gotoXY(16, 7);
    display.print(timePassedForValues); // time
    display.print("s");
}

// Function for display after charging //
void displayAfterCharging()
{
    display.gotoXY(0, 0);
    display.print("Battery");
    display.gotoXY(0, 2);
    display.print("Level:");
    display.print(batteryLevel);  // batteryLevel
    display.print("%");

    display.gotoXY(0, 3);
    display.print("Health:");
    display.print(batteryHealth); // batteryHealth
    display.print("%");

    display.gotoXY(5, 4);
    display.print("Money spent:");
    display.gotoXY(5, 5);
    display.print(totalMoneySpentLastTransaction); // totalMoneySpentLastTransaction

    display.gotoXY(16, 6);
    display.print("Time:");
    display.gotoXY(16, 7);
    display.print(timePassedForValues); // time
    display.print("s");
}

// Function for displaying values //
void displayValues()
{
    display.gotoXY(0, 0);
    display.print("Speed:");
    display.gotoXY(0, 1);
    display.print(cmPerSecond); // Speed
    display.gotoXY(5, 1);
    display.print("cm/s");

    display.gotoXY(0, 6);
    display.print("Distance:");
    display.gotoXY(0, 7);
    display.print(distanceTraveled); // Distance 
    display.print("m");

    display.gotoXY(16, 0);
    display.print("Money");

    display.gotoXY(13, 1);
    display.print("Use:");
    display.gotoXY(17, 1);
    display.print(totalMoneySpent); // total Money Spent

    display.gotoXY(12, 2);
    display.print("Earn:");
    display.gotoXY(17, 2);
    display.print(moneyEarned); // money earned

    display.gotoXY(13, 3);
    display.print("Tot:");
    display.gotoXY(17, 3);
    display.print(money); // total money on account

    display.gotoXY(16, 6);
    display.print("Time:");
    display.gotoXY(16, 7);
    display.print(timePassedForValues); // time
    display.print("s");
}

// Function for line follower // 
void lineFollower()
{

    int16_t position = lineSensors.readLine(lineSensorValues); // Read line sensor values 

    int error = position - 2000; // Calculate error

    int16_t speedDifference = error / 4 + 6 * (error - lastError); // Calculate speed difference

    lastError = error; // Update last error

    int16_t leftSpeed = maxSpeed + speedDifference; // Calculate left speed
    int16_t rightSpeed = maxSpeed - speedDifference; // Calculate right speed

    leftSpeed = constrain(leftSpeed, 0, maxSpeed); // Constrain speeds
    rightSpeed = constrain(rightSpeed, 0, maxSpeed); // Constrain speeds

    motors.setSpeeds(leftSpeed, rightSpeed); // Set speeds
}

// Function for battery level under 10% alert //
void underTenPercentAlertFunc()
{
    buzzer.play("!T104 L8 a."); // Play sound
    display.gotoXY(2, 0);
    display.print("Battery");
    display.gotoXY(3, 1);
    display.print("Level");
    display.gotoXY(3, 2);
    display.print("under");
    display.gotoXY(4, 3);
    display.print("10%");
    ledRed(0);
    ledYellow(0);
    ledGreen(0);
    if (millis() - underTenLightBlink >= 500) // Blink lights every 0.5s
    {
        ledRed(1);
        ledYellow(1);
        ledGreen(1);
        underTenLightBlink = millis();
    }
    if (millis() - underTenPercentAlertGivenMillis >= 2000) // alert for 2 seconds
    {
        display.clear();
        display.setLayout21x8();
        valuesShownVar = SHOWING_VALUES; 
        underTenPercentAlertVar = UNDER_TEN_PERCENT_ALERT_GIVEN;
    }
}

// Function for battery level under 5% alert //
void underFivePercentAlertFunc()
{
    display.clear();
    buzzer.playNote(NOTE_A(4), 1000, 15); // Play sound
    motors.setSpeeds(0, 0); // Stop
    delay(1000); // delay to simulate low battery bugs
    buzzer.stopPlaying(); // Stop playing sound
    delay(500); // 0.5s delay
    buzzer.playNote(NOTE_A(4), 1000, 15); // Play sound again
    delay(1000); 
    previousBatteryAlert = millis(); // previous battery alert time
}

// function for random batteryhealth change //
void randomEvent()
{
    unsigned long start = millis();
    if (randomOccured() && randomStateVar == NORMAL_RANDOM_STATE)
    {
        batteryHealth = batteryHealth / 2; // divide battery health by 2
        EEPROM.update(batteryHealthAddress, batteryHealth); // update eeprom
        display.clear();
        lastHealthChange = start; 
        start = start - previousNumber;
        previousNumber = start;
        randomStateVar = HEALTH_HAS_CHANGED;
    }

    if (millis() - lastHealthChange >= 240000 && randomStateVar == HEALTH_HAS_CHANGED) // wait 4 minutes before allowing another change
    {
        randomStateVar = NORMAL_RANDOM_STATE;
        previousWaitTimeForNext = waitTimeForNext;
    }
}

// Function for speed calculation //
void calculateSpeed()
{
    unsigned long currentMillis = millis();
    int holderEncoderLeft = encoder.getCountsAndResetLeft();
    int holderEncoderRight = encoder.getCountsAndResetRight();

    leftEncoderCount += (abs(holderEncoderLeft) + abs(holderEncoderRight)) / 2; 
    rotationCount = calculateRotation(); // rotation count

    if (currentMillis - previousMillis >= 100)
    {
        previousMillis = currentMillis;
        rotationsPerSecond = calculateRotationPerSecond() * 10; 
        previousRotationCount = rotationCount;
    }

    cmPerSecond = rotationsPerSecond * PI * wheelDiameter; // RPS multiplied by circumference in cm
}

// Function for variable update //
void updateEEprom()
{
    batteryHealth = EEPROM.read(batteryHealthAddress); // update battery health
    batteryLevel = EEPROM.read(batteryLevelAddress); // update battery level
    money = EEPROM.read(moneyAddress); // update money
}

// function for distance traveled //
int calculateDistanceTraveled()
{
    return rotationCount * 3.75 * PI / 100; // Distance traveled
}

// function for updating driving state //
void updateDrivingStateVar()
{
    // left turn detected //
    if ((lineSensorValues[0] > 600) && (lineSensorValues[1] > 600) && ((drivingStateVar == LINEFOLLOWER_NORMAL) || (drivingStateVar == TAXI_MODE)))
    {
        drivingStateVar = LEFTTURN; 
        distanceBeforeLeftTurn = distanceTraveled;
    }

    // right turn detected //
    if ((lineSensorValues[0] < 200) && (lineSensorValues[1] > 700) && (lineSensorValues[2] > 700) && (lineSensorValues[3] > 700) && (lineSensorValues[4] < 200) && ((drivingStateVar == LINEFOLLOWER_NORMAL) || (drivingStateVar == TAXI_MODE)) && ((batteryLevel > 10) /*|| (batteryHealth > 10)*/))
    {
        distanceBeforeRightTurn = distanceTraveled;
        drivingStateVar = RIGHTTURN;
    }

    // right turn detected while in need of charging //
    if ((lineSensorValues[0] < 200) && (lineSensorValues[1] > 100) && (lineSensorValues[2] > 500) && (lineSensorValues[3] < 300) && (lineSensorValues[4] > 700) && (drivingStateVar == LINEFOLLOWER_NORMAL) && ((batteryLevel < 10) /*|| (batteryHealth < 10)*/))
    {
        distanceBeforeRightTurnCharging = distanceTraveled;
        drivingStateVar = RIGHTTURN_CHARGING;
    }

    // taxi mode //
    if ((batteryLevel == randomTaxiPercentage) && (drivingStateVar == LINEFOLLOWER_NORMAL))
    {
        taxiStart = millis();
        display.clear();
        drivingStateVar = TAXIMODE_PICKUP;
    }
}

// Function for left turn //
void leftTurn()
{
    motors.setSpeeds(-100, 100);
    if (distanceTraveled - distanceBeforeLeftTurn >= 0.1)
    {
        drivingStateVar = LINEFOLLOWER_NORMAL;
    }
}

// function for right turn //
void rightTurn()
{
    motors.setSpeeds(100, -100);
    if (distanceTraveled - distanceBeforeRightTurn >= 0.05)
    {
        drivingStateVar = LINEFOLLOWER_NORMAL;
    }
}

// function for right turn while in need of charging //
void rightTurnCharging()
{
    motors.setSpeeds(100, -100);
    if (distanceTraveled - distanceBeforeRightTurnCharging >= 0.075)
    {
        distanceToStation = distanceTraveled;
        drivingStateVar = LINEFOLLOWER_CHARGING;
    }
}

// function for line follower while in need of charging //
void lineFollowerCharging()
{
    lineFollower();
    if (distanceTraveled - distanceToStation >= 0.65)
    {
        drivingStateVar = CHARGING_MODE;
    }
}

// function for charging mode //
void chargingMode()
{
    motors.setSpeeds(0, 0);
    chargingOptions(); // charging options function called
    EEPROM.update(moneyAddress, money); // update money
    EEPROM.update(batteryLevelAddress, batteryLevel); // update battery level 
    EEPROM.update(batteryHealthAddress, batteryHealth); // update battery health
    randomSeed(millis()); 
    randomTaxiPercentage = random(30, 90); 
    randomTaxiDistance = random(1, 3); 
    serviceDoneTimer = millis();
    valuesShownVar = SHOWING_AFTER_CHARGING; // show values after charging
    drivingStateVar = LINEFOLLOWER_NORMAL; // set driving state to line follower normal
    display.clear();
}

// function for taxi mode pickup //
void taxiModePickup()
{
    valuesShownVar = TAXI_MODE; // show taxi mode on display
    motors.setSpeeds(0, 0);
    if (millis() - taxiStart >= 2000) // wait 2s
    {
        taxiPickupDistance = distanceTraveled;
        drivingStateVar = TAXIMODE;
    }
}

// function for taxi mode //
void taxiMode()
{
    lineFollower();
    if (distanceTraveled - taxiPickupDistance >= randomTaxiDistance) // travel the random taxi distance 
    {
        taxiStop = millis();
        drivingStateVar = TAXIMODE_STOP; // stop
    }
}

// function for taxi mode stop //
void taxiModeStop()
{
    motors.setSpeeds(0, 0);
    if (millis() - taxiStop >= 2000) // wait 2s
    {
        money = money + 100; // add 100 to money
        moneyEarned = moneyEarned + 100; // add 100 to money earned
        EEPROM.update(moneyAddress, money); // update money EEPROM
        taxiPickupDistance = distanceTraveled; 
        valuesShownVar = SHOWING_VALUES;
        drivingStateVar = LINEFOLLOWER_NORMAL;
    }
}

// function for calculating time after setup in seconds //
void calculateTimePassedForValues()
{
    timePassedForValues = ((millis() - setupDoneTimer) / 1000);
}

// function for calculating time after setup in milliseconds //
void calculateTimePassedAfterSetup()
{
    timePassedAfterSetup = millis() - setupDoneTimer;
}

// Loop function //
void loop()
{

    updateEEprom(); // update eeprom function called
    distanceTraveled = calculateDistanceTraveled(); // distance traveled function called and stored in variable
    highestSpeed = updateHighestSpeed(); // highest speed function called and stored in variable
    updateDrivingStateVar(); // update driving state function called
    calculateSpeed(); // calculate speed function called
    calculateTimePassedForValues(); // calculate time function called
    calculateTimePassedAfterSetup(); // calculate time after setup function called

    // switch case for driving state //
    switch (drivingStateVar)
    {
    case LINEFOLLOWER_NORMAL:
        lineFollower();
        break;
    case LEFTTURN:
        leftTurn();
        break;
    case RIGHTTURN:
        rightTurn();
        break;
    case RIGHTTURN_CHARGING:
        rightTurnCharging();
        break;
    case LINEFOLLOWER_CHARGING:
        lineFollowerCharging();
        break;
    case CHARGING_MODE:
        chargingMode();
        break;
    case TAXIMODE_PICKUP:
        taxiModePickup();
        break;
    case TAXIMODE:
        taxiMode();
        break;
    case TAXIMODE_STOP:
        taxiModeStop();
        break;
    }

    // switch case for values shown //
    switch (valuesShownVar)
    {
    case SHOWING_VALUES:
        displayValues();
        if (timePassedAfterSetup - previousMillis2 >= 60000)
        {
            display.clear();
            previousMillis2 = timePassedAfterSetup;
            previousBatteryStatusShown = timePassedAfterSetup;
            valuesShownVar = SHOWING_AVERAGES; // display after 60 seconds
            break;
        }
        if (timePassedAfterSetup - previousBatteryStatusShown >= 10000) 
        {
            display.clear();
            previousBatteryStatusShown = timePassedAfterSetup;
            valuesShownVar = SHOWING_BATTERY_STATUS; // display after 10 seconds
        }
        break;

    case SHOWING_AVERAGES:
        displayAverageDistance();
        timePassedForAverage = (timePassedAfterSetup - previousMillis2) / 1000;
        if (timePassedAfterSetup - previousMillis2 >= 3000) // display for 3 seconds
        {
            display.clear();
            previousMillis2 = timePassedAfterSetup;
            highestSpeed = 0;
            valuesShownVar = SHOWING_VALUES;
        }
        break;

    case SHOWING_BATTERY_STATUS:
        displayBatteryStatus();
        timePassedForBatteryStatus = (timePassedAfterSetup - previousBatteryStatusShown) / 1000;
        if (timePassedAfterSetup - previousBatteryStatusShown >= 3000) // display for 3 seconds
        {
            display.clear();
            valuesShownVar = SHOWING_VALUES;
        }

        break;

    case SHOWING_AFTER_CHARGING:
        displayAfterCharging();
        if (millis() - serviceDoneTimer >= 15000) // display for 15 seconds
        {
            display.clear();
            totalMoneySpentLastTransaction = 0;
            valuesShownVar = SHOWING_VALUES;
        }
        break;

    case TAXI_MODE:
        display.print("TAXI MODE"); // display taxi mode during taxi mode

    default:
        break;
    }

    randomEvent(); // random event function called

    if ((batteryLevel <= 5) && (batteryHealth > 10) && (batteryLevelUnderFivePercentReductionGiven == false))
    {
        batteryHealth = batteryHealth - 10; // reduce batteryhealth by 10 if battery level is under 5%
        EEPROM.update(batteryHealthAddress, batteryHealth);
        batteryLevelUnderFivePercentReductionGiven = true;
    }


    if (cmPerSecond > maxSpeedCmPerSecond * 0.7)
    {
        if (millis() - previousTimeForSpeed >= 1000)
        {
            batteryLevel = batteryLevel - 1; // reduce battery level by 1 every second if speed is over 70% of max speed
            EEPROM.update(batteryLevelAddress, batteryLevel);
            previousTimeForSpeed = millis();
        }
    }

    else if (cmPerSecond <= maxSpeedCmPerSecond * 0.7 && cmPerSecond > 0)
    {
        if (millis() - previousTimeForSpeed2 >= 10000)
        {
            batteryLevel = batteryLevel - 1; // reduce battery level by 1 every 10 seconds if speed is under 70% of max speed
            EEPROM.update(batteryLevelAddress, batteryLevel);
            previousTimeForSpeed2 = millis();
        }
    }

    if (batteryLevel <= 10 && underTenPercentAlertVar == NORMAL_UNDER_TEN_PERCENT_ALERT)
    {
        underTenPercentAlertVar = UNDER_TEN_PERCENT; // under ten percent alert
    }

    // switch case for under ten percent alert //
    switch (underTenPercentAlertVar)
    {
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

    if ((batteryLevel <= 5) && (millis() - previousBatteryAlert >= 10000))
    {
        underFivePercentAlertFunc(); // under five percent alert every 10 seconds
    }


    if ((batteryLevel <= 1) && (zeroPercentStageVar == ZERO_PERCENT_NORMAL))
    {
        holdWhileTillShaken = true;
        zeroPercentStageVar = STUCK_IN_WHILE; // if battery level is 0, go to while loop
    }

    if (zeroPercentStageVar == STUCK_IN_WHILE)
    {
        while (holdWhileTillShaken == true) // while loop to simulate flat battery
        {
            motors.setSpeeds(0, 0);
            display.clear();
            imu.read();

            static bool wasShaken = false; // Variable to store if car was shaken

            float shakeValue = ((imu.a.x - imu.g.x) / (imu.a.y - imu.g.y)); // Calculate shake value

            // check for shaking
            if (shakeValue > shakeThreshold)
            {
                if (!wasShaken)
                {
                    wasShaken = true;
                    totalCarShakes += 1; // count total car shakes
                }
            }

            else
            {
                if (wasShaken)
                {
                    wasShaken = false;
                }
            }

            if (totalCarShakes >= 5) // if car is shaken 5 times, go to emergency power
            {
                batteryLevel = batteryLevel + 20;
                batteryLevelUnderFivePercentReductionGiven = false;
                display.clear();
                display.print("Emergency Power");
                display.gotoXY(0, 1);
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
}