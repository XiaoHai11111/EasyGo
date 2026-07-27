#ifndef __HAL_DEF_H
#define __HAL_DEF_H

#include <stdint.h>

namespace HAL
{

/* Clock */
typedef struct
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t week;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint16_t millisecond;
} Clock_Info_t;

/* IMU */
typedef struct
{
    float ax;
    float ay;
    float az;
    float gx;
    float gy;
    float gz;
    float mx;
    float my;
    float mz;
    float roll;
    float yaw;
    float pitch;
} IMU_Info_t;

/* Joints Angle */
typedef struct
{
    float joint1;
    float joint2;
    float joint3;
    float joint4;
    float joint5;
    float joint6;
    float x;
    float y;
    float z;
    float roll;
    float pitch;
    float yaw;
} Dummy_Info_t;

/* Power */
typedef struct
{
    uint16_t voltage;
    uint8_t usage;
    bool isCharging;
} Power_Info_t;

}

#endif
