#include "any_variable.h"
#include "all_core.h"
#include "../../../../errors.h"
#include "../../compiler/compiler.h"

namespace Megalo {
   OpcodeArgTypeinfo OpcodeArgValueAnyVariable::typeinfo = OpcodeArgTypeinfo(
      "_any_variable",
      "Any Variable",
      "A number, object, player, team, or timer can appear here.",
      //
      OpcodeArgTypeinfo::flags::none, // don't flag this as a variable, because you can't write an "any" variable in a script
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueAnyVariable>
   );
   //
   OpcodeArgValueAnyVariable::~OpcodeArgValueAnyVariable() {
      if (this->variable) {
         delete this->variable;
         this->variable = nullptr;
      }
   }
   bool OpcodeArgValueAnyVariable::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
      uint8_t type = stream.read_bits(3);
      switch ((variable_type)type) {
         case variable_type::scalar: this->variable = new OpcodeArgValueScalar; break;
         case variable_type::timer:  this->variable = new OpcodeArgValueTimer;  break;
         case variable_type::team:   this->variable = new OpcodeArgValueTeam;   break;
         case variable_type::player: this->variable = new OpcodeArgValuePlayer; break;
         case variable_type::object: this->variable = new OpcodeArgValueObject; break;
      }
      if (this->variable)
         return this->variable->read(stream, mp);
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
   arg_compile_result OpcodeArgValueAnyVariable::compile(Compiler& compiler, Script::string_scanner& arg_text, uint8_t part) noexcept {
      auto arg = compiler.arg_to_variable(arg_text);
      if (!arg)
         return arg_compile_result::failure("This argument is not a variable.");
      auto result = this->compile(compiler, *arg, part);
      delete arg;
      return result;
   }
   arg_compile_result OpcodeArgValueAnyVariable::compile(Compiler& compiler, Script::VariableReference& arg, uint8_t part) noexcept {
      auto type = arg.get_type();
      if (type == &OpcodeArgValueScalar::typeinfo) {
         this->variable = new OpcodeArgValueScalar;
      } else if (type == &OpcodeArgValueObject::typeinfo) {
         this->variable = new OpcodeArgValueObject;
      } else if (type == &OpcodeArgValuePlayer::typeinfo) {
         this->variable = new OpcodeArgValuePlayer;
      } else if (type == &OpcodeArgValueTeam::typeinfo) {
         this->variable = new OpcodeArgValueTeam;
      } else if (type == &OpcodeArgValueTimer::typeinfo) {
         this->variable = new OpcodeArgValueTimer;
      } else {
         return arg_compile_result::failure("This argument is not a variable.");
      }
      return this->variable->compile(compiler, arg, part);
   }
   void OpcodeArgValueAnyVariable::copy(const OpcodeArgValue* other) noexcept {
      auto cast = dynamic_cast<const OpcodeArgValueAnyVariable*>(other);
      assert(cast);
      if (this->variable)
         delete this->variable;
      if (cast->variable)
         this->variable = (Variable*)cast->variable->clone();
      else
         this->variable = nullptr;
   }
   //
   variable_type OpcodeArgValueAnyVariable::get_variable_type() const noexcept {
      if (!this->variable)
         return variable_type::not_a_variable;
      return this->variable->get_variable_type();
   }
   OpcodeArgValueAnyVariable* OpcodeArgValueAnyVariable::create_zero_or_none() const noexcept {
      if (!this->variable)
         return nullptr;
      auto wrapped = this->variable->create_zero_or_none();
      auto wrapper = new OpcodeArgValueAnyVariable;
      wrapper->variable = wrapped;
      return wrapper;
   }
}