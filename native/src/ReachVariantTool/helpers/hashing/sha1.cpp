#include "sha1.h"
#include <string>

namespace cobb::hashing {
   namespace tests {
      #pragma region All-at-once hasher
      static_assert( // Long input
         []() -> bool {
            std::string input;
            input.reserve(1505);
            for (size_t i = 0; i <= (1500 / 7); ++i)
               input += "Reach! ";
            //
            auto hash = sha1(input.data(), input.size() * 8);
            bool success = (hash[0] == 0x32574a98);
            success &= (hash[1] == 0x265fe214);
            success &= (hash[2] == 0xa4c787fd);
            success &= (hash[3] == 0x6447f737);
            success &= (hash[4] == 0x78312e94);
            return success;
         }()
      );
      #pragma endregion

      #pragma region Iterative hasher (bytes only)
      static_assert( // All at once, via the iterative hasher
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
      static_assert( // Iterative
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

      static_assert( // Long input
         []() -> bool {
            std::string input;
            input.reserve(1505);
            for (size_t i = 0; i <= (1500 / 7); ++i)
               input += "Reach! ";
            //
            byte_iterative_sha1 hasher;
            hasher.accumulate(input.data(), input.size());
            hasher.finalize();

            auto& hash = hasher.state;
            bool success = (hash[0] == 0x32574a98);
            success &= (hash[1] == 0x265fe214);
            success &= (hash[2] == 0xa4c787fd);
            success &= (hash[3] == 0x6447f737);
            success &= (hash[4] == 0x78312e94);
            return success;
         }()
      );
      static_assert( // Long input, iterative
         []() -> bool {
            std::string input;
            input.reserve(1505);
            for (size_t i = 0; i <= (1500 / 7); ++i)
               input += "Reach! ";
            //
            size_t size_a = input.size() / 5;
            size_t size_b = input.size() - size_a;
            //
            byte_iterative_sha1 hasher;
            hasher.accumulate(input.data(), size_a);
            hasher.accumulate(input.data() + size_a, size_b);
            hasher.finalize();

            auto& hash = hasher.state;
            bool success = (hash[0] == 0x32574a98);
            success &= (hash[1] == 0x265fe214);
            success &= (hash[2] == 0xa4c787fd);
            success &= (hash[3] == 0x6447f737);
            success &= (hash[4] == 0x78312e94);
            return success;
         }()
      );
      #pragma endregion
   }
}