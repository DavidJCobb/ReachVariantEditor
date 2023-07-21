#pragma once
#include <cstdint>
#include "../../opcode_arg.h"
#include "./type.h"

#include "./impl/explicit_handle_union.h"

namespace Megalo {
   namespace references {
      struct explicit_handle_typeinfo;
      struct kind_info;
   }

   class _TypeErasedHandleReference : public OpcodeArgValue {
      protected:
         struct {
            reference_type type;
            struct {
               const references::kind_info* first = nullptr;
               size_t count = 0;
            } kind_info;
         } typed_details;

         constexpr const references::explicit_handle_typeinfo& _get_top_level_handle_typeinfo() const;

      public:
         uint8_t kind;
         explicit_handle_union top_level;
         uint8_t member_index = 0;

         virtual bool read(cobb::ibitreader&, GameVariantDataMultiplayer& mp) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;

         constexpr bool is_member_variable() const noexcept;
         constexpr bool is_property() const noexcept;

         constexpr bool is_never_networked() const noexcept;
         constexpr bool is_none() const noexcept;
         constexpr bool is_read_only() const noexcept;
         constexpr bool is_transient() const noexcept;
   };

   template<typename RefKindEnum>
   class HandleReference : public _TypeErasedHandleReference {
   };
}

#include "./handle_reference.inl"