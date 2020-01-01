#pragma once
#include <cstdint>
#include <QString>

struct GameEngineVariantLoadError {
   enum class load_state {
      incomplete,
      success,
      failure,
   };
   enum class load_failure_point {
      none,
      //
      block_blam,
      block_chdr,
      block_mpvr, // should only appear if we can't identify a more specific cause
      //
      content_type, // extra[0] is the ReachFileType
      variant_type, // extra[0] is the variant type
      //
      string_table, // ZLIB decompression failure; extra[0] is the ZLIB failure code
      //
      megalo_conditions,
      megalo_actions,
      megalo_triggers,
   };
   enum class load_failure_reason {
      none,
      zlib_decompress_error,
      invalid_script_opcode_function_index, // extra[0] is the function index
      script_opcode_cannot_be_none, // should only appear during testing
      bad_script_opcode_argument, // can have load_failure_detail::failed_to_construct_script_opcode_arg
      early_eof,
      block_ended_early,
      block_missing,
      not_a_blam_file, // can be sent by the "block_blam" failure point
   };
   enum class load_failure_detail {
      none,
      failed_to_construct_script_opcode_arg,
      bad_opcode_variable_type, // extra[0] is the type value
      bad_opcode_player_or_team_var_type, // extra[0] is the type value
      bad_variable_subtype, // extra[0] is the type; extra[1] is the subtype
      bad_variable_scope, // extra[0] is a (variable_scope) constant; extra[1] is which scope of that type; extra[2] is the variable index; extra[3] is the variable type; extra[4] is 1 if it's a biped variable
   };

   load_state          state            = load_state::incomplete;
   load_failure_point  failure_point    = load_failure_point::none;
   int32_t             failure_index    = -1; // e.g. condition index
   int32_t             failure_subindex = -1; // e.g. condition arg index
   load_failure_reason reason           = load_failure_reason::none;
   load_failure_detail detail           = load_failure_detail::none;
   int32_t extra[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

   static GameEngineVariantLoadError& get() {
      static thread_local GameEngineVariantLoadError instance;
      return instance;
   }
   void reset();

   inline bool has_detail() const noexcept { return this->detail != load_failure_detail::none; }
   inline bool has_failure_point() const noexcept { return this->failure_point != load_failure_point::none; }

   QString to_qstring() const noexcept;
   //
   private:
      QString _explain_opcode_failure() const noexcept;
};