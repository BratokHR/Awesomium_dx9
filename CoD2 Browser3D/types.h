#pragma once
typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];

static void Sayline(const char *msg, ...)
{
	DWORD		d_sayline = 0x004B9230;
	va_list		argptr;
	char		text[150];

	va_start(argptr, msg);
	_vsnprintf_s<150>(text, 150, msg, argptr);
	va_end(argptr);

	__asm
	{
		lea ESI, text
		mov ECX, ESI
			call d_sayline
	}
}

typedef struct
{
	int				pad1[2];
	int				width;
	int				height;
	float			fov_x;
	float			fov_y;
	vec3_t			vieworg;
	vec3_t			viewaxis[3];
	int				pad2[4];
} refdef_t; //size=88, dw=22

typedef struct
{
	int				pad[41308];
	refdef_t		refdef; //22
	vec3_t			refdefViewAngles;
} cg_t; //size=1001888, dw=250472