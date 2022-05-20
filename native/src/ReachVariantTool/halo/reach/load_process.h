#pragma once
#include <optional>
#include <variant>
#include <vector>
#pragma region Load process
   #pragma region Errors
      #include "halo/common/load_errors/file_block_unexpected_end.h"
      #include "halo/common/load_errors/file_is_for_the_wrong_game.h"
      #include "halo/common/load_errors/game_variant_no_file_block_for_data.h"
      #include "halo/common/load_errors/first_file_block_is_not_blam_header.h"
      #include "halo/common/load_errors/invalid_file_block_header.h"
      #include "halo/common/load_errors/string_table_cannot_allocate_buffer.h"
      #include "halo/common/load_errors/string_table_entries_not_null_separated.h"
      #include "halo/common/load_errors/string_table_entry_offset_out_of_bounds.h"
      #include "halo/common/load_errors/string_table_too_large_buffer.h"
      #include "halo/common/load_errors/string_table_too_large_count.h"
   #pragma endregion
#pragma endregion
#include "./ugc_file_type.h"

namespace halo::reach {
   namespace load_errors {
      struct not_a_game_variant {
         ugc_file_type type = ugc_file_type::none;
      };
   }

   namespace load_process_messages {
      namespace megalo {
         enum class where {
            string_table,
         };
      };
      using where_t = std::variant<
         std::monostate,
         load_process_messages::megalo::where//,
      >;

      struct message_base {
         where_t where;
      };

      struct notice : public message_base {
      };
      struct warning : public message_base {
      };
      struct error : public message_base {
         using inner_data = std::variant<
            halo::common::load_errors::file_block_unexpected_end,
            halo::common::load_errors::invalid_file_block_header,
            halo::common::load_errors::string_table_cannot_allocate_buffer,
            halo::common::load_errors::string_table_entries_not_null_separated,
            halo::common::load_errors::string_table_entry_offset_out_of_bounds,
            halo::common::load_errors::string_table_too_large_buffer,
            halo::common::load_errors::string_table_too_large_count//,
         >;

         inner_data data;
      };
      struct fatal : public message_base {
         using inner_data = std::variant<
            halo::common::load_errors::file_is_for_the_wrong_game,
            halo::common::load_errors::first_file_block_is_not_blam_header,
            halo::common::load_errors::game_variant_no_file_block_for_data,
            halo::reach::load_errors::not_a_game_variant//,
         >;

         inner_data data;
      };
   }

   class load_process {
      public:
         using notice  = load_process_messages::notice;
         using warning = load_process_messages::warning;
         using error   = load_process_messages::error;
         using fatal   = load_process_messages::fatal;

      protected:
         struct {
            std::vector<notice>  notices;
            std::vector<warning> warnings;
            std::vector<error>   errors;
            std::optional<fatal> fatal_error;
         } contents;
         load_process_messages::where_t where;

      public:
         void emit_notice(const notice&);
         void emit_warning(const warning&);
         void emit_error(const error&);
         void throw_fatal(const fatal&);

         bool has_fatal() const { return this->contents.fatal_error.has_value(); }
         const fatal& get_fatal() const { return this->contents.fatal_error.value(); }

         inline const std::vector<notice>& notices() const { return this->contents.notices; }
         inline const std::vector<warning>& warnings() const { return this->contents.warnings; }
         inline const std::vector<error>& errors() const { return this->contents.errors; }

         void import_from(const load_process&);
   };
}