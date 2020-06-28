#include <Windows.h>
#include <stdint.h>
#include <math.h>
#include <chrono>
#include <dcommon.h>

#pragma once

#define BALL_RADIUS_DEFAULT 20
#define BALL_COLOR_DEFAULT 0x00b3234d
#define BALL_SPEED_DEFAULT 0.55

class Ball
{
private:
    D2D1_POINT_2F center;
    int m_radius;
    const UINT32 m_color = BALL_COLOR_DEFAULT;
    float m_x_speed = BALL_SPEED_DEFAULT; // percentage of the screen to cross per second
    float m_y_speed = BALL_SPEED_DEFAULT; // percentage of the screen to cross per second

public:
    Ball();
    ~Ball();
    Ball(float, float, float);

    void Initialize(D2D1_SIZE_F);
    bool Update(D2D1_SIZE_F, D2D1_RECT_F, D2D1_RECT_F, clock_t);
    void Draw(void *, int);
    void AdjustSpeed(int, int);
    bool IsAtLocation(int, int);
    int GetX() { return center.x; }
    int GetY() { return center.y; }
    D2D1_POINT_2F GetCenter() { return center; }
    int GetRadius() { return m_radius; }
    UINT32 GetColor() { return m_color; }
};