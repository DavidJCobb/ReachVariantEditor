#pragma once
#include <cstdint>
#include <functional>
#include <vector>
#include "value_types.h"

struct MegaloOpcodeBaseArgumentFlags {
   MegaloOpcodeBaseArgumentFlags() = delete;
   enum {
      none            = 0,
      is_out_variable = 1, // actions don't "return" a value; rather, you specify an out-variable as one of their arguments
   };
};
struct MegaloOpcodeBaseArgument { // for conditions or actions
   const char*      name;
   MegaloValueType& type;
   uint32_t         flags = 0;
   //
   MegaloOpcodeBaseArgument(const char* n, MegaloValueType& t, uint32_t f = 0) : name(n), type(t), flags(f) {};
};

class MegaloConditionFunction {
   public:
      const char* name;
      const char* desc;
      const char* verb_yes = "is";     // for normal   conditions
      const char* verb_no  = "is not"; // for inverted conditions
      const char* format;
      //
      std::vector<MegaloOpcodeBaseArgumentFlags> arguments;
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

struct MegaloConditionFunctionList {
   MegaloConditionFunctionList();
   ~MegaloConditionFunctionList();
   //
   MegaloConditionFunction* list = nullptr;
   uint32_t size;
   //
   inline const MegaloConditionFunction& operator[](int v) const noexcept {
      return this->list[v];
   }
   //
   inline static const MegaloConditionFunctionList& get() {
      static MegaloConditionFunctionList instance;
      return instance;
   }
};