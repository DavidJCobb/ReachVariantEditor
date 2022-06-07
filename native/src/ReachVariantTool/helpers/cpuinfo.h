#pragma once

namespace cobb {
   class cpuinfo {
      protected:
         static cpuinfo instance;
         cpuinfo();

      public:
         static const cpuinfo& get() {
            return instance;
         }

         struct {
            bool sse_1;
            bool sse_2;
            bool sse_3;
            bool ssse_3;
            bool sse_4_1;
            bool sse_4_2;
            bool sse_4a;
            bool sse_5;
            bool avx;
         } extension_support;
         
   };
}