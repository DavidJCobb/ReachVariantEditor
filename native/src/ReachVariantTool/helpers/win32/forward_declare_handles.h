#pragma once

#pragma push_macro("_MAC")
#ifndef _MAC
   #if defined(_M_M68K) || defined(_M_MPPC)
      #define _MAC
   #endif
#endif

using HANDLE = void*;
#ifdef NO_STRICT
   using HHOOK    = HANDLE;
   using HICON    = HANDLE;
   using HMONITOR = HANDLE;
   using HWND     = HANDLE;
   #if !defined(_MAC)
      using HACCEL   = HANDLE;
      using HBITMAP  = HANDLE;
      using HBRUSH   = HANDLE;
      using HFONT    = HANDLE;
      using HMENU    = HANDLE;
      using HPALETTE = HANDLE;
      using HPEN     = HANDLE;
   #endif
#else
   using HHOOK    = struct HHOOK__*;
   using HICON    = struct HICON__*;
   using HMONITOR = struct HMONITOR__*;
   using HWND     = struct HWND__*;
   #if !defined(_MAC)
      using HACCEL   = struct HACCEL__*;
      using HBITMAP  = struct HBITMAP__*;
      using HBRUSH   = struct HBRUSH__*;
      using HFONT    = struct HFONT__*;
      using HMENU    = struct HMENU__*;
      using HPALETTE = struct HPALETTE__*;
      using HPEN     = struct HPEN__*;
   #endif
#endif
using HCURSOR = HICON;

#pragma pop_macro("_MAC")