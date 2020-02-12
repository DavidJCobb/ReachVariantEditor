#include "all_constants.h"

namespace Megalo {
   #pragma region bool
   OpcodeArgTypeinfo OpcodeArgValueConstBool::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::default,
      0,
      &OpcodeArgValueConstBool::factory
   );
   //
   bool OpcodeArgValueConstBool::read(cobb::ibitreader& stream) noexcept {
      stream.read(this->value);
      return true;
   }
   void OpcodeArgValueConstBool::write(cobb::bitwriter& stream) const noexcept {
      stream.write(this->value);
   }
   void OpcodeArgValueConstBool::to_string(std::string& out) const noexcept {
      out = this->value ? this->baseStringTrue : this->baseStringFalse;
   }
   void OpcodeArgValueConstBool::configure_with_base(const OpcodeArgBase& base) noexcept {
      if (base.text_true)
         this->baseStringTrue = base.text_true;
      if (base.text_false)
         this->baseStringFalse = base.text_false;
   }
   #pragma endregion
   //
   #pragma region int8
   OpcodeArgTypeinfo OpcodeArgValueConstSInt8::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::default,
      0,
      &OpcodeArgValueConstSInt8::factory
   );
   //
   bool OpcodeArgValueConstSInt8::read(cobb::ibitreader& stream) noexcept {
      stream.read(this->value);
      return true;
   }
   void OpcodeArgValueConstSInt8::write(cobb::bitwriter& stream) const noexcept {
      stream.write(this->value);
   }
   void OpcodeArgValueConstSInt8::to_string(std::string& out) const noexcept {
      cobb::sprintf(out, "%d", this->value);
   }
   #pragma endregion
}