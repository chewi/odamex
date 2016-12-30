// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id$
//
// Copyright (C) 1993-1996 by id Software, Inc.
// Copyright (C) 2006-2015 by The Odamex Team.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// DESCRIPTION:
//	Common Windows includes and defines
//
//-----------------------------------------------------------------------------

#ifndef __WIN32INC_H__
#define __WIN32INC_H__

#ifdef _WIN32

    // DrawText macros in winuser.h interfere with v_video.h
    #ifndef NODRAWTEXT
        #define NODRAWTEXT
    #endif  // NODRAWTEXT

    #ifndef NOMINMAX
        #define NOMINMAX
    #endif  // NOMINMAX;

    #define WIN32_LEAN_AND_MEAN
    #ifndef _XBOX
        // need to make winxp compat for raw mouse input
        #if (_WIN32_WINNT < 0x0501)
            #undef _WIN32_WINNT
            #define _WIN32_WINNT 0x0501
        #endif

        #include <windows.h>
    #else
        #define _WIN32_WINNT 0x0400 // win2000 compat
        #include <xtl.h>
    #endif // !_XBOX

	// avoid a conflict with the winuser.h macro DrawText
	#ifdef DrawText
        #undef DrawText
    #endif

    // LoadMenu macros in winuser.h interfere with m_menu.cpp
    #ifdef LoadMenu
        #undef LoadMenu
    #endif  // LoadMenu

    // Visual Studio 2015 has these functions.  Anything earlier does not.
    #if defined(_MSC_VER) && _MSC_VER < 1900
        int snprintf(char *s, size_t n, const char *fmt, ...);
        int vsnprintf(char *s, size_t n, const char *fmt, va_list ap);
    #endif
#endif // WIN32

#endif  // __WIN32INC_H__
