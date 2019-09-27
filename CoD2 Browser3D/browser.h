#pragma once
#include <Awesomium\WebCore.h>
#include <Awesomium\BitmapSurface.h>
#include <Awesomium\STLHelpers.h>

class cBrowser
{
private:
	Awesomium::WebCore *core;
	Awesomium::WebView *view;
	Awesomium::BitmapSurface *surface;
	object box;
	vec3_t position;
	vec3_t angle;
	vec3_t direction;
	vec2_t mouse;
	std::string start_url;

	bool mouseDown;
	bool b_init;
	bool b_input;

	LPDIRECT3DTEXTURE9 ptexture;
	LPDIRECT3DSURFACE9 psurface;
	LPDIRECT3DTEXTURE9 ptexMouse;
	LPDIRECT3DTEXTURE9 tWebPNG;
	LPD3DXSPRITE sWebPNG;

public:

	bool WndProc(HWND hwd, UINT msg, WPARAM wParam, LPARAM lParam);
	bool Init();
	bool InitTextures();
	void SpawnView(vec3_t origin, vec3_t angle, std::string url);
	void CloseView();
	void Draw();
	void DrawSprite(float x, float y, LPDIRECT3DTEXTURE9 texture, DWORD color);
	void ClearTextures();
	void Reset();

	void LoadPage(std::string url);
	void Back();
	void Next();
	void Reload();
	void Stop();
};