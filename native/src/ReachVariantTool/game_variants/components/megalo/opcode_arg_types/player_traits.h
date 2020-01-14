#pragma once
#include "../opcode_arg.h"
#include "../limits.h"
#include "../../player_traits.h"
#include "../../../types/multiplayer.h"

namespace Megalo {
   class OpcodeArgValuePlayerTraits : public OpcodeArgValue {
      public:
         static constexpr int16_t max_index = Limits::max_script_traits;
         //
         int16_t                      index = 0;
         ref<ReachMegaloPlayerTraits> value = decltype(value)::make(*this);
         bool postprocessed = false;
         //
         static OpcodeArgValue* factory(cobb::ibitreader& stream) {
            return new OpcodeArgValuePlayerTraits;
         }
         virtual bool read(cobb::ibitreader& stream) noexcept override {
            this->index = stream.read_bits(cobb::bitcount(max_index - 1));
            return true;
         }
         virtual void write(cobb::bitwriter& stream) const noexcept override {
            if (!this->value) {
               stream.write(0, cobb::bitcount(max_index - 1));
               return;
            }
            stream.write(this->value->index, cobb::bitcount(max_index - 1));
         }
         virtual void postprocess(GameVariantDataMultiplayer* newlyLoaded) noexcept override {
            this->postprocessed = true;
            auto& list = newlyLoaded->scriptData.traits;
            if (this->index >= list.size())
               return;
            this->value = &list[this->index];
         }
         //
         virtual void to_string(std::string& out) const noexcept override {
            if (!this->postprocessed) {
               cobb::sprintf(out, "script traits %u", this->index);
               return;
            }
            if (!this->value) {
               cobb::sprintf(out, "invalid script traits %u", this->index);
               return;
            }
            ReachMegaloPlayerTraits* f = this->value;
            if (!f->name) {
               cobb::sprintf(out, "script traits %u", this->index);
               return;
            }
            out = f->name->english();
         }
   };
}
