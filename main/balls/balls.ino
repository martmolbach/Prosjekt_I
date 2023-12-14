//////////////////////////////////////////////////////////
///// Fungerende kode for speedometer og linjefølger //////
///////////////////////////////////////////////////////////
 
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
 
const float countPerRotation = 75.81 * 12;
 
unsigned long leftEncoderCount = 0;
float rotationCount = 0;
float previousRotationCount = 0;
 
unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;
 
int timePassedForValues = 0;
int timePassedForAverage = 0;
 
bool showingValues = true;
bool reduceLevel = false;
 
float cmPerSecond = 0;
float rotationsPerSecond = 0;
 
const int maxSpeed = 400;
const int calibrationTime = 3500;
const int NUM_SENSORS = 5;
 
int testValue = 0; // dette er en tester
 
int16_t lastError = 0;
 
unsigned long previousTimeForSpeed = 0;
unsigned long previousTimeForSpeed2 = 0;
 
unsigned int lineSensorValues[NUM_SENSORS];
 
/////////////////////////////
 
//REMOVED unsigned long previousNumber = 0;
unsigned long previousWaitTimeForNext = 0;
unsigned long lastHealthChange = 0;
 
unsigned long waitTimeForNext = 0;
 
unsigned long allowHealthChangeMillis = 0;
unsigned long healthChangeMillis = 0;
unsigned long previousBatteryAlert = 0;
unsigned long underTenPercentAlertGivenMillis = 0;
unsigned long underTenLightBlink = 0;
unsigned long buzzerChangedTime = 0;
 
int batteryLevel = 0;
int batteryHealth = 0;
int previousChange = 0;
 
const int batteryHealthAddress = 0;
const int batteryLevelAddress = 1;
 
bool allowHealthChange = false;
bool batteryLevelAlertGiven = false;
bool underTenPercentAlertGiven = false;
bool boolGivenForUnderTenPercent = false;
 
int randomNumber1 = 0;
int randomNumber2 = 0;
int randomNumber3 = 0;
int randomNumber4 = 0;
 
//////////////////////
 
void setup() {
    display.init();
    display.clear();
    display.gotoXY(2,3);
    display.setLayout21x8();
    buzzer.play(">g32>>c32");
    display.print("Trykk A for start");
    display.gotoXY(2,4);
    display.print("Trykk B for reset");
    waitForAorB();
    lineSensors.initFiveSensors();
    encoder.init();
    display.clear();
    unsigned long startTime = millis();
    display.print("Calib-");
    display.gotoXY(0,1);
    display.print("rating");
    calibrateSensors(startTime);
    display.clear();
    display.print("Calib");
    display.gotoXY(0,1);
    display.print("done");
    delay(2000);
    display.clear();
    encoder.init();
    buzzerChangedTime = millis();
}
 
void waitForAorB() {
    while(true){
        if (buttonA.isPressed()){
            break;
        }
        else if (buttonB.isPressed()){
            EEPROM.write(batteryHealthAddress, 100);
            EEPROM.write(batteryLevelAddress, 100);
            break;
        }
    }
}
 
float calculateRotation() {
    float rotationCounter = leftEncoderCount / countPerRotation;
    return rotationCounter;
}
 
float calculateRotationPerSecond() {
    return rotationCount - previousRotationCount;
}
 
void calibrateSensors(unsigned long startTime) {
    while(millis() - startTime < calibrationTime){
        lineSensors.calibrate();
        motors.setSpeeds(200,-200);
    }
    motors.setSpeeds(0,0);
}
 
void displayAverageDistance(){
    display.gotoXY(5,1);
    display.print("Average for");
    display.gotoXY(3,2);
    display.print("last 60 seconds: ");
    display.gotoXY(9,4);
    display.print((rotationCount * 3.75* PI/100) / 60);
    display.gotoXY(16,6);
    display.print("Time: ");
    display.gotoXY(16,7);
    display.print(timePassedForAverage);
    display.print("sec");
}
 
void displayValues() {
    display.gotoXY(0,0);
    display.print("Battery Health:");
    display.gotoXY(0,1);
    display.print(batteryHealth);
    display.gotoXY(0,4);
    display.print("Battery Level:");
    display.gotoXY(0,5);
    display.print(batteryLevel);
}

enum BatterySimulationState {
    CHECK_RANDOMIZE,
    WAIT_STATE
}
BatterySimulationState batterySimulationState = CHECK_RANDOMIZE;

