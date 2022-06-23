#include "sha1.h"
#include <string>

namespace cobb::hashing {
   namespace tests {
      static_assert(
         []() -> bool {
            std::string input = "The quick brown fox jumps over the lazy dog";
            //
            byte_iterative_sha1 hasher;
            hasher.accumulate(input.data(), input.size());
            hasher.finalize();

            auto& hash = hasher.state;
            bool success = (hash[0] == 0x2fd4e1c6);
            success &= (hash[1] == 0x7a2d28fc);
            success &= (hash[2] == 0xed849ee1);
            success &= (hash[3] == 0xbb76e739);
            success &= (hash[4] == 0x1b93eb12);
            return success;
         }()
      );
      static_assert(
         []() -> bool {
            std::string input = "The quick brown fox jumps over the lazy dog";
            size_t size_a = input.size() / 2;
            size_t size_b = input.size() - size_a;
            //
            byte_iterative_sha1 hasher;
            hasher.accumulate(input.data(), size_a);
            hasher.accumulate(input.data() + size_a, size_b);
            hasher.finalize();

            auto& hash = hasher.state;
            bool success = (hash[0] == 0x2fd4e1c6);
            success &= (hash[1] == 0x7a2d28fc);
            success &= (hash[2] == 0xed849ee1);
            success &= (hash[3] == 0xbb76e739);
            success &= (hash[4] == 0x1b93eb12);
            return success;
         }()
      );
   }
}