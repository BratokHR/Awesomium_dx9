#include "dllmain.h"

cRender render;

void cRender::draw_box(float x, float y, float w, float h, D3DCOLOR color)
{
	VERTEXCOLOR vertices[4] = {
		{ x, y, 0, 1.0f, color },
		{ x + w, y, 0, 1.0f, color },
		{ x, y + h, 0, 1.0f, color },
		{ x + w, y + h, 0, 1.0f, color }
	};

	g_pd3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(VERTEXCOLOR));
}

void cRender::clearTexture(LPDIRECT3DSURFACE9 psurface)
{
	g_pd3dDevice->SetRenderTarget(0, psurface);
	D3DRECT rect = { 0, 0, (long)1920, (long)1080 };
	g_pd3dDevice->Clear(1, &rect, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), 0, 0, 0);
}