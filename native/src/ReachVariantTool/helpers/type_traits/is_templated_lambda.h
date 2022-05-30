#pragma once

namespace cobb {
   template<auto Lambda, typename TestType = void> concept is_templated_lambda = requires {
      { Lambda.template operator()<TestType>() };
   };
   template<typename Lambda, typename TestType = void> concept is_templated_lambda_type = requires (Lambda&& instance) {
      { instance.template operator()<TestType>() };
   };
}