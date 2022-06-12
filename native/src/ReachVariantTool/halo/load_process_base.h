#pragma once
#include <cstdint>
#include <optional>
#include <vector>
#include "helpers/owned_ptr.h"
#include "helpers/split_vector.h"
#include "load_process_location.h"
#include "load_process_message_data.h"

namespace halo {
   class load_process_base {
      public:
         load_process_base() {}

      protected:
         struct message_base {
            cobb::owned_ptr<load_process_message_data_base> data;
            std::vector<load_process_location> location;

            message_base() {}
            message_base(message_base&& o) noexcept {
               std::swap(this->location, o.location);
               std::swap(this->data,     o.data);
            }
            message_base(const message_base& o) {
               this->location = o.location;
               this->data     = (o.data) ? (o.data->clone()) : nullptr;
            }
            message_base(const load_process_message_data_base& d) {
               this->data = d.clone();
            }
         };

      public:
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

      protected:
         struct {
            std::vector<notice>  notices;
            std::vector<warning> warnings;
            std::vector<error>   errors;
            std::optional<fatal> fatal_error;
         } contents;
         cobb::split_vector<load_process_location, 10> location; // current location
         struct {
            size_t  bytes = 0;
            uint8_t bits  = 0;
         } stream_offsets;

      public:
         void emit_notice(notice&&);
         void emit_warning(warning&&);
         void emit_error(error&&);
         void throw_fatal(fatal&&);
         //
         void emit_notice(const notice&);
         void emit_warning(const warning&);
         void emit_error(const error&);
         void throw_fatal(const fatal&);

         void set_stream_offsets(size_t bytes, uint8_t bits);
         void enter_location(const load_process_location&);
         void exit_location();
         
         template<typename T> requires load_process_message_wraps_content<T> void emit_notice(const typename T::message_content& info) {
            static_assert(T::compile_time_message_type == load_process_message_data_base::message_type::notice);
            //
            notice item;
            item.data = new T;
            ((T*)item.data.get())->info = info;
            this->emit_notice(std::move(item));
         }
         template<typename T> requires load_process_message_wraps_content<T> void emit_warning(const typename T::message_content& info) {
            static_assert(T::compile_time_message_type == load_process_message_data_base::message_type::warning);
            //
            warning item;
            item.data = new T;
            ((T*)item.data.get())->info = info;
            this->emit_warning(std::move(item));
         }
         template<typename T> requires load_process_message_wraps_content<T> void emit_error(const typename T::message_content& info) {
            static_assert(T::compile_time_message_type == load_process_message_data_base::message_type::error);
            //
            error item;
            item.data = new T;
            ((T*)item.data.get())->info = info;
            this->emit_error(std::move(item));
         }
         template<typename T> requires load_process_message_wraps_content<T> void throw_fatal(const typename T::message_content& info) {
            static_assert(T::compile_time_message_type == load_process_message_data_base::message_type::fatal);
            //
            fatal item;
            item.data = new T;
            ((T*)item.data.get())->info = info;
            this->throw_fatal(std::move(item));
         }

         bool has_fatal() const { return this->contents.fatal_error.has_value(); }
         const fatal& get_fatal() const { return this->contents.fatal_error.value(); }

         inline const std::vector<notice>& notices() const { return this->contents.notices; }
         inline const std::vector<warning>& warnings() const { return this->contents.warnings; }
         inline const std::vector<error>& errors() const { return this->contents.errors; }
   };
}