// CodeOpalX.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "CodeOpalX.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// DirectX Globals
IDXGISwapChain *swapchain;
ID3D11Device *dev;
ID3D11DeviceContext *devcon;
ID3D11RenderTargetView *backbuffer;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

// DirectX Methods
void InitD3D(HWND hWnd);
void CleanD3D(void);
void RenderFrame(float shade);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CODEOPALX, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	// Main message loop:
	const float Blue = 0.4f;
	float shade = Blue;
	float increment = Blue / 1000;
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				break;
			}
		}

		RenderFrame(shade);
		shade -= increment;

		if (shade <= 0.0)
		{
			shade = Blue;
		}
	}

	CleanD3D();

	return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	ZeroMemory(&wcex, sizeof(WNDCLASSEX));

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.hInstance		= hInstance;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wcex.lpszClassName	= szWindowClass;

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   InitD3D(hWnd);
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void InitD3D(HWND hWnd)
 {
     // create a struct to hold information about the swap chain
     DXGI_SWAP_CHAIN_DESC scd;

     // clear out the struct for use
     ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

     // fill the swap chain description struct
     scd.BufferCount = 1;                                    // one back buffer
     scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
     scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
     scd.OutputWindow = hWnd;                                // the window to be used
     scd.SampleDesc.Count = 4;                               // how many multisamples
     scd.Windowed = TRUE;                                    // windowed/full-screen mode

     // create a device, device context and swap chain using the information in the scd struct
     D3D11CreateDeviceAndSwapChain(NULL,
                                   D3D_DRIVER_TYPE_HARDWARE,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   D3D11_SDK_VERSION,
                                   &scd,
                                   &swapchain,
                                   &dev,
                                   NULL,
                                   &devcon);

	 // set the render target
	 ID3D11Texture2D *pBackBuffer;
	 swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	 dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
	 pBackBuffer->Release();
	 devcon->OMSetRenderTargets(1, &backbuffer, NULL);

	 // set the viewport
	 D3D11_VIEWPORT viewport;
	 ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	 viewport.TopLeftX = 0;
	 viewport.TopLeftY = 0;
	 viewport.Width = 800;
	 viewport.Height = 600;
	 devcon->RSSetViewports(1, &viewport);
 }

void CleanD3D()
{
     // close and release all existing COM objects
     swapchain->Release();
	 backbuffer->Release();
     dev->Release();
     devcon->Release();
 }

void RenderFrame(float shade)
{
	devcon->ClearRenderTargetView(backbuffer, D3DXCOLOR(0.0f, 0.5F * shade, shade, 1.0f));
	swapchain->Present(0, 0);
}