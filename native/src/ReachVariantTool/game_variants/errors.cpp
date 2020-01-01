#include "errors.h"
#include <QObject>
#include "base.h"
#include "components/megalo/variables_and_scopes.h"

void GameEngineVariantLoadError::reset() {
   this->state            = load_state::incomplete;
   this->reason           = load_failure_reason::none;
   this->detail           = load_failure_detail::none;
   this->failure_point    = load_failure_point::none;
   this->failure_index    = -1;
   this->failure_subindex = -1;
   for (size_t i = 0; i < std::extent<decltype(this->extra)>::value; i++)
      this->extra[i] = 0;
}
QString GameEngineVariantLoadError::_explain_opcode_failure() const noexcept {
   constexpr char* tr_disambiguator = "GameEngineVariantLoadError";
   //
   QString noun_single;
   QString noun_plural;
   if (this->failure_point == load_failure_point::megalo_actions) {
      noun_single = QObject::tr("action",  tr_disambiguator);
      noun_plural = QObject::tr("actions", tr_disambiguator);
   } else {
      noun_single = QObject::tr("condition",  tr_disambiguator);
      noun_plural = QObject::tr("conditions", tr_disambiguator);
   }
   //
   QString result;
   if (this->reason == load_failure_reason::bad_script_opcode_argument) {
      result = QObject::tr("Failed to load %1 #%2 argument #%3. ", tr_disambiguator).arg(noun_single).arg(this->failure_index).arg(this->failure_subindex);
      switch (this->detail) {
         case load_failure_detail::bad_opcode_player_or_team_var_type:
            result += QObject::tr("When an argument can be a team, a variable, or \"all players,\" the argument value is prefixed with a number indicating which of those it is. This argument used an invalid number: %1.", tr_disambiguator).arg(this->extra[0]);
            break;
         case load_failure_detail::bad_opcode_variable_type:
            result += QObject::tr("When an argument can be a constant number or any variable, the argument value is prefixed with a number indicating precisely what it is. This argument used an invalid number: %1.", tr_disambiguator).arg(this->extra[1]);
            break;
         case load_failure_detail::bad_variable_subtype:
            {
               QString varType = QObject::tr("unknown-type variable", tr_disambiguator);
               switch ((Megalo::variable_type)this->extra[0]) {
                  case Megalo::variable_type::object: varType = QObject::tr("object variable", tr_disambiguator); break;
                  case Megalo::variable_type::player: varType = QObject::tr("player variable", tr_disambiguator); break;
                  case Megalo::variable_type::scalar: varType = QObject::tr("number variable", tr_disambiguator); break;
                  case Megalo::variable_type::timer:  varType = QObject::tr("timer variable",  tr_disambiguator); break;
                  case Megalo::variable_type::team:   varType = QObject::tr("team variable",   tr_disambiguator); break;
               }
               result += QObject::tr("When an argument is a %1, the value is prefixed with a \"subtype\" number indicating precisely what is being accessed. This argument used an invalid subtype: %2.", tr_disambiguator).arg(varType).arg(this->extra[1]);
            }
            break;
         case load_failure_detail::bad_variable_scope:
            {
               QString varType = QObject::tr("unknown-type variable", tr_disambiguator);
               switch ((Megalo::variable_type)this->extra[3]) {
                  case Megalo::variable_type::object: varType = QObject::tr("object variable", tr_disambiguator); break;
                  case Megalo::variable_type::player: varType = QObject::tr("player variable", tr_disambiguator); break;
                  case Megalo::variable_type::scalar: varType = QObject::tr("number variable", tr_disambiguator); break;
                  case Megalo::variable_type::timer:  varType = QObject::tr("timer variable",  tr_disambiguator); break;
                  case Megalo::variable_type::team:   varType = QObject::tr("team variable",   tr_disambiguator); break;
               }
               QString scopeType = QObject::tr("unknown scope", tr_disambiguator);
               switch ((Megalo::variable_scope)this->extra[0]) {
                  case Megalo::variable_scope::global: scopeType = QObject::tr("global scope", tr_disambiguator); break;
                  case Megalo::variable_scope::object: scopeType = QObject::tr("object scope", tr_disambiguator); break;
                  case Megalo::variable_scope::player: scopeType = QObject::tr("player scope", tr_disambiguator); break;
                  case Megalo::variable_scope::team:   scopeType = QObject::tr("team scope",   tr_disambiguator); break;
               }
               if (extra[4]) {
                  result += QObject::tr("The argument is the biped of %1 #%2 in %3 #%4, but there aren't that many of that scope.", tr_disambiguator).arg(varType).arg(this->extra[2]).arg(scopeType).arg(this->extra[1]);
               } else {
                  result += QObject::tr("The argument is %1 #%2 in %3 #%4, but there aren't that many of that scope.", tr_disambiguator).arg(varType).arg(this->extra[2]).arg(scopeType).arg(this->extra[1]);
               }
            }
            break;
         case load_failure_detail::failed_to_construct_script_opcode_arg:
            result += QObject::tr("For some reason, we were unable to construct an in-memory data structure to hold this argument's value.", tr_disambiguator);
            break;
      }
   } else {
      result = QObject::tr("Failed to load %1 #%2. ", tr_disambiguator).arg(noun_single).arg(this->failure_index);
      switch (this->reason) {
         case load_failure_reason::script_opcode_cannot_be_none:
            result += QObject::tr("This program is running in a testing mode, where \"None\" %1 are not allowed.", tr_disambiguator).arg(noun_plural);
            break;
         case load_failure_reason::invalid_script_opcode_function_index:
            result += QObject::tr("The %1 has function index %2, which is not valid.", tr_disambiguator).arg(noun_single).arg(this->extra[0]);
            break;
         case load_failure_reason::early_eof:
            result += QObject::tr("The file ended early; the data is cut off.", tr_disambiguator);
            break;
         case load_failure_reason::block_ended_early:
            result += QObject::tr("The containing data block is cut off.", tr_disambiguator);
            break;
      }
   }
   return result;
}
QString GameEngineVariantLoadError::to_qstring() const noexcept {
   constexpr char* tr_disambiguator = "GameEngineVariantLoadError";
   //
   QString result;
   if (this->state != load_state::failure)
      return result;
   switch (this->failure_point) {
      case load_failure_point::block_blam:
         if (this->reason == load_failure_reason::not_a_blam_file) {
            return QObject::tr("This is not a Halo: Reach content file.", tr_disambiguator);
         }
         result = QObject::tr("Something went wrong while reading the file header (_blf section). ", tr_disambiguator);
         switch (this->reason) {
            case load_failure_reason::early_eof:
               result += QObject::tr("The file ended early; the data is cut off.", tr_disambiguator);
               break;
            case load_failure_reason::block_ended_early:
               result += QObject::tr("The data block is cut off.", tr_disambiguator);
               break;
            //
            // the (_blf) block never sends load_failure_reason::block_missing because if the block is missing, then the file isn't a Reach content file
         }
         return result;
      case load_failure_point::block_chdr:
         result = QObject::tr("Something went wrong while reading the file header (chdr section). ", tr_disambiguator);
         switch (this->reason) {
            case load_failure_reason::early_eof:
               result += QObject::tr("The file ended early; the data is cut off.", tr_disambiguator);
               break;
            case load_failure_reason::block_ended_early:
               result += QObject::tr("The data block is cut off.", tr_disambiguator);
               break;
            case load_failure_reason::block_missing:
               result += QObject::tr("The data block is missing.", tr_disambiguator);
               break;
         }
         return result;
      case load_failure_point::block_mpvr:
         result = QObject::tr("Something went wrong while reading the multiplayer data. ", tr_disambiguator);
         switch (this->reason) {
            case load_failure_reason::early_eof:
               result += QObject::tr("The file ended early; the data is cut off.", tr_disambiguator);
               break;
            case load_failure_reason::block_ended_early:
               result += QObject::tr("The data block is cut off.", tr_disambiguator);
               break;
            case load_failure_reason::block_missing:
               result += QObject::tr("The data block is missing.", tr_disambiguator);
               break;
         }
         return result;
      case load_failure_point::content_type:
         switch ((ReachFileType)this->extra[0]) {
            case ReachFileType::film:
            case ReachFileType::film_clip:
               return QObject::tr("Saved Theater content is not supported.", tr_disambiguator);
            case ReachFileType::map_variant:
               return QObject::tr("Map variants are not supported.", tr_disambiguator);
            case ReachFileType::screenshot:
               return QObject::tr("Xbox 360-era screenshots are not supported.", tr_disambiguator);
            case ReachFileType::none:
               return QObject::tr("This file's header doesn't specify its type.", tr_disambiguator);
         }
         return QObject::tr("The file has an unknown type: %1.", tr_disambiguator).arg(this->extra[0]);
      case load_failure_point::variant_type:
         switch ((ReachGameEngine)this->extra[0]) {
            case ReachGameEngine::campaign:
               return QObject::tr("Campaign variants are not supported. (Where'd you even find one? I'm pretty sure they were only ever used for Xbox 360 Matchmaking.)", tr_disambiguator);
            case ReachGameEngine::firefight:
               return QObject::tr("Firefight variants are not supported.", tr_disambiguator);
            case ReachGameEngine::none:
               return QObject::tr("The game variant doesn't specify its type (campaign, firefight, Forge, multiplayer, etc.); it just says zero.", tr_disambiguator);
         }
         return QObject::tr("The file has an unknown game variant type: %1.", tr_disambiguator).arg(this->extra[0]);
      case load_failure_point::string_table:
         return QObject::tr("Failed to decompress a string table. Zlib returned error code %1.", tr_disambiguator).arg(this->extra[0]);
      case load_failure_point::megalo_conditions:
      case load_failure_point::megalo_actions:
         return _explain_opcode_failure();
      case load_failure_point::megalo_triggers:
         result = QObject::tr("Failed to load trigger #%1. ", tr_disambiguator).arg(this->failure_index);
         if (this->reason == load_failure_reason::early_eof) {
            result += QObject::tr("The file ended early; the data is cut off.", tr_disambiguator);
         }
         return result;
   }
   return QObject::tr("An error occurred during read and was caught, but the program wasn't properly updated to display error information; let me know so I can fix that.\n\nRaw failure point: %1\nRaw reason: %2\nRaw detail: %3", tr_disambiguator
   ).arg((uint32_t)this->failure_point).arg((uint32_t)this->reason).arg((uint32_t)this->detail);
}