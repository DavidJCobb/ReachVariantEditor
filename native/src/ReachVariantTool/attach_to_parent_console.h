#pragma once

enum class attach_to_console_result {
   success,
   not_supported,
   already_have_a_console,
   parent_process_has_no_console,
   no_parent_process,
};

extern attach_to_console_result attach_to_parent_console();
extern bool attach_to_new_console();