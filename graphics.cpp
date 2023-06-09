// Programming 2D Games
// Copyright (c) 2011 by:
// Charles Kelly
// Chapter 3 graphics.cpp v1.2
// Last modification: March-6-2015

#include "graphics.h"

//=============================================================================
// Constructor
//=============================================================================
Graphics::Graphics()
{
    direct3d = NULL;
    device3d = NULL;
    fullscreen = false;
    width = GAME_WIDTH;    // width & height are replaced in initialize()
    height = GAME_HEIGHT;
}

//=============================================================================
// Destructor
//=============================================================================
Graphics::~Graphics()
{
    releaseAll();
}

//=============================================================================
// Initialize DirectX graphics
// throws GameError on error
//=============================================================================
void Graphics::initialize(HWND hw, int w, int h, bool full)
{
    hwnd = hw;
    width = w;
    height = h;
    fullscreen = full;
    bgColor = D3DCOLOR_XRGB(0, 0, 215);

    //initialize Direct3D
    direct3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (direct3d == NULL)
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing Direct3D"));

    initD3Dpp();        // init D3D presentation parameters
    if (fullscreen) {
        if (isAdapterCompatible()) {
            d3dpp.FullScreen_RefreshRateInHz = pMode.RefreshRate;
        } else {
            throw(GameError(gameErrorNS::FATAL_ERROR, "The graphics device does not support the specified resolution and/or format."));
        }
    }

    // determine if graphics card supports harware texturing and lighting and vertex shaders
    D3DCAPS9 caps;
    DWORD behavior;
    result = direct3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
    // If device doesn't support HW T&L or doesn't support 1.1 vertex 
    // shaders in hardware, then switch to software vertex processing.
    if( (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
            caps.VertexShaderVersion < D3DVS_VERSION(1,1) )
        behavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING;  // use software only processing
    else
        behavior = D3DCREATE_HARDWARE_VERTEXPROCESSING;  // use hardware only processing

    //create Direct3D device
    result = direct3d->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hwnd,
        behavior,
        &d3dpp, 
        &device3d);

    if (FAILED(result))
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error creating Direct3D device"));
 
}

//=============================================================================
// Initialize D3D presentation parameters
//=============================================================================
void Graphics::initD3Dpp()
{
    try{
        ZeroMemory(&d3dpp, sizeof(d3dpp));              // fill the structure with 0
        // fill in the parameters we need
        d3dpp.BackBufferWidth   = width;
        d3dpp.BackBufferHeight  = height;
        if(fullscreen)                                  // if fullscreen
            d3dpp.BackBufferFormat  = D3DFMT_X8R8G8B8;  // 24 bit color
        else
            d3dpp.BackBufferFormat  = D3DFMT_UNKNOWN;   // use desktop setting
        d3dpp.BackBufferCount   = 1;
        d3dpp.SwapEffect        = D3DSWAPEFFECT_DISCARD;
        d3dpp.hDeviceWindow     = hwnd;
        d3dpp.Windowed          = (!fullscreen);
        d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
    } catch(...)
    {
        throw(GameError(gameErrorNS::FATAL_ERROR, 
                "Error initializing D3D presentation parameters"));

    }
}

//=============================================================================
// Display the backbuffer
//=============================================================================
HRESULT Graphics::showBackbuffer()
{
    result = E_FAIL;    // default to fail, replace on success
    // (this function will be moved in later versions)
    // Clear the backbuffer to lime green 
    device3d->Clear(0, NULL, D3DCLEAR_TARGET, bgColor, 0.0f, 0);

    // Display backbuffer to screen
    result = device3d->Present(NULL, NULL, NULL, NULL);

    return result;
}

//=============================================================================
// Release all
//=============================================================================
void Graphics::releaseAll()
{
    safeRelease(device3d);
    safeRelease(direct3d);
}

//=============================================================================
// Checks the display adapter for compatibility of BackBuffer height, width,
// and refresh rate.
// Pre: d3dpp is initialized
// Post: Returns true if compatible mode is found, false if not
//=============================================================================
bool Graphics::isAdapterCompatible() {
    UINT modes = direct3d->GetAdapterModeCount(D3DADAPTER_DEFAULT, d3dpp.BackBufferFormat);

    for (int i = 0; i < modes - 1; i++) {
        // pMode is passed by ref and is populated for each found mode (so many side-effects)
        result = direct3d->EnumAdapterModes(D3DADAPTER_DEFAULT, d3dpp.BackBufferFormat, i, &pMode);

        if (pMode.Height == d3dpp.BackBufferHeight &&
            pMode.Width == d3dpp.BackBufferWidth &&
            pMode.RefreshRate >= d3dpp.FullScreen_RefreshRateInHz)
        {
            return true;
        }
    }
    
    // no compatible mode found
    return false;
}

void Graphics::setBackgroundColor(COLOR_ARGB color) {
    bgColor = color;
}