#pragma once

namespace halo::reach::megalo::bolt::config {
   // Have we implemented the following?
   //  - Detecting whether an `expression` evaluates to a reference result (i.e. a single variable 
   //    or value)
   //  - Resolving those references
   inline constexpr bool reference_expression_evaluation_implemented = false;

   // Code which is planned to evaluate reference expressions (e.g. alias declarations) should 
   // static-assert that this bool is false. When we implement resolving references, we can set 
   // the above bool to true, triggering a cascade of compiler errors telling us what code needs 
   // to be updated to use the new functionality.
   inline constexpr bool code_needs_update_for_reference_expression_evaluation_implemented = !reference_expression_evaluation_implemented;
}