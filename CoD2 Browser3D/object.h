#pragma once
class object
{
private:
	vec3_t position;
	vec3_t angle;
	vec3_t size;
	vec3_t axis[3];
	vec3_t points[4];
	vec3_t cross;

	LPDIRECT3DVERTEXBUFFER9 pvertexBuf;
	LPDIRECT3DINDEXBUFFER9 pindexBuf;

public:
	vec2_t mouseXY;
	bool b_spawn;
	bool b_hover;

	bool Init();
	void Reset();
	void Spawn(vec2_t size, vec3_t origin, vec3_t angle);
	void Draw(LPDIRECT3DTEXTURE9 texture);
	bool GetTracePosition(vec3_t res, vec3_t direction);
	bool GetCoordXY(vec3_t direction);
	void UpdateSize(float width, float height);
};