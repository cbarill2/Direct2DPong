#include "paddle.h"

Paddle::Paddle()
{
    m_size.width = PADDLE_WIDTH_DEFAULT;
    m_size.height = PADDLE_HEIGHT_DEFAULT;
}

Paddle::~Paddle()
{
}

void Paddle::Initialize(bool isPlayer1, bool isPlayer, D2D1_SIZE_F screenSize)
{
    m_isMovingDown = false;
    m_isMovingUp = false;
    m_isPlayer = isPlayer;
    m_isPlayer1 = isPlayer1;
    m_rectangle.left = screenSize.width * ((m_isPlayer1) ? 0.05 : 0.95) + ((m_isPlayer1) ? 0 : m_size.width);
    m_rectangle.top = screenSize.height * 0.45;
    m_rectangle.right = m_rectangle.left + m_size.width;
    m_rectangle.bottom = m_rectangle.top + m_size.height;
}

void Paddle::Update(bool isPlayer1, D2D1_SIZE_F screenSize, clock_t dt)
{
    float currentSpeed = (m_y_speed * dt);
    if (m_isPlayer)
    {
    }
    m_rectangle.left = screenSize.width * ((isPlayer1) ? 0.05 : 0.95) + ((isPlayer1) ? 0 : m_size.width);    
    m_rectangle.right = m_rectangle.left + m_size.width;

    if (m_isMovingDown && m_isMovingUp)
    {
    }
    else if (m_isMovingDown)
    {
        if (m_rectangle.bottom + currentSpeed < screenSize.height)
        {
            m_rectangle.top += currentSpeed;
            m_rectangle.bottom += currentSpeed;
        }
    }
    else if (m_isMovingUp)
    {
        if (m_rectangle.top - currentSpeed > 0)
        {
            m_rectangle.top -= currentSpeed;
            m_rectangle.bottom -= currentSpeed;
        }
    }

    if (m_rectangle.bottom >= screenSize.height)
    {
        m_rectangle.top = screenSize.height - m_size.height;
        m_rectangle.bottom = screenSize.height;
    }
}

void Paddle::Draw(void *BitmapMemory, int stride)
{
    uint8_t *Row = (uint8_t *)BitmapMemory;

    Row += (stride * (int)m_rectangle.top);

    for (UINT32 Y = m_rectangle.top; Y < m_rectangle.bottom; ++Y)
    {
        UINT32 *Pixel = (UINT32 *)Row;
        Pixel += (int)m_rectangle.left;
        for (int X = m_rectangle.left; X < m_rectangle.right; ++X)
        {
            *Pixel++ = m_color;
        }
        Row += stride;
    }
}

bool Paddle::IsAtLocation(int X, int Y)
{
    return (X >= m_rectangle.left && X <= m_rectangle.right &&
            Y >= m_rectangle.top && Y <= m_rectangle.bottom);
}

void Paddle::AdjustSpeed(int, int)
{
}

void Paddle::ResetPosition(D2D1_SIZE_F screenSize)
{
    Initialize(m_isPlayer1, m_isPlayer, screenSize);
}

void Paddle::ScreenResize(D2D1_SIZE_F oldSize, D2D1_SIZE_F newSize)
{
    m_rectangle.top = (m_rectangle.top / oldSize.height) * newSize.height;
    m_rectangle.bottom = m_rectangle.top + m_size.height;
    m_rectangle.left = (m_rectangle.left / oldSize.width) * newSize.width;
    m_rectangle.right = m_rectangle.left + PADDLE_WIDTH_DEFAULT;
}

void Paddle::StartMovingUp()
{
    m_isMovingUp = true;
}

void Paddle::StopMovingUp()
{
    m_isMovingUp = false;
}

void Paddle::StartMovingDown()
{
    m_isMovingDown = true;
}

void Paddle::StopMovingDown()
{
    m_isMovingDown = false;
}