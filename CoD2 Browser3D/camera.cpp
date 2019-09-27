#include "dllmain.h"

void cCamera::GetPositionInCamera(vec3_t res, vec3_t position)
{
	vec3_t new_vec;
	float dist;

	dist = GetDistanceAB(cg->refdef.vieworg, position) / 1.25f;

	VectorSubtract(cg->refdef.vieworg, position, new_vec);
	VectorNormalize(new_vec);
	VectorMA(position, dist, new_vec, res);
}

void cCamera::Update()
{
	D3DXMATRIX matProjection1, matProjection2;
	D3DXMATRIX matView;
	vec3_t pAt;

	// set state
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);    // turn off the 3D lighting
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);    // turn off culling
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);    // turn on the z-buffer*/
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	// projection
	D3DXMatrixPerspectiveFovLH(&matProjection1, 80.0f, cg->refdef.fov_y / 64.5f, 1, 9000);
	D3DXMatrixPerspectiveFovLH(&matProjection2, D3DXToRadian(cg->refdef.fov_y), cg->refdef.fov_y / 64.5f, 1, 9000);
	matProjection2._11 = matProjection1._11;

	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProjection2);

	// view
	VectorAdd(cg->refdef.vieworg, cg->refdef.viewaxis[0], pAt);

	D3DXMatrixLookAtLH(&matView,
		&D3DXVECTOR3(cg->refdef.vieworg[0], cg->refdef.vieworg[1], cg->refdef.vieworg[2]),
		&D3DXVECTOR3(pAt[0], pAt[1], pAt[2]),
		&D3DXVECTOR3(cg->refdef.viewaxis[2][0], cg->refdef.viewaxis[2][1], cg->refdef.viewaxis[2][2])
		);

	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);
}