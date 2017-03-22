#ifndef __PRECOMPILED_H__
#define __PRECOMPILED_H__

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>

#ifdef PTF_WINDOWS
#include <Windows.h>
#pragma comment(lib, "user32.lib")
#endif

#define SDL_MAIN_HANDLED
#include "../inc/SDL.h"
#include "../inc/SDL_syswm.h"
#include "../inc/SDL_image.h"
#include "../inc/SDL_ttf.h"
#pragma comment(lib, "../lib/SDL2.lib")
#pragma comment(lib, "../lib/SDL2_image.lib")
#pragma comment(lib, "../lib/SDL2_ttf.lib")

#include "../inc/glew.h"
#pragma comment(lib, "../lib/glew32.lib")
#pragma comment(lib, "opengl32.lib")

#include "mathlib.h"
#include "system.h"
#include "ptf.h"

#endif