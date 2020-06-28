#include <windows.h>
#include <wincodec.h>
#include <chrono>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <objbase.h>
#include "ball.h"
#include "paddle.h"
#include "resourceLoader.h"

// TODO: this is a global for now, until we decide how to handle closing
static bool Running, Paused;

static Paddle leftPaddle;
static Paddle rightPaddle;
static Ball ball;

static ID2D1Factory *pD2DFactory = NULL;
static ID2D1HwndRenderTarget *pRenderTarget = NULL;
static IDWriteFactory *pDWriteFactory = NULL;
static IWICImagingFactory2 *pImagingFactory = NULL;
static IDWriteTextFormat *pTextFormat = NULL;
static ID2D1Bitmap *pLeftPaddleBitmap = NULL;
static ID2D1Bitmap *pRightPaddleBitmap = NULL;

static HRESULT InitializeDirect(HWND windowHandle, RECT ClientRect, HINSTANCE instance)
{
    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);

    if (SUCCEEDED(hr))
    {
        hr = pD2DFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(
                windowHandle,
                D2D1::SizeU(
                    ClientRect.right - ClientRect.left,
                    ClientRect.bottom - ClientRect.top)),
            &pRenderTarget);
    }
    if (SUCCEEDED(hr))
    {
        hr = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown **>(&pDWriteFactory));
    }
    if (SUCCEEDED(hr))
    {
        hr = pDWriteFactory->CreateTextFormat(
            L"Gabriola",
            NULL,
            DWRITE_FONT_WEIGHT_REGULAR,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            72.0f,
            L"en-us",
            &pTextFormat);
    }
    // Center align (horizontally) the text.
    if (SUCCEEDED(hr))
    {
        hr = pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    }

    if (SUCCEEDED(hr))
    {
        hr = pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }
    
    if (SUCCEEDED(hr))
    {
        hr = CoCreateInstance(
            CLSID_WICImagingFactory2,
            NULL,
            CLSCTX_ALL,
            IID_IWICImagingFactory,
            (VOID **)&pImagingFactory
        );
    }

    if (SUCCEEDED(hr))
    {
        ResourceLoader rl;
        hr = rl.LoadResourceBitmap(
            instance,
            pRenderTarget,
            pImagingFactory,
            L"PaddleImage",
            L"Image",
            leftPaddle.GetSizeU().width,
            leftPaddle.GetSizeU().height,
            &pLeftPaddleBitmap);
    }

    return hr;
}

static HRESULT DirectDraw()
{
    ID2D1SolidColorBrush *pOrangeBrush = NULL;
    ID2D1SolidColorBrush *pRedBrush = NULL;
    ID2D1BitmapBrush *pBitmapBrush = NULL;
    HRESULT hr = pRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::DarkOrange),
        &pOrangeBrush);
    if (SUCCEEDED(hr))
    {
        hr = pRenderTarget->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::IndianRed),
            &pRedBrush);
    }
    if (SUCCEEDED(hr))
    {
        hr = pRenderTarget->CreateBitmapBrush(pLeftPaddleBitmap, &pBitmapBrush);
    }

    if (SUCCEEDED(hr))
    {
        pRenderTarget->BeginDraw();

        pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
        pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::DarkGray));

        pBitmapBrush->SetTransform(D2D1::Matrix3x2F::Translation(leftPaddle.GetX(), leftPaddle.GetY()));
        pRenderTarget->FillRectangle(
            &leftPaddle.GetRectangle(),
            pBitmapBrush);
        pRenderTarget->FillRectangle(
            &rightPaddle.GetRectangle(),
            pOrangeBrush);
        pRenderTarget->FillEllipse(
            D2D1::Ellipse(
                ball.GetCenter(),
                ball.GetRadius(),
                ball.GetRadius()),
            pRedBrush);
        /* pRenderTarget->DrawBitmap(pLeftPaddleBitmap,
                                  &leftPaddle.GetRectangle(),
                                  1.0f,
                                  D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
                                  NULL); */

        if (Paused)
        {
            pRenderTarget->DrawText(
                L"PAUSE",
                5,
                pTextFormat,
                D2D1::RectF(0.0f, 0.0f, pRenderTarget->GetSize().width, pRenderTarget->GetSize().height),
                pRedBrush);
        }
        hr = pRenderTarget->EndDraw();
        SafeRelease(&pOrangeBrush);
        SafeRelease(&pRedBrush);
        SafeRelease(&pBitmapBrush);
    }

    return hr;
}

