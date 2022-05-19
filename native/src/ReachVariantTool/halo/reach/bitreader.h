#pragma once
#include <variant>
#include <vector>
#include "halo/bitreader.h"
#pragma region Load process
   #pragma region Errors
      #include "halo/common/load_errors/string_table_too_large_buffer.h"
   #pragma endregion
#pragma endregion

namespace halo::reach {
   namespace load_process_messages {
      namespace megalo {
         enum class where {
            string_table,
         };
      };

      struct message_base {
         std::variant<
            megalo::where//,
         > where;
      };

      struct notice : public message_base {
      };
      struct warning : public message_base {
      };
      struct error : public message_base {
         using inner_data = std::variant<
            halo::common::load_errors::string_table_too_large_buffer//,
         >;

         inner_data data;
      };
      struct fatal : public message_base {
      };
   }

   class load_process {
      public:
         using notice  = load_process_messages::notice;
         using warning = load_process_messages::warning;
         using error   = load_process_messages::error;
         using fatal   = load_process_messages::fatal;

         void emit_notice(const notice&);
         void emit_warning(const warning&);
         void emit_error(const error&);
         void throw_fatal(const fatal&);

         bool has_fatal() const;
         const fatal& get_fatal() const;

         const std::vector<notice>& notices() const;
         const std::vector<warning>& warnings() const;
         const std::vector<error>& errors() const;
   };

   using bitreader = halo::bitreader<load_process>;
}