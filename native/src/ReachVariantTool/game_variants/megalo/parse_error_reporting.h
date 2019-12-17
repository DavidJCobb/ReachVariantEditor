#pragma once
#include <cstdint>

namespace Megalo {
   struct ParseState {
      static ParseState& get() {
         static ParseState instance;
         return instance;
      }
      static void print() noexcept {
         ParseState::get()._print();
      }
      static void reset() noexcept {
         ParseState::get()._reset();
      }
      //
      enum class opcode_type : uint8_t {
         none,
         condition,
         action,
      };
      enum class what : uint8_t {
         none,
         bad_variable_type,
         bad_variable_subtype,
         bad_player_or_team_var_type,
      };
      //
      bool        signalled = false;
      opcode_type opcode;
      uint32_t    opcode_index;
      uint32_t    opcode_arg_index;
      what        cause;
      //
      uint32_t    extra[10];
      //
      void _print() const noexcept;
      void _reset() noexcept;
   };

}