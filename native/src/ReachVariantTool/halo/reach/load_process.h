#pragma once
#include <memory>
#include <optional>
#include <variant>
#include <vector>
#include "halo/load_process_message_data.h"

namespace halo::reach {
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
         std::unique_ptr<load_process_message_data_base> data;

         message_base() {}
         message_base(const message_base& o) {
            this->where = o.where;
            this->data.reset((o.data) ? (o.data->clone()) : nullptr);
         }
         message_base(const load_process_message_data_base& d) {
            this->data.reset(d.clone());
         }

         template<typename T> requires load_process_message_wraps_content<T>
         message_base(const typename T::message_content& info) {
            this->data = std::make_unique<T>();
            ((T*)this->data)->info = info;
         }
      };

      struct notice : public message_base {
         using message_base::message_base;
      };
      struct warning : public message_base {
         using message_base::message_base;
      };
      struct error : public message_base {
         using message_base::message_base;
      };
      struct fatal : public message_base {
         using message_base::message_base;
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
         
         template<typename T> requires load_process_message_wraps_content<T> void emit_notice(const typename T::message_content& info) {
            static_assert(T::compile_time_message_type == load_process_message_data_base::message_type::notice);
            //
            notice item;
            item.data = std::make_unique<T>();
            ((T*)item.data.get())->info = info;
            this->emit_notice(item);
         }
         template<typename T> requires load_process_message_wraps_content<T> void emit_warning(const typename T::message_content& info) {
            static_assert(T::compile_time_message_type == load_process_message_data_base::message_type::warning);
            //
            warning item;
            item.data = std::make_unique<T>();
            ((T*)item.data.get())->info = info;
            this->emit_warning(item);
         }
         template<typename T> requires load_process_message_wraps_content<T> void emit_error(const typename T::message_content& info) {
            static_assert(T::compile_time_message_type == load_process_message_data_base::message_type::error);
            //
            error item;
            item.data = std::make_unique<T>();
            ((T*)item.data.get())->info = info;
            this->emit_error(item);
         }
         template<typename T> requires load_process_message_wraps_content<T> void throw_fatal(const typename T::message_content& info) {
            static_assert(T::compile_time_message_type == load_process_message_data_base::message_type::fatal);
            //
            fatal item;
            item.data = std::make_unique<T>();
            ((T*)item.data.get())->info = info;
            this->throw_fatal(item);
         }

         bool has_fatal() const { return this->contents.fatal_error.has_value(); }
         const fatal& get_fatal() const { return this->contents.fatal_error.value(); }

         inline const std::vector<notice>& notices() const { return this->contents.notices; }
         inline const std::vector<warning>& warnings() const { return this->contents.warnings; }
         inline const std::vector<error>& errors() const { return this->contents.errors; }
   };
}