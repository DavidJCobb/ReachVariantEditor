#pragma once

namespace halo::reach::megalo::bolt {
   //
   // For error reporting.
   //
   enum class syntax_element {

      alias_name,
      alias_equal_sign,
      alias_value,

      block_end, // "end" keyword; if missing, block is unterminated

      condition_list_item,       // a single condition; missing in cases like "if then", "if not then", "if and", "if or", etc.
      condition_list_terminator, // "then"

      // Syntax elements for for loops
      for_loop_do,
      for_loop_forge_label,
      for_loop_object_type,

      function_call_argument_list_terminator, // closing ')'

      identifier_part,           // e.g. `name.` where no subsequent name or index follows
      identifier_part_name,      // e.g. `name.[5]`
      identifier_part_index,     // e.g. `name[]`
      identifier_part_index_end, // e.g. `name[`

      pragma_name,

      user_defined_enum_name,
      user_defined_enum_member_value,
      user_defined_enum_end,

      // Syntax elements for user-defined function definitions
      user_defined_function_definition_name,
      user_defined_function_definition_paren_l, // argument list start, except that we don't allow args in Bolt
      user_defined_function_definition_paren_r, // argument list end, except that we don't allow args in Bolt

      variable_declaration_variable,
      variable_declaration_network_priority,
      variable_declaration_default_value,

   };
}