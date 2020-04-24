#include "specific_variable.h"
#include "variables/object.h"
#include "variables/player.h"
#include "variables/timer.h"
#include "../compiler/compiler.h"

namespace Megalo {
   #pragma region OpcodeArgValueObjectTimerVariable
      OpcodeArgTypeinfo OpcodeArgValueObjectTimerVariable::typeinfo = OpcodeArgTypeinfo(
         "_object_timer_variable",
         "Object Timer Variable Index",
         "A number indicating one of the timer variables on an object.",
         //
         OpcodeArgTypeinfo::flags::none,
         OpcodeArgTypeinfo::default_factory<OpcodeArgValueObjectTimerVariable>
      );
      //
      bool OpcodeArgValueObjectTimerVariable::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
         bool absence = stream.read_bits(1) != 0;
         if (absence)
            return true;
         auto scope = getScopeObjectForConstant(variable_scope::object);
         this->index = stream.read_bits(scope.index_bits(variable_type::timer));
         return true;
      }
      void OpcodeArgValueObjectTimerVariable::write(cobb::bitwriter& stream) const noexcept {
         if (this->index == -1) {
            stream.write(1, 1);
            return;
         }
         stream.write(0, 1);
         auto scope = getScopeObjectForConstant(variable_scope::object);
         stream.write(this->index, scope.index_bits(variable_type::timer));
      }
      void OpcodeArgValueObjectTimerVariable::to_string(std::string& out) const noexcept {
         const char* owner = "object";
         const char* type  = "timer";
         if (this->index == -1) {
            cobb::sprintf(out, "its %s[None] variable", owner, type);
            return;
         }
         cobb::sprintf(out, "its %s[%u] variable", owner, type, this->index);
      }
      void OpcodeArgValueObjectTimerVariable::decompile(Decompiler& out, Decompiler::flags_t flags) noexcept {
         if (this->index < 0) {
            out.write("none");
            return;
         }
         std::string temp;
         cobb::sprintf(temp, "%u", this->index);
         out.write(temp);
      }
      arg_compile_result OpcodeArgValueObjectTimerVariable::compile(Compiler& compiler, Script::string_scanner& arg, uint8_t part) noexcept {
         int32_t index;
         if (!arg.extract_integer_literal(index)) {
            auto word = arg.extract_word();
            if (word.compare("none", Qt::CaseInsensitive) == 0) {
               this->index = -1;
               return arg_compile_result::success();
            }
            if (word.startsWith("object.", Qt::CaseInsensitive)) {
               //
               // Allow the use of a relative object.timer alias here.
               //
               word = word.right(word.size() - strlen("object."));
               if (word.isEmpty())
                  return arg_compile_result::failure();
               auto alias = compiler.lookup_relative_alias(word, &OpcodeArgValueObject::typeinfo);
               if (!alias)
                  return arg_compile_result::failure();
               auto target = alias->target;
               if (!target)
                  return arg_compile_result::failure();
               if (target->is_accessor() || target->is_property())
                  return arg_compile_result::failure();
               auto& nested = target->resolved.nested;
               if (nested.type != &OpcodeArgValueTimer::typeinfo)
                  return arg_compile_result::failure();
               index = nested.index;
            } else {
               //
               // Look for an absolute alias of an integer.
               //
               auto alias = compiler.lookup_absolute_alias(word);
               if (!alias || !alias->is_integer_constant())
                  return arg_compile_result::failure();
               index = alias->get_integer_constant();
            }
         }
         //
         // Validate the index:
         //
         auto max = MegaloVariableScopeObject.max_timers;
         if (index >= max)
            return arg_compile_result::failure(QString("You specified object.timer variable %1, but only %2 such variables can exist.").arg(index).arg(max - 1));
         //
         compiler.imply_variable(variable_scope::object, variable_type::timer, index);
         //
         this->index = index;
         return arg_compile_result::success();
      }
   #pragma endregion
   //
   #pragma region OpcodeArgValueObjectPlayerVariable
      OpcodeArgTypeinfo OpcodeArgValueObjectPlayerVariable::typeinfo = OpcodeArgTypeinfo(
         "_object_player_variable",
         "Object Player Variable",
         "An object-and-player reference.",
         //
         OpcodeArgTypeinfo::flags::none,
         OpcodeArgTypeinfo::default_factory<OpcodeArgValueObjectPlayerVariable>
      ).set_accessor_proxy_types({
         &OpcodeArgValueObject::typeinfo
      });
      //
      bool OpcodeArgValueObjectPlayerVariable::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
         if (!this->object.read(stream, mp))
            return false;
         bool absence = stream.read_bits(1) != 0;
         if (absence)
            return true;
         this->playerIndex = stream.read_bits(MegaloVariableScopeObject.index_bits(variable_type::player));
         return true;
      }
      void OpcodeArgValueObjectPlayerVariable::write(cobb::bitwriter& stream) const noexcept {
         this->object.write(stream);
         if (this->playerIndex == -1) {
            stream.write(1, 1);
            return;
         }
         stream.write(0, 1);
         stream.write(this->playerIndex, MegaloVariableScopeObject.index_bits(variable_type::player));
      }
      void OpcodeArgValueObjectPlayerVariable::to_string(std::string& out) const noexcept {
         this->object.to_string(out);
         if (this->playerIndex == -1) {
            out += " - no/all? players";
            return;
         }
         cobb::sprintf(out, "%s.Player[%d]", out.c_str(), this->playerIndex);
      }
      void OpcodeArgValueObjectPlayerVariable::decompile(Decompiler& out, Decompiler::flags_t flags) noexcept {
         if (flags & Decompiler::flags::is_call_context) {
            this->object.decompile(out);
            return;
         }
         if (this->playerIndex < 0) {
            out.write("none");
            return;
         }
         std::string temp;
         cobb::sprintf(temp, "%u", this->playerIndex);
         out.write(temp);
      }
      arg_compile_result OpcodeArgValueObjectPlayerVariable::compile(Compiler& compiler, Script::string_scanner& arg, uint8_t part) noexcept {
         int32_t index;
         if (!arg.extract_integer_literal(index)) {
            auto word = arg.extract_word();
            if (word.startsWith("object.", Qt::CaseInsensitive)) {
               //
               // Allow the use of a relative object.player alias here.
               //
               word = word.right(word.size() - strlen("object."));
               if (word.isEmpty())
                  return arg_compile_result::failure();
               auto alias = compiler.lookup_relative_alias(word, &OpcodeArgValueObject::typeinfo);
               if (!alias)
                  return arg_compile_result::failure();
               auto target = alias->target;
               if (!target)
                  return arg_compile_result::failure();
               if (target->is_accessor() || target->is_property())
                  return arg_compile_result::failure();
               auto& nested = target->resolved.nested;
               if (nested.type != &OpcodeArgValuePlayer::typeinfo)
                  return arg_compile_result::failure();
               index = nested.index;
            } else {
               //
               // Look for an absolute alias of an integer.
               //
               auto alias = compiler.lookup_absolute_alias(word);
               if (!alias || !alias->is_integer_constant())
                  return arg_compile_result::failure();
               index = alias->get_integer_constant();
            }
         }
         //
         // Validate the index:
         //
         auto max = MegaloVariableScopeObject.max_players;
         if (index >= max)
            return arg_compile_result::failure(QString("You specified object.player variable %1, but only %2 such variables can exist.").arg(index).arg(max - 1));
         //
         compiler.imply_variable(variable_scope::object, variable_type::player, index);
         //
         this->playerIndex = index;
         return arg_compile_result::success();
      }
      arg_compile_result OpcodeArgValueObjectPlayerVariable::compile(Compiler& compiler, Script::VariableReference& arg, uint8_t part) noexcept {
         return this->object.compile(compiler, arg, 0);
      }
   #pragma endregion
}