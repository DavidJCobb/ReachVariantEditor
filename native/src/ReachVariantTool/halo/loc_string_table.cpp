#include "loc_string_table.h"
extern "C" {
   #include "../../zlib/zlib.h" // interproject ref
}
#include "../game_variants/errors.h"
#include "../game_variants/warnings.h"

namespace halo {
   namespace impl::loc_string_table {
      extern void warn_on_loading_excess_strings(size_t claimed, size_t maximum) {
         #if _DEBUG
            __debugbreak();
         #endif
         GameEngineVariantLoadWarningLog::get().push_back(QString("String table's maximum count is %1 strings; table claims to have %2 strings.").arg(maximum).arg(claimed));
      }
   }
}