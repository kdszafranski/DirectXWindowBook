// Programming 2D Games
// Copyright (c) 2011 by:
// Charles Kelly
// Chapter 3 DirectX Window v1.1
// winmain.cpp

#define _CRTDBG_MAP_ALLOC       // for detecting memory leaks
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <stdlib.h>             // for detecting memory leaks
#include <crtdbg.h>             // for detecting memory leaks
#include <time.h>
#include "graphics.h"

// Function prototypes
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int); 
bool CreateMainWindow(HWND &, HINSTANCE, int);
LRESULT WINAPI WinProc(HWND, UINT, WPARAM, LPARAM); 
void SetRandomBackgroundColor();

// Global variable
HINSTANCE hinst;

// Graphics pointer
Graphics *graphics;

//=============================================================================
// Starting point for a Windows application
//=============================================================================
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine, int nCmdShow)
{
    // Check for memory leak if debug build
    #if defined(DEBUG) | defined(_DEBUG)
        _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
    #endif

    MSG	 msg;
    HWND hwnd = NULL;

    // Create the window
    if (!CreateMainWindow(hwnd, hInstance, nCmdShow))
        return 1;

    try{
        // Create Graphics object
        graphics = new Graphics;
        // Initialize Graphics, throws GameError
        graphics->initialize(hwnd, GAME_WIDTH, GAME_HEIGHT, FULLSCREEN);

        // main message loop
        int done = 0;
        while (!done)
        {
            // PeekMessage,non-blocking method for checking for Windows messages.
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
            {
                // look for quit message
                if (msg.message == WM_QUIT)
                    done = 1;

                // decode and pass messages on to WinProc
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            } else
                graphics->showBackbuffer();
        }
        safeDelete(graphics);  // free memory before exit
        return msg.wParam;
    }
    catch(const GameError &err)
    {
        MessageBox(NULL, err.getMessage(), "Error", MB_OK);
    }
    catch(...)
    {
        MessageBox(NULL, "Unknown error occured in game.", "Error", MB_OK);
    }
    safeDelete(graphics);  // free memory before exit
    return 0;
}


//=============================================================================
// window event callback function
//=============================================================================
LRESULT WINAPI WinProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
            //tell Windows to kill this program
            PostQuitMessage(0);
            return 0;
        case WM_CHAR:
            switch (wParam) {
                case ESC_KEY:
                    PostQuitMessage(0);
                    return 0;
                case SPACE_KEY:
                    // set random bg color
                    SetRandomBackgroundColor();
                }
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}

//=============================================================================
// Utility to choose and set a randome bg color
//=============================================================================
void SetRandomBackgroundColor() {
    time_t t;
    srand((unsigned)time(&t));
    const DWORD R = rand();
    const DWORD G = rand();
    const DWORD B = rand();
    graphics->setBackgroundColor(D3DCOLOR_XRGB(R, G, B));
}

//=============================================================================
// Create the window
// Returns: false on error
//=============================================================================
bool CreateMainWindow(HWND &hwnd, HINSTANCE hInstance, int nCmdShow) 
{ 
    WNDCLASSEX wcx; 
 
    // Fill in the window class structure with parameters 
    // that describe the main window. 
    wcx.cbSize = sizeof(wcx);           // size of structure 
    wcx.style = CS_HREDRAW | CS_VREDRAW;    // redraw if size changes 
    wcx.lpfnWndProc = WinProc;          // points to window procedure 
    wcx.cbClsExtra = 0;                 // no extra class memory 
    wcx.cbWndExtra = 0;                 // no extra window memory 
    wcx.hInstance = hInstance;          // handle to instance 
    wcx.hIcon = NULL; 
    wcx.hCursor = LoadCursor(NULL,IDC_ARROW);   // predefined arrow 
    wcx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);    // black background 
    wcx.lpszMenuName =  NULL;           // name of menu resource 
    wcx.lpszClassName = CLASS_NAME;     // name of window class 
    wcx.hIconSm = NULL;                 // small class icon 
 
    // Register the window class. 
    // RegisterClassEx returns 0 on error.
    if (RegisterClassEx(&wcx) == 0)    // if error
        return false;

    DWORD style;
    
    if (FULLSCREEN == true) {
        style = WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP;
    } else {
        style = WS_OVERLAPPEDWINDOW;
    }

    // Create window
    hwnd = CreateWindow(
        CLASS_NAME,             // name of the window class
        GAME_TITLE,             // title bar text
        style,                  // window style
        CW_USEDEFAULT,          // default horizontal position of window
        CW_USEDEFAULT,          // default vertical position of window
        GAME_WIDTH,             // width of window
        GAME_HEIGHT,            // height of the window
        (HWND) NULL,            // no parent window
        (HMENU) NULL,           // no menu
        hInstance,              // handle to application instance
        (LPVOID) NULL);         // no window parameters

    // if there was an error creating the window
    if (!hwnd)
        return false;

    if (!FULLSCREEN) {
        // windowed mode
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);

        MoveWindow(hwnd, 
            0, 0,                                               // left, top
            GAME_WIDTH + (GAME_WIDTH - clientRect.right),       // right
            GAME_HEIGHT + (GAME_HEIGHT - clientRect.bottom),    // bottom
            TRUE);  // repaint the window?

    }

    // Show the window
    ShowWindow(hwnd, nCmdShow);

    // Send a WM_PAINT message to the window procedure
    UpdateWindow(hwnd);
    return true;
}
