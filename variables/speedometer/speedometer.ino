const float countPerRotation = 75.81 * 12;

unsigned long leftEncoderCount = 0;
float rotationCount = 0;
float previousRotationCount = 0;

unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;

int timePassedForValues = 0;
int timePassedForAverage = 0;

bool showingValues = true;

float cmPerSecond = 0;
float rotationsPerSecond = 0;