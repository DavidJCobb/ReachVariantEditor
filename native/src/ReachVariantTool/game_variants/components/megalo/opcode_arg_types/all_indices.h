#pragma once
#include "../opcode_arg.h"
#include "../limits.h"

namespace Megalo {
   class OpcodeArgValueBaseIndex : public OpcodeArgValue {
      public:
         static constexpr int32_t none = -1;
         enum class index_quirk {
            none,
            presence, // index value is preceded by an "is none" bit
            reference,
            offset,
            word,
         };
         //
      public:
         OpcodeArgValueBaseIndex(const char* name, uint32_t max, index_quirk quirk = index_quirk::none) : 
            name(name), max(max), quirk(quirk)
         {};

         const char* name;
         uint32_t    max;
         index_quirk quirk;
         int32_t     value = 0; // loaded value
         //
         virtual bool read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         virtual void decompile(Decompiler& out, uint64_t flags = 0) noexcept override;
         virtual arg_compile_result compile(Compiler&, Script::string_scanner&, uint8_t part) noexcept override;
         virtual void copy(const OpcodeArgValue*) noexcept override;
   };

   class OpcodeArgValueRequisitionPalette : public OpcodeArgValueBaseIndex { // development leftover; later used in Halo 4?
      megalo_opcode_arg_value_make_create_override;
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
         OpcodeArgValueRequisitionPalette() : OpcodeArgValueBaseIndex("Requisition Palette", cobb::bitmax(4), index_quirk::presence) {}
   };
   class OpcodeArgValueTrigger : public OpcodeArgValueBaseIndex {
      megalo_opcode_arg_value_make_create_override;
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
         OpcodeArgValueTrigger() : OpcodeArgValueBaseIndex("Trigger", Limits::max_triggers, index_quirk::reference) {}
   };
}