#pragma once
#include "../opcode_arg.h"
#include "../limits.h"
#include "../forge_label.h"
#include "../../../types/multiplayer.h"

namespace Megalo {
   class OpcodeArgValueForgeLabel : public OpcodeArgValue {
      public:
         static OpcodeArgTypeinfo typeinfo;
         static OpcodeArgValue* factory(cobb::ibitreader& stream) {
            return new OpcodeArgValueForgeLabel;
         }
         //
      public:
         static constexpr int16_t index_of_none = -1;
         static constexpr int16_t max_index     = Limits::max_script_labels;
         //
         int16_t              index = index_of_none;
         ref<ReachForgeLabel> value = decltype(value)::make(*this);
         bool postprocessed = false;
         //
         virtual bool read(cobb::ibitreader& stream) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void postprocess(GameVariantDataMultiplayer* newlyLoaded) noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
   };
}
