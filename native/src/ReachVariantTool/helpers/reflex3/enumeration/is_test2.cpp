#include <tuple>

namespace helpers {

   // Iterate over all values in a tuple using a templated lambda.
   template<typename Tuple, typename Functor>
   extern consteval void tuple_for_each_value(Tuple&& tuple, Functor&& functor) {
      // IIFE to avoid the need for a helper function:
      []<std::size_t... I>(Tuple&& tuple, Functor&& functor, std::index_sequence<I...>) consteval {
         (functor(std::get<I>(tuple)), ...);
      }(
         std::forward<Tuple>(tuple),
         std::forward<Functor>(functor),
         std::make_index_sequence<std::tuple_size_v<std::decay_t<Tuple>>>{}
      );
   }
}

namespace test {
   template<typename T> struct dummy { T v; };
   struct basic { int v; };

   template<typename T> struct trait {};

   template<typename Subclass> class host_base {
      public:
         static consteval bool tup_has_item(int i) {
            bool has = false;
            helpers::tuple_for_each_value(trait<Subclass>::my_tup, [i, &has]<typename T>(const T& item) {
               if (item.v == i)
                  has = true;
            });
            return has;
         };
   };

   class host;
   template<> struct trait<host> {
      static constexpr const auto my_tup = std::make_tuple(
         basic{ 1 },
         basic{ 2 },
         dummy{ 3 }
      );
   };
   class host : public host_base <host> {
   };

   constexpr bool has = host::tup_has_item(1);
};