#include <Windows.h>
#include <chrono>
#include <stdint.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dcommon.h>

#pragma once

#define PADDLE_WIDTH_DEFAULT 20
#define PADDLE_HEIGHT_DEFAULT 100
#define PADDLE_COLOR_DEFAULT 0x00bca4b1
#define PADDLE_SPEED_DEFAULT 0.65

class Paddle
{
private:
    static const UINT32 m_color = PADDLE_COLOR_DEFAULT;
    float m_y_speed = PADDLE_SPEED_DEFAULT;
    bool m_isMovingUp;
    bool m_isMovingDown;
    bool m_isPlayer;
    bool m_isPlayer1;
    D2D1_RECT_F m_rectangle;
    D2D1_SIZE_F m_size;

public:
    Paddle();
    ~Paddle();

    void Initialize(bool, bool, D2D1_SIZE_F);
    void Update(bool, D2D1_SIZE_F, clock_t);
    void Draw(void *, int);
    void AdjustSpeed(int, int);
    void ResetPosition(D2D1_SIZE_F);
    void ScreenResize(D2D1_SIZE_F, D2D1_SIZE_F);
    void StartMovingUp();
    void StopMovingUp();
    void StartMovingDown();
    void StopMovingDown();
    bool IsAtLocation(int, int);
    UINT32 GetX() { return m_rectangle.left; }
    UINT32 GetY() { return m_rectangle.top; }
    static UINT32 GetColor() { return m_color; }
    D2D1_RECT_F GetRectangle() { return m_rectangle; }
    D2D1_SIZE_F GetSize() { return m_size; }
    D2D1_SIZE_U GetSizeU() { return D2D1::SizeU((UINT32) m_size.width, (UINT32) m_size.height); }
};