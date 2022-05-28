#pragma once
#include <variant>
#include "helpers/cs.h"
#include "helpers/tuple_filter.h"
#include "helpers/tuple_prepend.h"
#include "helpers/tuple_transform.h"
#include "helpers/tuple_unpack.h"
#include "halo/util/refcount.h"
#include "../../operand.h"
#include "../../operand_typeinfo.h"

#include "../../variable_type.h"
#include "./register_type.h"

namespace halo::reach::megalo::operands::variables {
   namespace impl {
      class base : public operand {
         protected:
            size_t  register_set_index = 0;
         public:
            uint8_t which = 0;
            int16_t index = 0;

            virtual void read(bitreader& stream) override;
      };

      template<typename Tuple> struct extract_indexed_type_list {
         template<typename Current> struct transformer {
            using type = util::refcount_ptr<Current>;
         };

         //
         // Iterate over all of the register set metadata types. Find any that 
         // refer to indexed data, and extract the types of indexed data they 
         // refer to. Create a std::variant of util::refcount_ptrs to those 
         // types, and give the variant std::monostate as its first type.
         //
         using type = cobb::tuple_unpack_t<
            std::variant,
            cobb::tuple_prepend<
               std::monostate,
               cobb::tuple_transform<
                  transformer,
                  cobb::tuple_filter_t<
                     []<typename Current>() {
                        return !std::is_same_v<void, typename Current::indexed_data_type>;
                     },
                     Tuple
                  >
               >
            >
         >;
      };
   }

   template<
      cobb::cs Name,
      variable_type Type,
      typename RegisterSetsTuple,
      auto RegisterSetsTupleGetter
   > class base : public impl::base {
      public:
         static constexpr variable_type type = Type;
         static constexpr operand_typeinfo typeinfo = {
            .internal_name = Name.c_str(),
         };

         static constexpr RegisterSetsTuple register_sets_tuple = RegisterSetsTupleGetter();

      protected:
         using all_indexed_types = impl::extract_indexed_type_list<typename std::decay_t<RegisterSetsTuple>>;

      public:
         all_indexed_types indexed_data = {};

         virtual void read(bitreader& stream) override;
   };
}