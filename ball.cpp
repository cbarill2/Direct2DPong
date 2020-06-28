#include "ball.h"

Ball::Ball()
{
    center.x = 0.0f;
    center.y = 0.0f;
    m_radius = 1;
}

Ball::~Ball()
{
}

void Ball::Initialize(D2D1_SIZE_F screenSize)
{
    center.x = screenSize.width / 2;
    center.y = screenSize.height / 2;
    m_radius = BALL_RADIUS_DEFAULT;
}

void Ball::Draw(void *BitmapMemory, int stride)
{
    uint8_t *Row = (uint8_t *)BitmapMemory;

    Row += (stride * (int)(center.y - m_radius));

    for (int Y = 0; Y < 2 * m_radius; ++Y)
    {
        uint32_t *Pixel = (uint32_t *)Row;
        Pixel += (int)center.x - m_radius;
        for (int X = 0; X < 2 * m_radius; ++X)
        {
            *Pixel = m_color;
            Pixel++;
        }
        Row += stride;
    }
}

bool Ball::Update(D2D1_SIZE_F size, D2D1_RECT_F playerRect, D2D1_RECT_F enemyRect, clock_t dt)
{
    center.x += (m_x_speed * dt);
    center.y += (m_y_speed * dt);

    //top of screen collision check
    if (center.y - m_radius <= 0)
    {
        center.y = m_radius + 10;
        m_y_speed = -m_y_speed;
    }

    //bottom of screen collision check
    if (center.y + m_radius >= size.height)
    {
        center.y = size.height - m_radius - 10;
        m_y_speed = -m_y_speed;
    }

    //leftPaddle paddle collision check
    if (center.x - m_radius <= playerRect.right)
    {
        if (center.y >= playerRect.top && center.y <= playerRect.bottom)
        {
            center.x = m_radius + playerRect.right;
            m_x_speed = -m_x_speed;
        }
        else
        {
            return 1;
            //player2 scores a point and reset for next ball
        }
    }

    //player2 paddle collision check
    if (center.x + m_radius >= enemyRect.left)
    {
        if (center.y >= enemyRect.top && center.y <= enemyRect.bottom)
        {
            center.x = enemyRect.left - m_radius;
            m_x_speed = -m_x_speed;
        }
        else
        {
#ifdef DEBUG
            return 1;
#else
            center.x = enemyRect.left - m_radius;
            m_x_speed = -m_x_speed;
#endif
            //leftPaddle scores a point and reset for next ball
        }
    }

    return 0;
}

void Ball::AdjustSpeed(int width, int height)
{
    // m_x_speed = sqrt(width) / width / 2.0;
}

bool Ball::IsAtLocation(int X, int Y)
{
    return (X >= (center.x - m_radius) && X <= (center.x + m_radius) &&
            Y >= (center.y - m_radius) && Y <= (center.y + m_radius));
}