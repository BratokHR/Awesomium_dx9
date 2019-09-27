#include "dllmain.h"

bool object::Init()
{
	short indices[] = {
		0, 1, 2, // 1
		2, 1, 3
	};

	if (!pindexBuf)
	{
		g_pd3dDevice->CreateIndexBuffer(6 * sizeof(short), 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &pindexBuf, 0);
	}

	if (!pvertexBuf)
	{
		g_pd3dDevice->CreateVertexBuffer(4 * sizeof(VERTEXTEXTURE), 0, D3DFVF_XYZ | D3DFVF_TEX1, D3DPOOL_MANAGED, &pvertexBuf, 0);
	}

	VOID *pVoid;
	pindexBuf->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, indices, sizeof(indices));
	pindexBuf->Unlock();

	UpdateSize(1, 1);

	return true;
}

void object::Reset()
{
	SAFE_RELEASE(pvertexBuf);
	SAFE_RELEASE(pindexBuf);
}

void object::Spawn(vec2_t size, vec3_t origin, vec3_t angle)
{
	VectorCopy(origin, position);
	VectorCopy(angle, this->angle);
	VectorCopy(cg->refdef.viewaxis[0], axis[0]);
	VectorCopy(cg->refdef.viewaxis[1], axis[1]);
	VectorCopy(cg->refdef.viewaxis[2], axis[2]);

	this->angle[1] += 90.0f;
	this->angle[0] = 90.0f;
	axis[2][0] = 0;
	axis[2][1] = 0;
	axis[2][2] = 1;

	UpdateSize(size[0], size[1]);

	vec3_t vec_left;
	VectorCopy(axis[1], vec_left);
	VectorScale(vec_left, -1, vec_left);

	// points0------points1
	//   |            |
	//   |            |
	// point2-------points3

	//mark1
	VectorCopy(position, points[0]);
	VectorMA(points[0], size[0] * 0.5f , vec_left, points[0]);
	VectorMA(points[0], size[1], axis[2], points[0]);
	//mark2
	VectorCopy(position, points[1]);
	VectorMA(points[1], size[0] * 0.5f, axis[1], points[1]);
	VectorMA(points[1], size[1], axis[2], points[1]);
	//mark3
	VectorCopy(position, points[2]);
	VectorMA(points[2], size[0] * 0.5f , vec_left, points[2]);
	//mark4
	VectorCopy(position, points[3]);
	VectorMA(points[3], size[0] * 0.5f, axis[1], points[3]);

	b_spawn = true;
}

void object::Draw(LPDIRECT3DTEXTURE9 texture)
{
	if (!b_spawn || !texture)
		return;

	D3DXMATRIX matRotateY, matRotateX, matRotateZ, translation, matScale;
	vec3_t pos;

	camera.GetPositionInCamera(pos, this->position);

	D3DXMatrixRotationX(&matRotateX, D3DXToRadian(angle[0]));
	D3DXMatrixRotationY(&matRotateY, D3DXToRadian(angle[2]));
	D3DXMatrixRotationZ(&matRotateZ, D3DXToRadian(angle[1]));
	D3DXMatrixTranslation(&translation, pos[0], pos[1], pos[2]);

	translation._11 /= 5.0f;
	translation._22 /= 5.0f;
	translation._33 /= 5.0f;

	g_pd3dDevice->SetTransform(D3DTS_WORLD, &(matRotateX * matRotateY * matRotateZ * translation));

	g_pd3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);
	g_pd3dDevice->SetTexture(0, texture);
	g_pd3dDevice->SetPixelShader(NULL);

	g_pd3dDevice->SetStreamSource(0, pvertexBuf, 0, sizeof(VERTEXTEXTURE));
	g_pd3dDevice->SetIndices(pindexBuf);
	g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
}

bool object::GetTracePosition(vec3_t res, vec3_t direction)
{
	if (TraceTriangle(res, cg->refdef.vieworg, direction, points[0], points[1], points[2]) ||
		TraceTriangle(res, cg->refdef.vieworg, direction, points[2], points[1], points[3]))
	{
		b_hover = true;
	}
	else
	{
		b_hover = false;
	}

	return b_hover;
}

bool object::GetCoordXY(vec3_t direction)
{
	if (!GetTracePosition(cross, direction))
	{
		mouseXY[0] = 0;
		mouseXY[1] = 0;
		return false;
	}

	vec2_t mark0_p = { points[0][0], points[0][1] };
	vec2_t mark1_p = { points[1][0], points[1][1] };
	vec2_t point_p = { cross[0], cross[1] };

	float dist_m = GetDistance2D(mark0_p, mark1_p);
	float dist_p = GetDistance2D(mark0_p, cross);

	mouseXY[0] = dist_p * 1920.0f / dist_m;
	mouseXY[1] = (cross[2] - points[0][2]) * 1080.0f / (points[2][2] - points[0][2]);

	return true;
}

void object::UpdateSize(float width, float height)
{
	size[0] = width;
	size[1] = height;

	VERTEXTEXTURE vertices[] =
	{
		{ -width*0.5f, height, 0, 0, 0 }, // 0
		{ width*0.5f, height, 0, 1, 0 }, // 1
		{ -width*0.5f, 0, 0, 0, 1 }, // 2
		{ width*0.5f, 0, 0, 1, 1 }, // 3
	};

	VOID *pVoid;
	pvertexBuf->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, vertices, sizeof(vertices));
	pvertexBuf->Unlock();
}