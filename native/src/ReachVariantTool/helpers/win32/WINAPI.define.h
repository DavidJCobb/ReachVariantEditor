#ifdef _MAC
   #define WINAPI _cdecl
#elif (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED)
   #define WINAPI __stdcall
#else
   #define WINAPI
#endif