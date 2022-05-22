#pragma once
#include "halo/bitbool.h"
#include "halo/bitnumber.h"

// Forward-declarations for the Reach bitreader specialization.

namespace halo {
   template<
      typename LoadProcess
   > class bitreader;

   namespace reach {
      using bitreader = halo::bitreader<class load_process&>;
   }
}