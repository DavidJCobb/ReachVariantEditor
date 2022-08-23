#pragma once
#include <bit>
#include "halo/reach/bitstreams.fwd.h"
#include "../limits.h"
#include "../opcode.h"
#include "../opcode_function.h"
#include "./index_types.h"

namespace halo::reach::megalo {
   class condition_function : public opcode_function {};

   class condition : public opcode {
      public:
         bitbool invert = false;
         condition_index or_group = 0; // see documentation ("how Megalo executes a trigger.txt"). WARNING: game truncates 9-bit value to 8 bits on load!
         mutable struct {
            //
            // Data used during serialization.
            //
            action_index execute_before = 0; // see documentation ("how Megalo executes a trigger.txt"). WARNING: game truncates 10-bit value to 8 bits on load!
         } load_state;

         virtual void read(bitreader&) override;
         virtual void write(bitwriter&) const override;

         condition() {}
         condition(const condition& o) = delete;
         condition(condition&& o) noexcept { *this = std::forward<condition&&>(o); }

         condition& operator=(const condition& o) = delete;
         condition& operator=(condition&& o) noexcept {
            std::swap(this->invert,     o.invert);
            std::swap(this->or_group,   o.or_group);
            std::swap(this->load_state, o.load_state);
            opcode::operator=(std::forward<opcode&&>(o));
            return *this;
         }

         condition* clone() const;
   };
}