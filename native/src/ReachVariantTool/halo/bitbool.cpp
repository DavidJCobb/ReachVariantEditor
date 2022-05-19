#pragma once
#include "./bitbool.h"

namespace halo {
   bitbool& bitbool::operator=(const underlying_type other) {
      this->value = other;
      return *this;
   }
   bitbool& bitbool::operator|=(const underlying_type other) {
      this->value |= other;
      return *this;
   }
   bitbool& bitbool::operator&=(const underlying_type other) {
      this->value &= other;
      return *this;
   }
}