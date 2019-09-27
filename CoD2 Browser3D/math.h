#pragma once
#define M_PI					3.14159265358979323846f

#define DotProduct(x,y)			((x)[0]*(y)[0]+(x)[1]*(y)[1]+(x)[2]*(y)[2])
#define VectorCopy(a,b)			((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2])
#define VectorSubtract(a,b,c)	((c)[0]=(a)[0]-(b)[0],(c)[1]=(a)[1]-(b)[1],(c)[2]=(a)[2]-(b)[2])
#define VectorSubtract2D(a,b,c)	((c)[0]=(a)[0]-(b)[0],(c)[1]=(a)[1]-(b)[1])
#define VectorAdd(a,b,c)		((c)[0]=(a)[0]+(b)[0],(c)[1]=(a)[1]+(b)[1],(c)[2]=(a)[2]+(b)[2])
#define	VectorScale(v, s, o)	((o)[0]=(v)[0]*(s),(o)[1]=(v)[1]*(s),(o)[2]=(v)[2]*(s))
#define	VectorMA(v, s, b, o)	((o)[0]=(v)[0]+(b)[0]*(s),(o)[1]=(v)[1]+(b)[1]*(s),(o)[2]=(v)[2]+(b)[2]*(s))
#define DEG2RAD( a )			( ( (a) * M_PI ) / 180.0F )

void ScreenToWorld(vec2_t Screen, vec3_t vectorAngle);
vec_t VectorNormalize(vec3_t v);
float GetDistanceAB(vec3_t A, vec3_t B);
float GetDistance2D(vec2_t A, vec2_t B);
bool TraceTriangle(vec3_t trace, vec3_t origin, vec3_t direction, vec3_t A, vec3_t B, vec3_t C);