#pragma once

namespace halo::reach::megalo::bolt::config {
   // Have we implemented the following?
   //  - Detecting when an `expression` is a constant expression
   //  - Computing the results of constant `expression`s
   inline constexpr bool constant_expression_evaluation_implemented = false;

   // Code which is planned to evaluate constant expressions (e.g. enum/alias declarations) should 
   // static-assert that this bool is false. When we implement constant expressions, we can set the 
   // above bool to true, triggering a cascade of compiler errors telling us what code needs to be 
   // updated to use the new functionality.
   inline constexpr bool code_needs_update_for_constant_expression_evaluation_implemented = !constant_expression_evaluation_implemented;
}