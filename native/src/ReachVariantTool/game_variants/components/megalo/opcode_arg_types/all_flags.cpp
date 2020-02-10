#include "all_flags.h"
#include "../enums.h"

namespace {
   namespace _MegaloArgValueFlagsBase {
      megalo_define_smart_flags(CreateObject,
         "never garbage-collect",
         "unk_1",
         "unk_2"
      );
      megalo_define_smart_flags(KillerType,
         "guardians",
         "suicide",
         "kill",
         "betrayal",
         "quit"
      );
      megalo_define_smart_flags(PlayerUnusedMode,
         "unk_0",
         "unk_1",
         "unk_2",
         "unk_3",
      );
   }
}
namespace Megalo {
   OpcodeArgValueCreateObjectFlags::OpcodeArgValueCreateObjectFlags()
      : OpcodeArgValueBaseFlags(_MegaloArgValueFlagsBase::CreateObject)
   {}
   //
   OpcodeArgValueKillerTypeFlags::OpcodeArgValueKillerTypeFlags()
      : OpcodeArgValueBaseFlags(_MegaloArgValueFlagsBase::KillerType)
   {}
   //
   OpcodeArgValuePlayerUnusedModeFlags::OpcodeArgValuePlayerUnusedModeFlags()
      : OpcodeArgValueBaseFlags(_MegaloArgValueFlagsBase::PlayerUnusedMode)
   {}
   //
   bool OpcodeArgValueGenericFlagsMask::read(cobb::ibitreader & stream) noexcept {
      auto& list = this->typeinfo.elements;
      this->value = stream.read_bits(cobb::bitcount(list.size()));
      return true;
   }
   void OpcodeArgValueGenericFlagsMask::write(cobb::bitwriter & stream) const noexcept {
      auto& list = this->typeinfo.elements;
      stream.write(this->value, cobb::bitcount(list.size()));
   }
   void OpcodeArgValueGenericFlagsMask::to_string(std::string & out) const noexcept {
      auto& list = this->typeinfo.elements;
      auto  size = list.size();
      //
      out.clear();
      size_t i     = 0;
      size_t found = 0;
      for (; i < size; i++) {
         if (value & (1 << i)) {
            if (++found == 1)
               out = '(' + out;
            else if (found > 1)
               out += ", ";
            out += list[i];
         }
      }
      if (found > 1)
         out += ')';
   }
   void OpcodeArgValueGenericFlagsMask::decompile(Decompiler& out, uint64_t flags) noexcept {
      std::string s;
      this->to_string(s);
      out.write(s);
   }
   //
   namespace { // factories
      template<OpcodeArgTypeinfo& ti> OpcodeArgValue* _flagsMaskFactory(cobb::ibitreader& stream) {
         return new OpcodeArgValueGenericFlagsMask(ti);
      }
   }
   OpcodeArgTypeinfo OpcodeArgValueFlagsMaskTypeinfoCreateObject = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::flags_mask,
      0,
      {
         "never_garbage_collect",
         "unk_1",
         "unk_2"
      },
      &_flagsMaskFactory<OpcodeArgValueFlagsMaskTypeinfoCreateObject>
   );
   OpcodeArgTypeinfo OpcodeArgValueFlagsMaskTypeinfoKillerType = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::flags_mask,
      0,
      {
         "guardians",
         "suicide",
         "kill",
         "betrayal",
         "quit"
      },
      &_flagsMaskFactory<OpcodeArgValueFlagsMaskTypeinfoKillerType>
   );
   OpcodeArgTypeinfo OpcodeArgValueFlagsMaskTypeinfoPlayerUnusedMode = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::flags_mask,
      0,
      {
         "unk_0",
         "unk_1",
         "unk_2",
         "unk_3",
      },
      &_flagsMaskFactory<OpcodeArgValueFlagsMaskTypeinfoPlayerUnusedMode>
   );
}