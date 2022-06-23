#include "create_operand.h"
#include <array>
#include <typeinfo>
#include "helpers/tuple_foreach.h"
#include "./operands/all.h"

namespace halo::reach::megalo {
   namespace {
      using operand_clone_function = void (*)(const operand& src, operand& dst);
      using operand_spawn_function = operand* (*)();

      struct operand_type_table_entry {
         const operand_typeinfo& typeinfo  = operand_types::no_type;
         const std::type_info*   native_ti = nullptr;
         //
         operand_clone_function  clone = nullptr;
         operand_spawn_function  spawn = nullptr;
      };

      template<typename T> void _clone(const operand& src, operand& dst) {
         static_cast<T&>(dst) = static_cast<const T&>(src);
      }
      template<typename T> operand* _spawn() { return new T; }

      static constexpr auto operand_type_table = [](){
         std::array<operand_type_table_entry, std::tuple_size_v<all_operands_tuple>> out = {};
         //
         size_t i = 0;
         cobb::tuple_foreach<all_operands_tuple>([&out, &i]<typename Type>() {
            std::construct_at(
               &out[i++],
               operand_type_table_entry{
                  .typeinfo  = Type::typeinfo,
                  .native_ti = &typeid(Type),
                  //
                  .clone = &_clone<Type>,
                  .spawn = &_spawn<Type>,
               }
            );
         });
         return out;
      }();
   }

   extern operand* create_operand(const operand_typeinfo& info) {
      for (auto& item : operand_type_table) {
         if (&item.typeinfo == &info)
            return (item.spawn)();
      }
      return nullptr;
   }

   extern const operand_typeinfo& typeinfo_of(const operand& subject) {
      const auto& ti = typeid(subject);
      for (auto& item : operand_type_table)
         if (item.native_ti == &ti)
            return item.typeinfo;
      cobb::unreachable();
   }

   extern operand* clone_operand(const operand& subject) {
      const auto& ti = typeid(subject);
      for (auto& item : operand_type_table) {
         if (item.native_ti == &ti) {
            // Same loop as typeinfo_of

            auto* clone = create_operand(item.typeinfo);
            item.clone(subject, *clone);
            return clone;
         }
      }
      return nullptr;
   }
}