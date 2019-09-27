#pragma once
#define SAFE_RELEASE(obj) if (obj != 0) { obj->Release(); obj = 0; }

typedef HRESULT(WINAPI *oReset)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
typedef HRESULT(WINAPI *oEndScene)(LPDIRECT3DDEVICE9);

void init_hook();

extern cCamera camera;
extern LPDIRECT3DDEVICE9 g_pd3dDevice;
extern LPDIRECT3DSURFACE9 g_psurfaceOld;