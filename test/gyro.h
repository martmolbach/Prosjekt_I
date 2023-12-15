#ifndef GYRO_H
#define GYRO_H
#include <Zumo32U4IMU.h>
struct Gyro
{
    float gyroOffsetZ;
    float angle = 0;
    void updateGyroOffset(Zumo32U4IMU &imu)
    {
        gyroOffsetZ += imu.g.z;
    }

    void updateAngleGyro(Zumo32U4IMU &imu)
    {
        static uint16_t lastUpdate = 0;
        uint16_t m = micros();
        uint16_t dt = m - lastUpdate;
        lastUpdate = m;
        imu.readGyro();
        angle += ((float)imu.g.z - gyroOffsetZ) * 70 * dt / 1000000000;
    }

    void gyroSetup(Zumo32U4IMU &imu)
    {
        imu.init();
        imu.enableDefault();
        imu.configureForBalancing();
    }
};

#endif