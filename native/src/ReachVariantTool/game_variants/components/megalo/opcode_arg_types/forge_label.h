#pragma once
#include "../opcode_arg.h"
#include "../limits.h"
#include "../forge_label.h"
#include "../../../types/multiplayer.h"

namespace Megalo {
   class OpcodeArgValueForgeLabel : public OpcodeArgValue {
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
      public:
         static constexpr int16_t index_of_none = -1;
         static constexpr int16_t max_index     = Limits::max_script_labels;
         //
         int16_t index = index_of_none;
         cobb::refcount_ptr<ReachForgeLabel> value;
         bool postprocessed = false;
         //
         virtual bool read(cobb::ibitreader& stream) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void postprocess(GameVariantDataMultiplayer* newlyLoaded) noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         virtual void decompile(Decompiler& out, uint64_t flags = 0) noexcept override;
   };
}
