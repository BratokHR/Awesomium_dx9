#pragma once
struct VERTEXTEXTURE { FLOAT x, y, z, rhw, tu, tv; };
struct VERTEXCOLOR { FLOAT x, y, z, rhw; DWORD color; };

class cRender
{
private:
public:
	void draw_box(float x, float y, float w, float h, D3DCOLOR color);
	void clearTexture(LPDIRECT3DSURFACE9 psurface);
};

extern cRender render;