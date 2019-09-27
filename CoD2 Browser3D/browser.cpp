#include "dllmain.h"
#include "tx_mouse.h"

bool cBrowser::WndProc(HWND hwd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int zDelta;

	if (!core || !view || !box.b_hover)
		return false;

	if (b_input)
	{
		Awesomium::WebKeyboardEvent wke(msg, wParam, lParam);
		view->InjectKeyboardEvent(wke);
	}

	switch (msg)
	{
	case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
		mouseDown = true;
		view->InjectMouseDown(Awesomium::MouseButton::kMouseButton_Left);
		return true;

	case WM_LBUTTONUP:
		mouseDown = false;
		view->InjectMouseUp(Awesomium::MouseButton::kMouseButton_Left);
		return true;

	case WM_KEYDOWN:
		if (box.b_hover)
		{
			if (wParam == VK_SHIFT && !b_input)
			{
				b_input = true;
				MemoryWrite((void*)0x00461C6F, (void*)"\x74", 1);
				return true;
			}
			else if (wParam == VK_ESCAPE && b_input)
			{
				b_input = false;
				MemoryWrite((void*)0x00461C6F, (void*)"\x75", 1);
				return true;
			}
		}
		break;
	}

	if (b_input)
	{
		switch (msg)
		{
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
			if (msg == WM_RBUTTONDOWN)
				view->InjectMouseDown(Awesomium::MouseButton::kMouseButton_Right);
			else
				view->InjectMouseUp(Awesomium::MouseButton::kMouseButton_Right);
			break;

		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
			if (msg == WM_MBUTTONDOWN)
				view->InjectMouseDown(Awesomium::MouseButton::kMouseButton_Middle);
			else
				view->InjectMouseUp(Awesomium::MouseButton::kMouseButton_Middle);
			break;

		case WM_MOUSEWHEEL:
			zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			view->InjectMouseWheel(zDelta, 0);
			break;
		}

		return true;
	}

	return false;
}

bool cBrowser::Init()
{
	core = Awesomium::WebCore::Initialize(Awesomium::WebConfig());

	if (!core)
		return false;

	view = core->CreateWebView(1920, 1080);

	if (!view)
		return false;

	view->SetTransparent(false);
	b_init = true;

	return true;
}

bool cBrowser::InitTextures()
{
	if (!ptexture)
	{
		if (g_pd3dDevice->CreateTexture(1920, 1080, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &ptexture, NULL) != D3D_OK)
			return false;
		
		if (ptexture->GetSurfaceLevel(0, &psurface) != D3D_OK)
			return false;
	}

	if (!view)
		return false;

	if (g_pd3dDevice->CreateTexture(1920, 1080, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &tWebPNG, 0) != D3D_OK)
		return false;

	if (D3DXCreateSprite(g_pd3dDevice, &sWebPNG) != D3D_OK)
		return false;

	if (D3DXCreateTextureFromFileInMemory(g_pd3dDevice, &byte_mouse, sizeof(byte_mouse), &ptexMouse) != D3D_OK)
		return false;

	return box.Init();
}

void cBrowser::Reset()
{
	SAFE_RELEASE(ptexture);
	SAFE_RELEASE(psurface);
	SAFE_RELEASE(tWebPNG);
	SAFE_RELEASE(sWebPNG);
	SAFE_RELEASE(ptexMouse);

	box.Reset();
}

void cBrowser::LoadPage(std::string url)
{
	if (!view || !url.size())
		return;

	if (strncmp(url.c_str(), "http://", 7) != 0)
		url = "http://" +  url;

	surface = (Awesomium::BitmapSurface*)view->surface();

	Sayline("Load ^2%s", (char*)url.c_str());
	view->LoadURL(Awesomium::WebURL(Awesomium::WSLit(url.c_str())));
	core->Update();
}

void cBrowser::Back()
{
	if (!b_init || !view)
		return;

	if (view->CanGoBack())
		view->GoBack();
}

void cBrowser::Next()
{
	if (!b_init || !view)
		return;

	if (view->CanGoForward())
		view->GoForward();
}

void cBrowser::Stop()
{
	if (!b_init || !view)
		return;

	view->Stop();
}

void cBrowser::Reload()
{
	if (!b_init || !view)
		return;

	view->Reload(true);
}

void cBrowser::SpawnView(vec3_t origin, vec3_t angle, std::string url)
{
	vec2_t size = { 150, 84 };
	
	VectorCopy(origin, position);
	VectorCopy(angle, this->angle);
	start_url = url;

	box.Spawn(size, origin, angle);
	LoadPage(url);
}

void cBrowser::CloseView()
{
	if (!view)
		return;

	view->Stop();
	view->session()->Release();
	view->Destroy();
}

void cBrowser::Draw()
{
	static float z = 1;
	static float speed = 0.1f;
	static bool v = false;
	static DWORD clrs[3] = { 0xFFFF0000, 0xFF00FF00, 0xFF0000FF };
	POINT CursorPos;

	if (!core || !view)
		return;

	g_pd3dDevice->SetRenderTarget(0, psurface);
	core->Update();

	if (view->IsLoading())
	{
		if (!v) speed += (5.0f - speed) * 0.005f;
		else if (v) speed += (30.0f - speed) * 0.005f;

		if (speed >= 25.0f) v = false;
		else if (speed <= 6.0f) v = true;

		float rad = (speed * 400.0f / 30.0f) - 30.0f;
		float x, y;
		z += speed * 0.001f;

		render.draw_box(0, 0, 1920, 1080, 0xFFFFFFFF);
		for (int i = 0; i < 3; i++)
		{
			x = 1920.0f * 0.5f - rad * cos(D3DX_PI * z + 90 * i);
			y = 1080.0f * 0.5f - rad * sin(D3DX_PI * z + 90 * i);
			render.draw_box(x - 25, y - 25, 50, 50, clrs[i]);
		}

	}
	else
	{
		view->Focus();
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
			DrawSprite(0, 0, tWebPNG, 0xFFFFFFFF);

			// draw mouse
			if (b_input)
			{
				GetCursorPos(&CursorPos);
				ScreenToClient(GetForegroundWindow(), &CursorPos);

				mouse[0] = (int)CursorPos.x;
				mouse[1] = (int)CursorPos.y;

				ScreenToWorld(mouse, direction);
			}
			else
			{
				VectorCopy(cg->refdef.viewaxis[0], direction);
			}

			box.GetCoordXY(direction);
			view->InjectMouseMove((int)box.mouseXY[0], (int)box.mouseXY[1]);

			if (!b_input)
				DrawSprite(box.mouseXY[0], box.mouseXY[1], ptexMouse, 0xFFFFFFFF);
		}
		else
		{
			render.draw_box(0, 0, 1920, 1080, 0xFF400000);
		}
	}

	g_pd3dDevice->SetRenderTarget(0, g_psurfaceOld);
	box.Draw(ptexture);

	if (b_input)
		DrawSprite(mouse[0], mouse[1], ptexMouse, 0xFFFFFFFF);
}

void cBrowser::DrawSprite(float x, float y, LPDIRECT3DTEXTURE9 texture, DWORD color)
{
	if (!texture)
		return;

	sWebPNG->Begin(D3DXSPRITE_ALPHABLEND);
	sWebPNG->Draw(texture, NULL, NULL, &D3DXVECTOR3(x, y, 0), color);
	sWebPNG->End();
}

void cBrowser::ClearTextures()
{
	g_pd3dDevice->SetRenderTarget(0, psurface);
	render.clearTexture(psurface);
}