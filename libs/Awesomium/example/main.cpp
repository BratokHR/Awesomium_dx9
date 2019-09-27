#include <Windows.h>
#include <iostream>

// Directx 9
#include <d3dx9.h>
#include <d3d9.h>

#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "d3d9.lib")

// Awesomium
#include <Awesomium\WebCore.h>
#include <Awesomium\BitmapSurface.h>
#include <Awesomium\STLHelpers.h>

#pragma comment (lib, "awesomium.lib")


LPDIRECT3DDEVICE9 g_pd3dDevice;
D3DPRESENT_PARAMETERS g_d3dpp;
LPDIRECT3DTEXTURE9 tWebPNG;
LPD3DXSPRITE sWebPNG;

Awesomium::WebCore *webCore;
Awesomium::WebView *webView;
Awesomium::BitmapSurface *surface;

int windowWidth = 1200;
int windowHeight = 800;

void init_graphics()
{
	// init awesomium
	webCore = Awesomium::WebCore::Initialize(Awesomium::WebConfig());

	webView = webCore->CreateWebView(windowWidth, windowHeight);
	webView->SetTransparent(true);
	webView->LoadURL(Awesomium::WebURL(Awesomium::WSLit("http://google.com"))); // load google page

	while (webView->IsLoading())
	{
		webCore->Update();
	}

	webView->Focus();

	// init dx9 texture and sprite
	g_pd3dDevice->CreateTexture(windowWidth, windowHeight, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &tWebPNG, 0);
	D3DXCreateSprite(g_pd3dDevice, &sWebPNG);
	surface = (Awesomium::BitmapSurface*)webView->surface();
}

void relese_graphics()
{
	if (tWebPNG) tWebPNG->Release();
	if (sWebPNG) sWebPNG->Release();

	webView->Stop();
	webView->Destroy();
	
	webCore->Update();
	webCore->Shutdown();
}

void draw_awesomium()
{
	webCore->Update();

	if (surface != 0)
	{
		// copy the surface Awesomium in directx texture
		if (surface->is_dirty())
		{
			D3DLOCKED_RECT lockedRect;
			if (SUCCEEDED(tWebPNG->LockRect(0, &lockedRect, NULL, D3DLOCK_DISCARD | D3DLOCK_DONOTWAIT)))
			{
				unsigned char* destBuffer = static_cast<unsigned char*>(lockedRect.pBits);
				surface->CopyTo(destBuffer, lockedRect.Pitch, (int)4, false, false); // <--
				tWebPNG->UnlockRect(0);
			}
		}

		// draw website
		sWebPNG->Begin(D3DXSPRITE_ALPHABLEND);
		sWebPNG->Draw(tWebPNG, NULL, NULL, &D3DXVECTOR3(0, 0, 0), 0xFFFFFFFF);
		sWebPNG->End();
	}
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (webCore && webView)
	{
		Awesomium::WebKeyboardEvent wke(msg, wParam, lParam);
		webView->InjectKeyboardEvent(wke);
		int zDelta;

		switch (msg)
		{
		case WM_TIMER:
			webCore->Update();
			if (webCore)
			{
				InvalidateRect(0, NULL, TRUE);
				UpdateWindow(0);
			}
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
			if (msg == WM_LBUTTONDOWN)
				webView->InjectMouseDown(Awesomium::MouseButton::kMouseButton_Left);
			else
				webView->InjectMouseUp(Awesomium::MouseButton::kMouseButton_Left);
			break;

		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
			if (msg == WM_RBUTTONDOWN)
				webView->InjectMouseDown(Awesomium::MouseButton::kMouseButton_Right);
			else
				webView->InjectMouseUp(Awesomium::MouseButton::kMouseButton_Right);
			break;

		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
			if (msg == WM_MBUTTONDOWN)
				webView->InjectMouseDown(Awesomium::MouseButton::kMouseButton_Middle);
			else
				webView->InjectMouseUp(Awesomium::MouseButton::kMouseButton_Middle);
			break;

		case WM_MOUSEWHEEL:
			zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			webView->InjectMouseWheel(zDelta, 0);
			break;

		case WM_MOUSEMOVE:
			POINT cursor_pos;
			GetCursorPos(&cursor_pos);
			ScreenToClient(hWnd, &cursor_pos);
			webView->InjectMouseMove(cursor_pos.x, cursor_pos.y);
			break;
		};
	}

	if (msg == WM_DESTROY || (msg == WM_KEYDOWN && wParam == VK_ESCAPE))
	{
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPreviousInst, LPSTR lpCommandLine, int nCommandShow)
{
	// Create application window
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, LoadCursor(0, IDC_ARROW), NULL, NULL, "krasota", NULL };
	RegisterClassEx(&wc);
	HWND hwnd = CreateWindow("krasota", "krasota", WS_OVERLAPPEDWINDOW, (GetSystemMetrics(SM_CXSCREEN) - windowWidth) / 2, (GetSystemMetrics(SM_CYSCREEN) - windowHeight) / 2, windowWidth, windowHeight, NULL, NULL, wc.hInstance, NULL);

	// Initialize Direct3D
	LPDIRECT3D9 pD3D;
	if ((pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
	{
		UnregisterClass("krasota", wc.hInstance);
		return 0;
	}

	ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	g_d3dpp.EnableAutoDepthStencil = 1;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	// Create the D3DDevice
	if (pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
	{
		pD3D->Release();
		UnregisterClass("krasota", wc.hInstance);
		return 0;
	}

	// Main loop
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);

	// init graphics
	init_graphics();

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}

		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFF232b2e, 1.0f, 0);

		if (g_pd3dDevice->BeginScene() >= 0)
		{
			// draw
			draw_awesomium();

			g_pd3dDevice->EndScene();
		}
		HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

		// Handle loss of D3D9 device
		if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		{
			g_pd3dDevice->Reset(&g_d3dpp);
		}
	}

	relese_graphics();
	g_pd3dDevice->Release();
	pD3D->Release();
	DestroyWindow(hwnd);
	UnregisterClass("krasota", wc.hInstance);

	return 0;
}