enum BatteryStatus {
    BATTERY_OK,
    BATTERY_BELOW_10_PERCENT_SHOW_WARNING,
    BATTERY_BELOW_10_PERCENT_WARNING_COMPLETE
}
BatteryStatus batteryStatus = BATTERY_OK;

enum UnderFiveBuzzerStatus {
    BUZZER_SILENT,
    BUZZER_TONE_ONE,
    BUZZER_TONE_TWO
}
UnderFiveBuzzerStatus UnderFiveBuzzerStatus = BUZZER_SILENT;

BatteryStatus BatteryStatus = BATTERY_OK;

void degradeBattery() {
    unsigned long start = millis();

    // Reduce battery 50%
    batteryHealth = batteryHealth / 2;
    //batteryLevel = batteryLevel / 2;
    EEPROM.update(batteryHealthAddress,batteryHealth);
    EEPROM.update(batteryLevelAddress,batteryLevel);
    display.clear();
    lastHealthChange = start;
    //REMOVED start = start - previousNumber;
    //REMOVED previousNumber = start;    
}

void setSpeedLineFollower() {
    int16_t position = lineSensors.readLine(lineSensorValues);
 
    int error = position - 2000;
 
    int16_t speedDifference = error / 4 + 6 * (error - lastError);
 
    lastError = error;
 
    int16_t leftSpeed = maxSpeed + speedDifference;
    int16_t rightSpeed = maxSpeed - speedDifference;
 
    leftSpeed = constrain(leftSpeed, 0, maxSpeed);
    rightSpeed = constrain(rightSpeed, 0, maxSpeed);
 
    motors.setSpeeds(leftSpeed, rightSpeed);
}

bool randomOccured() {
    randomNumber1 = random(0, 10000);
    randomNumber2 = random(0, 10000);
    randomNumber3 = random(0, 10000);
    randomNumber4 = random(0, 10000);
    return (randomNumber1 == randomNumber2 == randomNumber3 == randomNumber4)
}

void checkBatterySimulation() {
    if (randomOccured() && batterySimulationState == CHECK_RANDOMIZE) {   // Denne kjøres kun dersom status på batterySimulationState er CHECK_RANDOMIZE. I funksjonen settes denne til WAIT_STATE.
        degradeBattery();
        batterySimulationState = WAIT_STATE;  // Her setter jeg batterySimulationState til WAIT_STATE for å unngå at det kjøres inntil batterySimulationState er satt tilbake til CHECK_RANDOMIZE
    }
 
    if (millis() - lastHealthChange >= 240000 && batterySimulationState == WAIT_STATE) {
        batterySimulationState = CHECK_RANDOMIZE;  // Her hopper den tilbake til CHECK_RANDOMIZE når det er gått 240 sekunder.
    }
}

void adjustBatteryLevel(Int delta) {
    if(millis() - previousTimeForSpeed >= delta) {
        batteryLevel = batteryLevel - 1;
        EEPROM.update(batteryLevelAddress,batteryLevel);
        previousTimeForSpeed = millis();
    }
}

void calculateBatteryLevel() {
    // Her har jeg flyttet koden for å redusere batteriet inn i en funksjon. De var veldig like, kun deltaen på 1 sekund eller 10 sekund var forskjellig. 
    // Du trenger ikke 2 time variabler heller. Kun en tenker jeg. Er jo siste justeringstidspunkt som er interessant, uavhengig av om farta var over eller under 70% av 65?
    if (cmPerSecond > 65*0.7) {
        adjustBatteryLevel(1000); 
    } else if (cmPerSecond <= 65*0.7 && cmPerSecond > 0) {
        adjustBatteryLevel(10000);
    } 
}
 
void readBatteryHealth() {
    batteryHealth = EEPROM.read(batteryHealthAddress);
    batteryLevel = EEPROM.read(batteryLevelAddress);
}

void warnBatteryUnderTenPct() {
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
}

void turnOnLeds() {
    ledRed(1);
    ledYellow(1);
    ledGreen(1);
}

void resetScreenAfterBattertWarning() {
    display.clear();
    display.setLayout21x8();
    showingValues = true;    
}

