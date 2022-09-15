#include <tuple>

#define CONCAT_MACRO(a, b) a##b
#define CONCAT_MACRO3(a, b, c) a##b##c

#define SEPARATE_NAMESPACES 1
#define P1717_SIBLINGS 0
#if SEPARATE_NAMESPACES == 1
   #define NS_DEFN definitions
   #define NS_TEST tests
   #if P1717_SIBLINGS == 1
      #define DATA_TRAIT_SPECIALIZATION_NAME CONCAT_MACRO3(::, NS_DEFN, ::data)
   #else
      #define DATA_TRAIT_SPECIALIZATION_NAME definitions__data
   #endif
#else
   #define NS_DEFN 
   #define NS_TEST 
   #if P1717_SIBLINGS == 1
      #define DATA_TRAIT_SPECIALIZATION_NAME data
   #else
      #define DATA_TRAIT_SPECIALIZATION_NAME definitions__data
   #endif
#endif

// Helper functions that aren't specifically part of the problem, but are used 
// to facilitate the affected code.
namespace helpers {

   // Iterate over all values in a tuple using a templated lambda.
   template<typename Tuple, typename Functor>
   extern constexpr void tuple_for_each_value(Tuple&& tuple, Functor&& functor) {
      // IIFE to avoid the need for a helper function:
      []<std::size_t... I>(Tuple&& tuple, Functor&& functor, std::index_sequence<I...>) {
         (functor(std::get<I>(tuple)), ...);
      }(
         std::forward<Tuple>(tuple),
         std::forward<Functor>(functor),
         std::make_index_sequence<std::tuple_size_v<std::decay_t<Tuple>>>{}
      );
   }
}

#pragma region Definitions for the affected code

#if P1717_SIBLINGS == 0
// must be outside of any namespace so it can be specialized from any namespace; use "__" instead of "::"
template<typename T> struct definitions__data;
#else
   #if SEPARATE_NAMESPACES == 1
   namespace NS_DEFN {
   #endif
      template<typename T> struct data;
   #if SEPARATE_NAMESPACES == 1
   }
   #endif
#endif

#if SEPARATE_NAMESPACES == 1
namespace NS_DEFN {
#endif
   #if P1717_SIBLINGS == 0
   template<typename T> using data = definitions__data<T>;
   #endif

   struct member {
      int id;
   };
   template<typename List> struct member_nested {
      using list_type = List;

      int id;
   };
   template<typename T> concept is_member = requires {
      T::id;
   };

   // CRTP
   template<typename Subclass> struct list {
      using my_type = Subclass;
      using data_type = data<my_type>;
      static constexpr const auto& members = data_type::members;

      static consteval bool has_item(int id) {
         bool has = false;
         helpers::tuple_for_each_value(members, [id, &has]<typename Current>(const Current& item) {
            if constexpr (is_member<Current>) {
               if (id == item.id)
                  has = true;
            }
         });
         return has;
      }
   };
#if SEPARATE_NAMESPACES == 1
}
#endif
#pragma endregion

#pragma region Code which triggers the problem given those definitions
#if SEPARATE_NAMESPACES == 1
namespace NS_TEST {
#endif
   template<typename T> struct dummy { T v; };

   class inner_list;
   template<> struct DATA_TRAIT_SPECIALIZATION_NAME<inner_list> { // GCC fails here (due to not implementing P1787 afaik)
      static constexpr const auto members = std::make_tuple(
         NS_DEFN::member{ 1 },
         NS_DEFN::member{ 2 },
         NS_DEFN::member{ 3 }
      );
   };
   class inner_list : public NS_DEFN::list<inner_list> {};

   constexpr bool has_1 = inner_list::has_item(1);
   constexpr bool has_n6 = inner_list::has_item(-6);

   class outer_list;
   template<> struct DATA_TRAIT_SPECIALIZATION_NAME<outer_list> { // GCC fails here (due to not implementing P1787 afaik)
      static constexpr const auto members = std::make_tuple(
         NS_DEFN::member{ 101 },
         NS_DEFN::member{ 102 },
         NS_DEFN::member{ 103 },
         NS_DEFN::member_nested<inner_list>{104}//
      );
   };
   class outer_list : public NS_DEFN::list<outer_list> {};

   using foo = decltype(dummy{ 3 });
   using bar = NS_DEFN::data<outer_list>;
   using baz = decltype(NS_DEFN::data<outer_list>::members);

   constexpr auto value   = outer_list{};
   constexpr bool has_101 = outer_list::has_item(101); // IntelliSense fails here, believing 
#if SEPARATE_NAMESPACES == 1
}
#endif
#pragma endregion