#pragma once

// Forward-declarations for the Reach bytereader specialization.

namespace halo {
   template<
      typename LoadProcess
   > class bytereader;

   namespace reach {
      using bytereader = halo::bytereader<class load_process&>;
   }
}