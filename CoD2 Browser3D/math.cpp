#include "dllmain.h"

void ScreenToWorld(vec2_t Screen, vec3_t vectorAngle)
{
	float px = tan(DEG2RAD(cg->refdef.fov_x) * 0.5f);

	float xc = cg->refdef.width  * 0.5f;
	float yc = cg->refdef.height * 0.5f;

	float dx = xc - (float)Screen[0];
	float dy = yc - (float)Screen[1];

	vectorAngle[0] = cg->refdef.viewaxis[0][0] * (xc / px) + cg->refdef.viewaxis[1][0] * dx + cg->refdef.viewaxis[2][0] * dy;
	vectorAngle[1] = cg->refdef.viewaxis[0][1] * (xc / px) + cg->refdef.viewaxis[1][1] * dx + cg->refdef.viewaxis[2][1] * dy;
	vectorAngle[2] = cg->refdef.viewaxis[0][2] * (xc / px) + cg->refdef.viewaxis[1][2] * dx + cg->refdef.viewaxis[2][2] * dy;

	VectorNormalize(vectorAngle);
}

vec_t VectorNormalize(vec3_t v) {
	float	length, ilength;

	length = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	length = sqrt(length);

	if (length) {
		ilength = 1 / length;
		v[0] *= ilength;
		v[1] *= ilength;
		v[2] *= ilength;
	}

	return length;
}

float GetDistanceAB(vec3_t A, vec3_t B)
{
	float dx = A[0] - B[0];
	float dy = A[1] - B[1];
	float dz = A[2] - B[2];

	return (float)(sqrt((dx * dx) + (dy * dy) + (dz * dz)));
}

float GetDistance2D(vec2_t A, vec2_t B)
{
	float dx = A[0] - B[0];
	float dy = A[1] - B[1];

	return (float)(sqrt((dx * dx) + (dy * dy)));
}

void getNormal(vec3_t res, vec3_t vec1, vec3_t vec2, vec3_t vec3)
{
	res[0] = (vec2[1] - vec1[1])*(vec3[2] - vec1[2]) - (vec2[2] - vec1[2])*(vec3[1] - vec1[1]);
	res[1] = (vec2[2] - vec1[2])*(vec3[0] - vec1[0]) - (vec2[0] - vec1[0])*(vec3[2] - vec1[2]);
	res[2] = (vec2[0] - vec1[0])*(vec3[1] - vec1[1]) - (vec2[1] - vec1[1])*(vec3[0] - vec1[0]);
}

void VectorCross(vec3_t res, vec3_t vec1, vec2_t vec2)
{
	for (int i = 0; i < 3; i++)
	{
		res[i] = vec1[(i + 1) % 3] * vec2[(i + 2) % 3] - vec1[(i + 2) % 3] * vec2[(i + 1) % 3];
	}
}

bool TraceTriangle(vec3_t trace, vec3_t origin, vec3_t direction, vec3_t A, vec3_t B, vec3_t C)
{
	float frac, d;
	vec3_t normal;

	getNormal(normal, A, B, C);

	float dot = DotProduct(normal, direction);

	if (dot < 0)
		return false;

	d = -normal[0] * A[0] - normal[1] * A[1] - normal[2] * A[2];

	frac = -(normal[0] * origin[0] + normal[1] * origin[1] + normal[2] * origin[2] + d) / (normal[0] * direction[0] + normal[1] * direction[1] + normal[2] * direction[2]);

	trace[0] = frac * direction[0] + origin[0];
	trace[1] = frac * direction[1] + origin[1];
	trace[2] = frac * direction[2] + origin[2];

	vec3_t orgs[3], sub1, sub2, scal;
	VectorCopy(A, orgs[0]);
	VectorCopy(B, orgs[1]);
	VectorCopy(C, orgs[2]);

	for (int i = 0; i < 3; i++)
	{
		int num = i + 1;

		if (i >= 2)
			num = 0;

		VectorSubtract(orgs[num], orgs[i], sub1);
		VectorSubtract(trace, orgs[i], sub2);
		VectorCross(scal, sub1, sub2);

		if (DotProduct(scal, normal) <= 0)
			return false;
	}

	return true;
}