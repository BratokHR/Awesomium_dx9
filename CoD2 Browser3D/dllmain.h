#pragma once
#include "stdafx.h"
#include "memhook.h"
#include "types.h"
#include "math.h"
#include "render.h"
#include "camera.h"
#include "object.h"
#include "browser.h"
#include "direct.h"

#define NAME_WINDOW "Call of Duty 2 Multiplayer"

typedef bool(*oRegisterFunction) (std::string func_name, void(*func)(std::string s));

extern HWND handle_window;
extern oRegisterFunction console_func;

extern cg_t *cg;