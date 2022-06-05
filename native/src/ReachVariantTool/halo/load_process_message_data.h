#pragma once
#include <concepts>
#include <QString>

namespace halo {
   class load_process_message_data_base {
      public:
         enum class message_type {
            notice,
            warning,
            error,
            fatal,
         };

      public:
         virtual ~load_process_message_data_base() {}

         virtual QString to_string() const = 0;
         virtual message_type type() const = 0;

         virtual load_process_message_data_base* clone() const = 0;
   };

   //
   // This uses the curiously-recurring template pattern in order to more easily provide 
   // a "clone" function.
   //
   template<typename Subclass, load_process_message_data_base::message_type Type>
   class load_process_message_data : public load_process_message_data_base {
      public:
         static constexpr message_type compile_time_message_type = Type;

      public:
         virtual message_type type() const final { return Type; };

         virtual load_process_message_data_base* clone() const final {
            auto* inst = new Subclass;
            *inst = *static_cast<const Subclass*>(this);
            return inst;
         }
   };

   // As of C++20, you cannot use designated initializers with polymorphic classes. 
   // The pattern we're setting up to work around this is to define a nested type, 
   // a POD struct named "message_content", and to have an instance of this type as 
   // a member of your subclass named "info".
   template<typename T> concept load_process_message_wraps_content = requires(const T& x) {
      typename T::message_content;
      { x.info } -> std::same_as<const typename T::message_content&>;
   };
}