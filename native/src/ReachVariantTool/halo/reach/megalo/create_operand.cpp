#include "create_operand.h"
#include <array>
#include "helpers/tuple_foreach.h"
#include "./operands/all.h"

namespace halo::reach::megalo {
   namespace {
      using operand_spawn_function = operand* (*)();

      struct operand_type_table_entry {
         const operand_typeinfo& typeinfo = operand_types::no_type;
         operand_spawn_function  spawn    = nullptr;
      };

      template<typename T> operand* _spawn() { return new T; }

      static constexpr auto operand_type_table = [](){
         std::array<operand_type_table_entry, std::tuple_size_v<all_operands_tuple>> out = {};
         //
         size_t i = 0;
         cobb::tuple_foreach<all_operands_tuple>([&out, &i]<typename Type>() {
            std::construct_at(
               &out[i++],
               operand_type_table_entry{
                  .typeinfo = Type::typeinfo,
                  .spawn    = &_spawn<Type>,
               }
            );
         });
         return out;
      }();
   }

   extern operand* create_operand(const operand_typeinfo& info) {
      for (auto& item : operand_type_table) {
         if (item.typeinfo == info)
            return (item.spawn)();
      }
      return nullptr;
   }
}