#include "object.h"
#include "number.h"
#include "player.h"
#include "team.h"
#include "../../compiler/compiler.h"

namespace {
   using namespace Megalo;
   using namespace Megalo::variable_scope_indicators::object;
   VariableScopeIndicatorValueList scopes = VariableScopeIndicatorValueList(OpcodeArgValueObject::typeinfo, Megalo::variable_type::object, {
      &global_object,
      &player_object,
      &object_object,
      &team_object,
      &player_biped,
      &player_player_biped,
      &object_player_biped,
      &team_player_biped,
   });
}
namespace Megalo {
   namespace variable_scope_indicators {
      namespace object {
         extern VariableScopeIndicatorValueList& as_list() {
            return scopes;
         }
         //
         extern VariableScopeIndicatorValue global_object       = VariableScopeIndicatorValue::make_variable_scope("%w",                  "%w",                      &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::none);
         extern VariableScopeIndicatorValue player_object       = VariableScopeIndicatorValue::make_variable_scope("%w.object[%i]",       "%w's object[%i]",         &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::object);
         extern VariableScopeIndicatorValue object_object       = VariableScopeIndicatorValue::make_variable_scope("%w.object[%i]",       "%w's object[%i]",         &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::object);
         extern VariableScopeIndicatorValue team_object         = VariableScopeIndicatorValue::make_variable_scope("%w.object[%i]",       "%w's object[%i]",         &MegaloVariableScopeTeam,   VariableScopeIndicatorValue::index_type::object);
         extern VariableScopeIndicatorValue player_biped        = VariableScopeIndicatorValue("%w.biped",            "%w's biped",              &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::none,   VariableScopeIndicatorValue::flags::is_readonly);
         extern VariableScopeIndicatorValue player_player_biped = VariableScopeIndicatorValue("%w.player[%i].biped", "%w's player[%i]'s biped", &MegaloVariableScopePlayer, VariableScopeIndicatorValue::index_type::player, VariableScopeIndicatorValue::flags::is_readonly);
         extern VariableScopeIndicatorValue object_player_biped = VariableScopeIndicatorValue("%w.player[%i].biped", "%w's player[%i]'s biped", &MegaloVariableScopeObject, VariableScopeIndicatorValue::index_type::player, VariableScopeIndicatorValue::flags::is_readonly);
         extern VariableScopeIndicatorValue team_player_biped   = VariableScopeIndicatorValue("%w.player[%i].biped", "%w's player[%i]'s biped", &MegaloVariableScopeTeam,   VariableScopeIndicatorValue::index_type::player, VariableScopeIndicatorValue::flags::is_readonly);
      }
   }
   OpcodeArgValueObject::OpcodeArgValueObject() : Variable(scopes) {}
   OpcodeArgTypeinfo OpcodeArgValueObject::typeinfo = OpcodeArgTypeinfo(
      "object",
      "Object",
      "A physical object existing in the game world.",
      //
      OpcodeArgTypeinfo::flags::is_variable | OpcodeArgTypeinfo::flags::can_hold_variables,
      OpcodeArgTypeinfo::default_factory<OpcodeArgValueObject>,
      {
         Script::Property("spawn_sequence", OpcodeArgValueScalar::typeinfo, &Megalo::variable_scope_indicators::number::spawn_sequence),
         Script::Property("team",           OpcodeArgValueTeam::typeinfo,   &Megalo::variable_scope_indicators::team::object_owner_team),
      },
      0, // static count
      8  // temporary count
   ).set_variable_which_values(
      &variable_which_values::object::global_0,
      nullptr,
      &variable_which_values::object::temporary_0
   );
   //
   arg_compile_result OpcodeArgValueObject::compile(Compiler& compiler, Script::VariableReference& arg, uint8_t part) noexcept {
      auto result = Variable::compile(compiler, arg, part);
      //
      if (result.code == arg_compile_result::code_t::base_class_is_expecting_override_behavior) {
         if (arg.is_property()) {
            auto& res  = arg.resolved;
            auto  prop = res.property.definition;
            if (cobb::strieq(prop->name, "biped")) {
               //
               // The property in question is the "biped" property, which requires special handling.
               //
               auto& top = res.top_level;
               //
               if (auto nested = arg.resolved.nested.type) {
                  //
                  // The property is being accessed on a nested variable, i.e. (var.var.biped). Ensure 
                  // that the nested variable is a player (if it isn't, then this is some other "biped" 
                  // property and not the one we intend to handle).
                  //
                  if (nested != &OpcodeArgValuePlayer::typeinfo)
                     return arg_compile_result::failure();
                  //
                  auto type = top.type;
                  if (top.namespace_member.which)
                     this->which = top.namespace_member.which->as_integer();
                  else
                     this->which = Variable::_global_index_to_which(*type, top.index, top.is_static);
                  this->index = res.nested.index;
                  //
                  if (type == &OpcodeArgValueObject::typeinfo)
                     this->scope = &variable_scope_indicators::object::object_player_biped;
                  else if (type == &OpcodeArgValuePlayer::typeinfo)
                     this->scope = &variable_scope_indicators::object::player_player_biped;
                  else if (type == &OpcodeArgValueTeam::typeinfo)
                     this->scope = &variable_scope_indicators::object::team_player_biped;
                  else
                     return arg_compile_result::failure();
                  //
                  return arg_compile_result::success();
               }
               //
               // The property is being accessed on a global variable. Ensure that the global variable 
               // is a player (if it isn't, then this is some other "biped" property and not the one we 
               // intend to handle).
               //
               if (top.type != &OpcodeArgValuePlayer::typeinfo)
                  return arg_compile_result::failure();
               this->scope = &variable_scope_indicators::object::player_biped;
               if (top.namespace_member.which)
                  this->which = top.namespace_member.which->as_integer();
               else {
                  if (top.is_temporary)
                     this->which = Variable::_temporary_index_to_which(*top.type, top.index);
                  else
                     this->which = Variable::_global_index_to_which(*top.type, top.index, top.is_static);
               }
               return arg_compile_result::success();
            }
         }
         return arg_compile_result::failure();
      }
      return result;
   }
   //
   Variable* OpcodeArgValueObject::create_zero_or_none() const noexcept {
      auto arg = new OpcodeArgValueObject;
      arg->scope = &variable_scope_indicators::object::global_object;
      arg->which = variable_which_values::object::no_object.as_integer();
      arg->index = 0;
      return arg;
   }
   bool OpcodeArgValueObject::set_to_zero_or_none() noexcept {
      this->scope = &variable_scope_indicators::object::global_object;
      this->which = variable_which_values::object::no_object.as_integer();
      this->index = 0;
      return true;
   }
}