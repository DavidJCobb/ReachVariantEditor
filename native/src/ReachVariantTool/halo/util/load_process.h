#pragma once
#include <concepts>
#include <vector>

namespace halo::util {
   template<typename T> concept load_process = requires (T& process, const T& const_process) {
      typename T::notice;
      typename T::warning;
      typename T::error;
      typename T::fatal;
      { process.emit_notice(typename T::notice{}) };
      { process.emit_warning(typename T::warning{}) };
      { process.emit_error(typename T::error{}) };
      { process.throw_fatal(typename T::fatal{}) };
      { const_process.has_fatal() } -> std::same_as<bool>;
      { const_process.get_fatal() } -> std::same_as<const typename T::error&>;
      { const_process.notices() }   -> std::same_as<const std::vector<typename T::notice>&>;
      { const_process.warnings() }  -> std::same_as<const std::vector<typename T::warning>&>;
      { const_process.errors() }    -> std::same_as<const std::vector<typename T::error>&>;
      { process.import_from(const_process) };
   };

   template<typename T> struct extract_load_process_types {
      using notice  = void;
      using warning = void;
      using error   = void;
      using fatal   = void;
   };
   template<typename T> requires load_process<T> struct extract_load_process_types<T> {
      using notice  = T::notice;
      using warning = T::warning;
      using error   = T::error;
      using fatal   = T::fatal;
   };
}