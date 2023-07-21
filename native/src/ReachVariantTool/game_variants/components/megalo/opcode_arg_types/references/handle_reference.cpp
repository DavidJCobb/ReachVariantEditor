#include "./handle_reference.h"
#include "../../../../errors.h"
#include "./impl/reference_kind_info.h"

#include "./impl/explicit_handle_typeinfo.h"
#include "./impl/explicit_object_handle.h"
#include "./impl/explicit_player_handle.h"
#include "./impl/explicit_team_handle.h"

namespace Megalo {
   namespace {
      const references::explicit_handle_typeinfo& handle_typeinfo_for(reference_type t) {
         switch (t) {
            case reference_type::object: return references::explicit_object_handle_typeinfo;
            case reference_type::player: return references::explicit_player_handle_typeinfo;
            case reference_type::team:   return references::explicit_team_handle_typeinfo;
         }
         throw;
      }
   }

   bool _TypeErasedHandleReference::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
      uint8_t kind = stream.read_bits(std::bit_width(this->typed_details.kind_info.count - 1));
      if (kind >= this->typed_details.kind_info.count) {
         auto& error    = GameEngineVariantLoadError::get();
         error.state    = GameEngineVariantLoadError::load_state::failure;
         error.reason   = GameEngineVariantLoadError::load_failure_reason::bad_script_opcode_argument;
         error.detail   = GameEngineVariantLoadError::load_failure_detail::bad_variable_subtype;
         error.extra[0] = (int32_t)this->typed_details.type; // TODO: Make sure this matches our error reporting
         error.extra[1] = kind;
         return false;
      }

      const auto& kind_info = *(this->typed_details.kind_info.first + kind);

      auto& top_level_typeinfo = handle_typeinfo_for(kind_info.top_level_type);
      //
      this->top_level.untyped = stream.read_bits(top_level_typeinfo.bitcount()) - 1;
      assert(this->top_level.untyped >= -1);

      if (kind_info.member_type.has_value()) {
         const auto& member_typeinfo = handle_typeinfo_for(kind_info.member_type.value());
         const auto* top_level_scope = getScopeObjectForConstant((Megalo::variable_type)kind_info.top_level_type);

         size_t member_count = top_level_scope->max_variables_of_type((Megalo::variable_type)kind_info.member_type.value());
         size_t bitcount     = std::bit_width(member_count - 1U);

         this->member_index = stream.read_bits(bitcount);
         if (this->member_index >= member_count) {
            auto& error    = GameEngineVariantLoadError::get();
            error.state    = GameEngineVariantLoadError::load_state::failure;
            error.reason   = GameEngineVariantLoadError::load_failure_reason::bad_script_opcode_argument;
            error.detail   = GameEngineVariantLoadError::load_failure_detail::none; // TODO
            return false;
         }
      }
      
      if (this->top_level.untyped >= top_level_typeinfo.value_count) {
         auto& error    = GameEngineVariantLoadError::get();
         error.state    = GameEngineVariantLoadError::load_state::failure;
         error.reason   = GameEngineVariantLoadError::load_failure_reason::bad_script_opcode_argument;
         error.detail   = GameEngineVariantLoadError::load_failure_detail::bad_variable_scope;
         error.extra[0] = -1;
         error.extra[1] = this->top_level.untyped;
         error.extra[2] = this->member_index;
         error.extra[3] = (int32_t)this->typed_details.type;
         //
         // TODO: For errors on var.player.biped, extra[3] should be the player-type and extra[4] should be 1.
         //
         return false;
      }
      return true;
   }
   void _TypeErasedHandleReference::write(cobb::bitwriter& stream) const noexcept {
      const auto  kind_bitcount      = std::bit_width(this->typed_details.kind_info.count - 1);
      const auto& kind_info          = *(this->typed_details.kind_info.first + this->kind);
      const auto& top_level_typeinfo = handle_typeinfo_for(kind_info.top_level_type);

      stream.write(this->kind, kind_bitcount);
      stream.write(this->top_level.untyped + 1, top_level_typeinfo.bitcount());
      if (kind_info.member_type.has_value()) {
         const auto& member_typeinfo = handle_typeinfo_for(kind_info.member_type.value());
         const auto* top_level_scope = getScopeObjectForConstant((Megalo::variable_type)kind_info.top_level_type);

         size_t member_count = top_level_scope->max_variables_of_type((Megalo::variable_type)kind_info.member_type.value());
         size_t bitcount     = std::bit_width(member_count - 1U);

         stream.write(this->member_index, bitcount);
      }
   }
}