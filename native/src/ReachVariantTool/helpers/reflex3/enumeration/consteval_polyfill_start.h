#pragma push_macro("CONSTEVAL_POLYFILL")
#if defined(__INTELLISENSE__)
   #define CONSTEVAL_POLYFILL constexpr
#else
   #define CONSTEVAL_POLYFILL constexpr
#endif