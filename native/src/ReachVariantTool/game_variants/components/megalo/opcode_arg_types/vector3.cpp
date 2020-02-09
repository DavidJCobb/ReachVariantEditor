#include "vector3.h"

namespace Megalo {
   OpcodeArgTypeinfo OpcodeArgValueVector3::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::default,
      0,
      &OpcodeArgValueVector3::factory
   );
}