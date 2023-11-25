void displayValues() {
    display.gotoXY(0,0);
    display.print("Speed:");
    display.gotoXY(0,1);
    display.print(centiPerSecond);
    display.print("cm/s");
    display.gotoXY(0, 6);
    display.print("Distance:");
    display.gotoXY(0,7);
    display.print(rotationCount * 3.75* PI/100); // 3.75 er dm til hjulet
    display.print("m");
    display.gotoXY(16,0);
    display.print("Money");
    display.gotoXY(11,1);
    display.print("Used:");
    display.gotoXY(17,1);
    display.print("4kr");
    display.gotoXY(9,2);
    display.print("Earned:");
    display.gotoXY(17,2);
    display.print("2kr");
    display.gotoXY(10,3);
    display.print("Total:");
    display.gotoXY(17,3);
    display.print("7kr");
    display.gotoXY(16,6);
    display.print("Time:");
    display.gotoXY(16,7);
    display.print(timePassedForValues);
    display.print("sec");
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