void alertUnderFivePct() {
        // For å unngå sleep (som er blokkerende kode), så må vi også kjøre dette i en "state-machine". Her brukes UnderFiveBuzzerStatus som går fra SILENT til TONE1 til TONE2 og tilbake til SILENT igjen.
        if ((millis() - buzzerChangedTime > 1000) && underFiveBuzzerStatus == BUZZER_SILENT) {
            buzzer.playNote(NOTE_A(4), 1000, 15);
            motors.setSpeeds(0,0);        
            buzzerChangedTime = millis();
            underFiveBuzzerStatus = BUSSER_TONE_ONE;
        } else if ((millis() - buzzerChangedTime > 1000) && underFiveBuzzerStatus == BUZZER_TONE_ONE) {
            buzzer.playNote(NOTE_A(4), 1000, 15);
            motors.setSpeeds(0,0);        
            buzzerChangedTime = millis();
            underFiveBuzzerStatus = BUSSER_TONE_TWO;
        } else if (millis() - buzzerChangedTime >= 13000 && underFiveBuzzerStatus = BUSSER_TONE_TWO) {
            underFiveBuzzerStatus = BUZZER_SILENT;
        }    
}

void loop() {
    setSpeedLineFollower();  // Samlet kodelinjene i en funksjon. Usikker på hva denne bør hete, men tok navnet fra kommentarene

    readBatteryHealth();  // Flyttet kodelinjene i en funksjon. For readability.... :-) 

    if (showingValues == true){
        displayValues();
    } 

    // Her kjøres "randomize" koden som degraderer batteriet...
    checkBatterySimulation();    

    calculateBatteryLevel(); // Flyttet inn i en funksjon. Kodelinjene hører sammen. Da blir det litt enklere å lese. Kommenter gjerne over hva funksjonen gjør.

    // Varsel under 10%. Gå fra Batteri=OK til Batteri under 10 prosent status.
    if (batteryLevel <= 10 && batteryStatus == BATTERY_OK) {
        // Går fra OK til under 10%
        underTenPercentAlertGivenMillis = millis();
        showingValues = false;
        display.clear();
        display.setLayout11x4();
        batteryStatus = BATTERY_BELOW_10_PERCENT_SHOW_WARNING;
    }
 
    if (batteryLevel <= 10 && batteryLevel > 5 && batteryStatus == BATTERY_BELOW_10_PERCENT_SHOW_WARNING) {
        // Repeter denne første koden HVER loop så lenge batteriet er under 10?
        warnBatteryUnderTenPct();   // Igjen, flyttet ut i funksjon

        if (millis() - underTenLightBlink >= 500) {
            turnOnLeds();
            underTenLightBlink = millis();
        }
        if (millis() - underTenPercentAlertGivenMillis >= 2000){            
            resetScreenAfterBatteryWarning();  // Gå tilbake til slik skjermen var før, og gå videre til neste batteristatus som hindrer dette i å kjøres på nytt
            batteryStatus = BATTERY_BELOW_10_PERCENT_WARNING_COMPLETE;
        }
    } else if (batteryLevel <= 5) {
        alertUnderFivePct();
    }
 
    // batteri tomt
    if(batteryLevel <= 0 || batteryHealth <= 0) {
        motors.setSpeeds(0,0);
    }
    // batteri tomt slutt
 
    // battery level slutt //
 
    /////////////////////////////////////
    ////////// BatteryCharging //////////
    /////////////////////////////////////
 
    if (batteryHealth < 10){
        initiateBatteryService = true;
    }
    if (batteryHealth < 5){
        initiateBatteryChange = true;
    }
 
    // setter bilen i lademodus med et knappetrykk //
   
    // samtidig som bilen lades må underTenPercentAlertGiven settes til true. Samt mange andre variabler
 
    // Bilen skal kun lade om den har mindre enn 5 % batteri (vurderer å sette som 10%)
    // samtidig som ladingen pågår, og 5 sekunder etter viser displayet battery_level, charging_cost og account_balance
 
    // Battery Charging slutt //
 
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
 
    int holderEncoderLeft = encoder.getCountsAndResetLeft();
    int holderEncoderRight = encoder.getCountsAndResetRight();
    leftEncoderCount += (abs(holderEncoderLeft) + abs(holderEncoderRight)) / 2;
    rotationCount = calculateRotation();
 
    unsigned long currentMillis = millis();
 
    if (currentMillis - previousMillis >= 100) {
        previousMillis = currentMillis;
        rotationsPerSecond = calculateRotationPerSecond() * 10;
        previousRotationCount = rotationCount;
    }
 
    cmPerSecond = rotationsPerSecond * PI * 3.75; // RPS * omkrets
 
    unsigned long currentMillis2 = millis();
 
    timePassedForValues = (currentMillis - previousMillis2) / 1000;
}