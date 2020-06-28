#include <windows.h>
#include <wincodec.h>
#include <d2d1helper.h>

#pragma once

template <class T>
void SafeRelease(T **ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

class ResourceLoader
{
public:
    ResourceLoader();
    ~ResourceLoader();

    HRESULT ResourceLoader::LoadResourceBitmap(HMODULE,
        ID2D1RenderTarget *,
        IWICImagingFactory *,
        PCWSTR,
        PCWSTR,
        UINT,
        UINT,
        ID2D1Bitmap **);
};