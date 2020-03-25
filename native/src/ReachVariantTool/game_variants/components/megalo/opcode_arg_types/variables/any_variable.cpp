#include "any_variable.h"
#include "number.h"
#include "object.h"
#include "player.h"
#include "team.h"
#include "timer.h"
#include "../../../../errors.h"

namespace Megalo {
   OpcodeArgTypeinfo OpcodeArgValueAnyVariable::typeinfo = OpcodeArgTypeinfo(
      OpcodeArgTypeinfo::typeinfo_type::default,
      0,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueAnyVariable>
   );
   //
   bool OpcodeArgValueAnyVariable::read(cobb::ibitreader& stream) noexcept {
      uint8_t type = stream.read_bits(3);
      switch ((variable_type)type) {
         case variable_type::scalar: this->variable = new OpcodeArgValueScalar; break;
         case variable_type::timer:  this->variable = new OpcodeArgValueTimer;  break;
         case variable_type::team:   this->variable = new OpcodeArgValueTeam;   break;
         case variable_type::player: this->variable = new OpcodeArgValuePlayer; break;
         case variable_type::object: this->variable = new OpcodeArgValueObject; break;
      }
      if (this->variable)
         return this->variable->read(stream);
      auto& error = GameEngineVariantLoadError::get();
      error.state  = GameEngineVariantLoadError::load_state::failure;
      error.reason = GameEngineVariantLoadError::load_failure_reason::bad_script_opcode_argument;
      error.detail = GameEngineVariantLoadError::load_failure_detail::bad_opcode_variable_type;
      error.extra[0] = type;
      return false;
   }
   void OpcodeArgValueAnyVariable::write(cobb::bitwriter& stream) const noexcept {
      assert(this->variable && "Cannot save an any-variable when there is no variable.");
      auto type = this->variable->get_variable_type();
      assert(type != variable_type::not_a_variable && "Bad any-variable type.");
      stream.write((uint8_t)type, 3);
      this->variable->write(stream);
   }
   void OpcodeArgValueAnyVariable::to_string(std::string& out) const noexcept {
      if (this->variable)
         this->variable->to_string(out);
   }
   void OpcodeArgValueAnyVariable::decompile(Decompiler& out, Decompiler::flags_t flags) noexcept {
      if (this->variable)
         this->variable->decompile(out, flags);
   }
   //
   variable_type OpcodeArgValueAnyVariable::get_variable_type() const noexcept {
      if (!this->variable)
         return variable_type::not_a_variable;
      return this->variable->get_variable_type();
   }

}