static void HandleKeyboardInput(UINT event, WPARAM wParam, LPARAM lParam)
{
    switch (event)
    {
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            Running = false;
            break;
        case 'W':
            leftPaddle.StartMovingUp();
            break;
        case 'S':
            leftPaddle.StartMovingDown();
            break;
        case VK_UP:
            rightPaddle.StartMovingUp();
            break;
        case VK_DOWN:
            rightPaddle.StartMovingDown();
            break;
        case VK_SPACE:
#ifdef DEBUG
            Paused = !Paused;
#else
            Paused = false;
#endif
            break;
        }
        break;
    case WM_KEYUP:
    {
        switch (wParam)
        {
        case 'W':
            leftPaddle.StopMovingUp();
            break;
        case 'S':
            leftPaddle.StopMovingDown();
            break;
        case VK_UP:
            rightPaddle.StopMovingUp();
            break;
        case VK_DOWN:
            rightPaddle.StopMovingDown();
            break;
        }
    }
    }
}

LRESULT CALLBACK MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;
    switch (Message)
    {
    case WM_GETMINMAXINFO:
    {
        LPMINMAXINFO lpMinMax = (LPMINMAXINFO)LParam;
        lpMinMax->ptMinTrackSize.x = 400;
        lpMinMax->ptMinTrackSize.y = 400;
        break;
    }
    case WM_DESTROY:
        //TODO: handle as an error?
        Running = false;
        break;
    case WM_CLOSE:
        //TODO: send a message to the user?
        Running = false;
        break;
    case WM_ACTIVATEAPP:
#ifdef DEBUG
        OutputDebugStringA("ACTIVE\n");
#endif
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT Paint;
        HDC DeviceContext = BeginPaint(Window, &Paint);
        EndPaint(Window, &Paint);
        break;
    }
    case WM_SIZE:
        if (pRenderTarget)
        {
            UINT width = LOWORD(LParam);
            UINT height = HIWORD(LParam);
            leftPaddle.ScreenResize(pRenderTarget->GetSize(), D2D1::SizeF(width, height));
            rightPaddle.ScreenResize(pRenderTarget->GetSize(), D2D1::SizeF(width, height));
            pRenderTarget->Resize(D2D1::SizeU(width, height));
        }
        break;
    case WM_KEYDOWN:
    case WM_KEYUP:
        HandleKeyboardInput(Message, WParam, LParam);
        break;
    default:
        Result = DefWindowProc(Window, Message, WParam, LParam);
        break;
    }

    return (Result);
}

int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CmdLine, int CmdShow)
{
    WNDCLASS WindowClass = {};

    WindowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = MainWindowCallback;
    WindowClass.hInstance = Instance;
    // WindowClass.hIcon = ;
    WindowClass.lpszClassName = "PongWindowClass";

    if (RegisterClass(&WindowClass))
    {
        HWND WindowHandle = CreateWindowEx(0,
                                           WindowClass.lpszClassName,
                                           "Pong Test",
                                           WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                           CW_USEDEFAULT,
                                           CW_USEDEFAULT,
                                           CW_USEDEFAULT,
                                           CW_USEDEFAULT,
                                           0,
                                           0,
                                           Instance,
                                           0);

        if (WindowHandle)
        {
            RECT ClientRect;
            GetClientRect(WindowHandle, &ClientRect);
            clock_t delta = 0;
            clock_t startTime = 0;
            clock_t endTime = 0;
            bool isTwoPlayer = false;
            D2D1_SIZE_F size;
            // long frames = 0;

            Paused = true; //start game paused for now
            MSG Message;
            if (SUCCEEDED(InitializeDirect(WindowHandle, ClientRect, Instance)))
            {
                Running = true;
                size = pRenderTarget->GetSize();
                ball.Initialize(size);
                leftPaddle.Initialize(true, true, size);
                rightPaddle.Initialize(false, isTwoPlayer, size);
            }

            while (Running)
            {
                delta = endTime - startTime;
                startTime = clock();
                // ++frames;
                while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
                {
                    if (Message.message == WM_QUIT)
                    {
                        Running = false;
                    }
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }

                size = pRenderTarget->GetSize();
                leftPaddle.Update(true, size, delta);
                rightPaddle.Update(false, size, delta);
                if (!Paused)
                {
                    Paused = ball.Update(size, leftPaddle.GetRectangle(), rightPaddle.GetRectangle(), delta);
                }
                if (!SUCCEEDED(DirectDraw()))
                {
                    SafeRelease(&pRenderTarget);
                    SafeRelease(&pD2DFactory);
                    GetClientRect(WindowHandle, &ClientRect);
                    InitializeDirect(WindowHandle, ClientRect, Instance);
                }
                endTime = clock();
            }
        }
        else
        {
            // TODO: error?
        }
    }
    else
    {
        // TODO: error?
    };

    return (0);
}
