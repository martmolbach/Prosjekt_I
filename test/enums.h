#ifndef ENUMS_H
#define ENUMS_H

enum drivingState
{
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

enum batteryState // Enum for the state of the battery
{
    INITIATE_BATTERY_CHANGE,
    INITIATE_BATTERY_SERVICE,
    NORMAL_BATTERY_STATE
};
enum randomState
{
    HEALTH_HAS_CHANGED,
    NORMAL_RANDOM_STATE
};

enum underTenPercentAlert
{
    UNDER_TEN_PERCENT,
    UNDER_TEN_PERCENT_GIVEN_MILLIS,
    UNDER_TEN_PERCENT_ALERT_GIVEN,
    NORMAL_UNDER_TEN_PERCENT_ALERT
};

enum valuesShown
{
    SHOWING_VALUES,
    SHOWING_AVERAGES,
    SHOWING_BATTERY_STATUS,
    NORMAL_VALUES_SHOWN,
    SHOWING_AFTER_CHARGING,
    TAXI_MODE
};

enum zeroPercentStage
{
    ZERO_PERCENT_NORMAL,
    STUCK_IN_WHILE,
    FIVE_SHAKES_DETECTED,
    LEAVE_WHILE
};

enum State
{
    moveForward,
    moveForward2,
    stopForward,
    turnRight,
    stopTurn,
    turnAround,
    stop,
    finish
};

#endif