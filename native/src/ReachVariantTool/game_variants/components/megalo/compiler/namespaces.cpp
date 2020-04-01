#include "namespaces.h"
#include "../opcode_arg_types/variables/number.h"
#include "../opcode_arg_types/variables/object.h"
#include "../opcode_arg_types/variables/timer.h"

namespace Megalo {
   namespace Script {
      namespace namespaces {
         Namespace unnamed = Namespace("game", true, false, {
            NamespaceMember("no_object", OpcodeArgValueObject::typeinfo, 0, NamespaceMember::no_scope, NamespaceMember::flag::is_none), // TODO: replace 0 with some reference to the "no_object" value in (megalo_objects)
            //
            // TODO: FINISH ME
            //
         });
         Namespace global = Namespace("global", false, true);
         Namespace game = Namespace("game", false, false, {
            NamespaceMember("betrayal_booting", OpcodeArgValueScalar::typeinfo, NamespaceMember::no_which, OpcodeArgValueScalar::scope_indicator_id::betrayal_booting),
            NamespaceMember("betrayal_penalty", OpcodeArgValueScalar::typeinfo, NamespaceMember::no_which, OpcodeArgValueScalar::scope_indicator_id::betrayal_penalty),
            NamespaceMember("current_round",    OpcodeArgValueScalar::typeinfo, NamespaceMember::no_which, OpcodeArgValueScalar::scope_indicator_id::current_round),
            //
            // TODO: FINISH ME
            //
         });
      }
   }
}