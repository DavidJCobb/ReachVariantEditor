/*

This file is provided under the Creative Commons 0 License.
License: <https://creativecommons.org/publicdomain/zero/1.0/legalcode>
Summary: <https://creativecommons.org/publicdomain/zero/1.0/>

One-line summary: This file is public domain or the closest legal equivalent.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
#pragma once
#include <cstdint>
#include <limits>

namespace cobb {
   template<uint32_t count> class bitset {
      private:
         static constexpr int      bits_per_chunk    = 32;
         static constexpr uint32_t all_bits_set      = std::numeric_limits<uint32_t>::max();
         static constexpr int      chunk_count       = count / bits_per_chunk + (count % bits_per_chunk ? 1 : 0);
         //
         // To understand these next constexprs and any other mentions of "partial chunks," 
         // see the comments for find_first_clear.
         //
         static constexpr int      undershoot_cc     = count / bits_per_chunk; // number of non-partial chunks
         static constexpr int      bits_in_partial   = count % bits_per_chunk;
         static constexpr bool     has_partial_chunk = bits_in_partial != 0;
         static constexpr int      partial_chunk_max = (1 << (count % bits_per_chunk)) - 1; // like all_bits_set but for the partial chunk
         //
         uint32_t data[chunk_count];
         //
      public:
         bitset() {
            memset(&this->data, 0, sizeof(this->data));
         }
         //
         uint32_t dword(uint8_t i) const noexcept {
            return this->data[i];
         }
         unsigned int dword_count() const noexcept {
            return chunk_count;
         }
         constexpr uint32_t size() const noexcept { return count; }
         //
         bool none() const noexcept { // return true if all bits are clear
            //
            // We don't have to worry about partial chunks here, since we memset all chunks 
            // to zero. The unused portions of a partial chunk should always be cleared.
            //
            for (uint32_t i = 0; i < chunk_count; i++)
               if (this->data[i])
                  return false;
            return true;
         }
         bool test(uint32_t index) const noexcept {
            uint32_t ci  = index / bits_per_chunk;
            uint32_t bit = 1 << (index % bits_per_chunk);
            return this->data[ci] & bit;
         }
         //
         void set(uint32_t index) noexcept {
            uint32_t ci  = index / bits_per_chunk;
            uint32_t bit = 1 << (index % bits_per_chunk);
            this->data[ci] |= bit;
         }
         void reset(uint32_t index) noexcept {
            uint32_t ci  = index / bits_per_chunk;
            uint32_t bit = 1 << (index % bits_per_chunk);
            this->data[ci] &= ~bit;
         }
         void modify(uint32_t index, bool state) noexcept {
            if (state)
               this->set(index);
            else
               this->reset(index);
         }
         void clear() noexcept {
            for (uint32_t i = 0; i < chunk_count; i++)
               this->data[i] = 0;
         }
         //
         void set_all() noexcept {
            for (uint32_t i = 0; i < undershoot_cc; i++)
               this->data[i] = 0xFFFFFFFF;
            if (has_partial_chunk)
               this->data[chunk_count - 1] = partial_chunk_max;
         }
         void set_range(uint32_t start, uint32_t end) noexcept {
            uint32_t i = start;
            for (; i + 32 < end; i += 32)
               this->data[i / 32] = 0xFFFFFFFF;
            for (; i < end; i++)
               this->set(i);
         }
         void clear_range(uint32_t start, uint32_t end) noexcept {
            uint32_t i = start;
            for (; i + 32 < end; i += 32)
               this->data[i / 32] = 0;
            for (; i < end; i++)
               this->reset(i);
         }
         //
         int32_t find_first_clear() const noexcept {
            //
            // Finds the first zero bit in the set. This function performs significantly 
            // better than looping from 0 to (count) and testing each individual bit, as you 
            // would have to do when using std::bitset as of this writing.
            //
            for (uint32_t i = 0; i < undershoot_cc; i++) {
               auto chunk = this->data[i];
               if (chunk != all_bits_set) {
                  for (uint8_t j = 0; j < bits_per_chunk; j++) {
                     if ((chunk & (1 << j)) == 0) {
                        return i * bits_per_chunk + j;
                     }
                  }
               }
            }
            if (has_partial_chunk) {
               //
               // If the number of bits in the set isn't cleanly divisible by 32, then we're 
               // going to have a final chunk that only uses some of its bits. We need to ONLY 
               // LOOK AT THE BITS THAT THAT CHUNK ACTUALLY USES, or we'll end up returning bit 
               // indices past the end of our set.
               //
               auto chunk = this->data[chunk_count - 1];
               if (chunk != partial_chunk_max) {
                  #pragma warning(suppress: 6294) // Initial condition in for-loop does not satisfy test. Normal if a bitmask has no partial chunk.
                  for (uint8_t j = 0; j < bits_in_partial; j++) {
                     if ((chunk & (1 << j)) == 0) {
                        return (chunk_count - 1) * bits_per_chunk + j;
                     }
                  }
               }
            }
            return -1;
         }
         int32_t find_first_clear_from(uint32_t index) const noexcept {
            //
            // Finds the first zero bit in the set. This function performs significantly 
            // better than looping from 0 to (count) and testing each individual bit, as you 
            // would have to do when using std::bitset as of this writing.
            //
            uint32_t ci = index / bits_per_chunk; // chunks to skip
            uint32_t bi = index % bits_per_chunk; // bits   to skip
            for (uint32_t i = ci; i < undershoot_cc; i++) {
               auto chunk = this->data[i];
               if (chunk != all_bits_set) {
                  for (uint8_t j = bi; j < bits_per_chunk; j++) {
                     if ((chunk & (1 << j)) == 0) {
                        return i * bits_per_chunk + j;
                     }
                  }
               }
               bi = 0; // only skip bits in the first chunk we look at
            }
            if (has_partial_chunk) {
               index -= chunk_count * undershoot_cc;
               //
               // If the number of bits in the set isn't cleanly divisible by 32, then we're 
               // going to have a final chunk that only uses some of its bits. We need to ONLY 
               // LOOK AT THE BITS THAT THAT CHUNK ACTUALLY USES, or we'll end up returning bit 
               // indices past the end of our set.
               //
               auto chunk = this->data[chunk_count - 1];
               if (chunk != partial_chunk_max) {
                  #pragma warning(suppress: 6294) // Initial condition in for-loop does not satisfy test. Normal if a bitmask has no partial chunk.
                  for (uint8_t j = index; j < bits_in_partial; j++) {
                     if ((chunk & (1 << j)) == 0) {
                        return (chunk_count - 1) * bits_per_chunk + j;
                     }
                  }
               }
            }
            return -1;
         }
         //
         void remove(uint32_t index) noexcept {
            //
            // Let's pretend for a second we're using 8-bit chunks. The indices are laid out like this:
            //
            //    76543210 FEDCBA98
            //
            // If we want to remove bit 5, then our goal is to produce this result:
            //
            //    87643210 xFEDCBA9
            //
            // We'll take care of this as follows:
            //
            //  - Use a test-and-modify loop for the bits of the first chunk we need to edit, because 
            //    right now I can't work out in my head how to "cut" a DWORD in two at an arbitrary bit.
            //
            //  - Use bitwise operators to deal with all subsequent chunks, if any.
            //
            uint32_t cap = index - (index % bits_per_chunk) + bits_per_chunk;
            if (cap >= count) {
               cap = count;
               for (; index + 1 < cap; ++index)
                  this->modify(index, this->test(index + 1));
               this->reset(index);
               return;
            }
            //
            uint32_t ci = index / bits_per_chunk;
            for (; index + 1 < cap; ++index) // 76543210 FEDCBA98 -> remove 5 -> 77643210 FEDCBA98
               this->modify(index, this->test(index + 1));
            this->modify(index, this->test(index + 1)); // 76543210 FEDCBA98 -> remove 5 -> 87643210 FEDCBA98
            ++ci;
            for (; ci + 1 < chunk_count; ++ci) {
               this->data[ci] >>= 1;
               this->data[ci] |= (this->data[ci + 1] & 1) ? 1 : 0;
            }
         }
         void swap_bits(uint32_t a, uint32_t b) noexcept {
            bool x = this->test(a);
            bool y = this->test(b);
            this->modify(a, y);
            this->modify(b, x);
         }
   };
};