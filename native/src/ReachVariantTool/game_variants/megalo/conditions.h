#pragma once
#include <cstdint>
#include <functional>
#include <vector>
#include "limits.h"
#include "value_types.h"

struct MegaloOpcodeBaseArgumentFlags {
   MegaloOpcodeBaseArgumentFlags() = delete;
   enum {
      none            = 0,
      is_out_variable = 1, // actions don't "return" a value; rather, you specify an out-variable as one of their arguments
   };
};
struct MegaloOpcodeBaseArgument { // for conditions or actions
   const char* name  = "";
   const MegaloValueType* type; // can't be a reference because MegaloValueTypes are constexpr; it would have to be a const reference, and then std::vector can't instantiate
   uint32_t    flags = 0; // MegaloOpcodeBaseArgumentFlags
   //
   MegaloOpcodeBaseArgument(const char* n, const MegaloValueType& t, uint32_t f = 0) : name(n), type(&t), flags(f) {};
   //
   void to_string(std::string& out, const MegaloValue& value) const noexcept;
};

class MegaloConditionFunction {
   public:
      const char* name;
      const char* desc;
      const char* verb_yes = "is";     // for normal   conditions
      const char* verb_no  = "is not"; // for inverted conditions
      const char* format;
      //
      std::vector<MegaloOpcodeBaseArgument> arguments;
      //
      MegaloConditionFunction(const char* c, const char* d, const char* f) : name(c), desc(d), format(f) {};
      //
      MegaloConditionFunction(const char* c, const char* d, const char* f, std::function<void(MegaloConditionFunction&)> init) : name(c), desc(d), format(f) {
         init(*this);
      };
      //
      MegaloConditionFunction(
         const char* c,
         const char* d,
         const char* f,
         const char* y,
         const char* n,
         std::function<void(MegaloConditionFunction&)> init
      ) : name(c), desc(d), format(f), verb_yes(y), verb_no(n) {
         init(*this);
      };
};

extern std::vector<MegaloConditionFunction> g_conditionFunctionList;

class MegaloCondition {
   public:
      #if _DEBUG
         std::string debug_stringified;
      #endif
      //
      const MegaloConditionFunction* function = nullptr;
      bool    inverted     = false;
      int32_t or_group     = -1; // 0 == none, or -1? // local to containing virtual trigger. all conditions with the same non-none (or_group) are OR-linked.
      int32_t child_action = -1; // used to group conditions with their actions into triggers
      std::vector<MegaloValue> arguments;
      //
      void to_string(std::string& out) const noexcept;
      void read(cobb::bitstream& stream) noexcept;
};