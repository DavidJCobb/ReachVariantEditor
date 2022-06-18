#pragma once
#include "./operand.h"

namespace halo::reach::megalo {
   extern operand* create_operand(const operand_typeinfo& info);

   extern const operand_typeinfo& typeinfo_of(const operand&);

   extern operand* clone_operand(const operand&);
}