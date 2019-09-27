#include "dllmain.h"

bool wnd_proc_hook = false;
bool lost_device = true;

LPDIRECT3DDEVICE9 g_pd3dDevice;
PDIRECT3DSTATEBLOCK9 g_pstateBlock;
LPDIRECT3DSURFACE9 g_psurfaceOld;
DWORD g_gfx;
WNDPROC m_pWindowProc = nullptr;

cCamera camera;
cBrowser browser;

void console(std::string val)
{
	browser.SpawnView(cg->refdef.vieworg, cg->refdefViewAngles, "http://google.com");
	Beep( 700, 100 );
}

void load(std::string val)
{
	browser.LoadPage(val);
}

void reload(std::string)
{
	browser.Reload();
}

void back(std::string)
{
	browser.Back();
}

void next(std::string)
{
	browser.Next();
}

void stop(std::string)
{
	browser.Stop();
}

void Reset()
{
	SAFE_RELEASE(g_pstateBlock);
	SAFE_RELEASE(g_psurfaceOld);

	browser.Reset();

	lost_device = true;
}

void init_graphics()
{
	if (!g_pstateBlock)
	{
		g_pd3dDevice->BeginStateBlock();
		g_pd3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
		g_pd3dDevice->SetPixelShader(0);
		g_pd3dDevice->SetTexture(0, 0);
		g_pd3dDevice->EndStateBlock(&g_pstateBlock);
	}

	if (!g_psurfaceOld)
		g_pd3dDevice->GetRenderTarget(0, &g_psurfaceOld);

	if (!browser.InitTextures())
		MessageBox(0, "1", "1", 0);
}

LRESULT CALLBACK WndProcHandler(HWND hwd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		Reset();
		break;
	case WM_SYSCOMMAND:
		if (wParam == SC_RESTORE)
			Reset();
		break;
	}

	if (browser.WndProc(hwd, msg, wParam, lParam))
		return true;

	return CallWindowProcA(WNDPROC(m_pWindowProc), hwd, msg, wParam, lParam);
}

oReset pReset;
HRESULT APIENTRY hReset(LPDIRECT3DDEVICE9 pd3dDeivce, D3DPRESENT_PARAMETERS *pPresentationParametrs)
{
	Reset();
	return pReset(pd3dDeivce, pPresentationParametrs);
}

oEndScene pEndScene;
HRESULT APIENTRY hEndScene(LPDIRECT3DDEVICE9 pd3dDevice)
{
	static bool once = false;

	if (!once)
	{
		console_func("spawn", console);
		console_func("load", load);
		console_func("back", back);
		console_func("next", next);
		console_func("reload", reload);
		console_func("stop", stop);
		browser.Init();
		MemoryWrite((void*)0x4D0F15, (void*)"\x75", 1); // enable no recoil
		once = true;
	}

	if (!pd3dDevice)
		return pEndScene(pd3dDevice);

	if (pd3dDevice != g_pd3dDevice)
	{
		g_pd3dDevice = pd3dDevice;
		wnd_proc_hook = false;
		handle_window = ::FindWindow(0, NAME_WINDOW);
		return pEndScene(pd3dDevice);
	}

	if (lost_device)
	{
		// create textures etc...
		init_graphics();
		lost_device = false;
	}

	if (!wnd_proc_hook)
	{
		m_pWindowProc = (WNDPROC)SetWindowLongPtr(handle_window, GWL_WNDPROC, (LONG_PTR)WndProcHandler);
		wnd_proc_hook = true;
	}

	g_pstateBlock->Capture();
	g_pd3dDevice->SetTexture(0, 0);
	g_pd3dDevice->SetPixelShader(0);

	// draw
	camera.Update();
	browser.Draw();

	if (g_pstateBlock->Apply() != D3D_OK)
	{
		MessageBox(0, "stateblock->apply()", "Error!", MB_ICONERROR);
		PostQuitMessage(0);
	}

	HRESULT hRet = pEndScene(pd3dDevice);

	// clear textures
	browser.ClearTextures();
	g_pd3dDevice->SetRenderTarget(0, g_psurfaceOld);

	return hRet;
}

void init_hook()
{
	g_gfx = (DWORD)LoadLibraryA("gfx_d3d_mp_x86_s.dll");
	g_pd3dDevice = (LPDIRECT3DDEVICE9)*(int*)(g_gfx + 0x001D1BF8);

	DWORD *pvTable = (DWORD*)*(DWORD*)g_pd3dDevice;

	pReset = (oReset)DetourFunction((PBYTE)pvTable[16], (PBYTE)hReset);
	pEndScene = (oEndScene)DetourFunction((PBYTE)pvTable[42], (PBYTE)hEndScene);
}