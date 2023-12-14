#ifndef GYRO_H
#define GYRO_H
#include <Zumo32U4IMU.h>
struct Gyro
{
    void gyroSetup(Zumo32U4IMU &imu)
    {
        imu.init();
        imu.enableDefault();
        imu.configureForBalancing();
    }

    void updateAngleGyro(Zumo32U4IMU &imu)
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
};

#endif