#pragma once
#include "../opcode_arg.h"
#include "../limits.h"
#include "../forge_label.h"
#include "../../../types/multiplayer.h"

namespace Megalo {
   class OpcodeArgValueForgeLabel : public OpcodeArgValue {
      public:
         static constexpr int16_t index_of_none = -1;
         static constexpr int16_t max_index     = Limits::max_script_labels;
         //
         int16_t              index = index_of_none;
         ref<ReachForgeLabel> value = decltype(value)::make(*this);
         bool postprocessed = false;
         //
         static OpcodeArgValue* factory(cobb::ibitreader& stream) {
            return new OpcodeArgValueForgeLabel;
         }
         virtual bool read(cobb::ibitreader& stream) noexcept override {
            if (stream.read_bits(1) != 0) { // absence bit
               this->index = index_of_none;
               return true;
            }
            this->index = stream.read_bits(cobb::bitcount(max_index - 1));
            return true;
         }
         virtual void write(cobb::bitwriter& stream) const noexcept override {
            if (!this->value) {
               stream.write(1, 1); // absence bit
               return;
            }
            stream.write(0, 1); // absence bit
            stream.write(this->value->index, cobb::bitcount(max_index - 1));
         }
         virtual void postprocess(GameVariantDataMultiplayer* newlyLoaded) noexcept override {
            this->postprocessed = true;
            if (this->index == index_of_none)
               return;
            auto& list = newlyLoaded->scriptContent.forgeLabels;
            if (this->index >= list.size())
               return;
            this->value = list[this->index].get();
         }
         //
         virtual void to_string(std::string& out) const noexcept override {
            if (!this->postprocessed) {
               if (this->index == index_of_none) {
                  out = "no label";
                  return;
               }
               cobb::sprintf(out, "label index %u", this->index);
               return;
            }
            if (!this->value) {
               if (this->index == index_of_none) {
                  out = "no label";
                  return;
               }
               cobb::sprintf(out, "invalid label index %u", this->index);
               return;
            }
            ReachForgeLabel* f = this->value;
            if (!f->name) {
               cobb::sprintf(out, "label index %u", this->index);
               return;
            }
            out = f->name->english();
         }
   };
}
