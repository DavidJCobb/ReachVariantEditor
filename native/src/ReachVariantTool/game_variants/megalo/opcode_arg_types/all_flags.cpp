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
}