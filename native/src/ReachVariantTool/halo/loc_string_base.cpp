#include "loc_string_base.h"

namespace halo {
   bool loc_string_base::can_be_forge_label() const {
      auto& list  = this->translations;
      auto& first = list[0];
      for (size_t i = 1; i < list.size(); i++) {
         if (list[i] != first)
            return false;
      }
      return true;
   }
   bool loc_string_base::empty() const {
      for (auto& item : this->translations)
         if (!item.empty())
            return false;
      return true;
   }

   void loc_string_base::serialize_to_blob(std::string& blob, loc_string_table_details::offset_list& offsets) const {
      if constexpr (loc_string_table_details::optimization == loc_string_table_details::optimization_type::bungie) {
         bool all_equal = true;
         for (size_t i = 1; i < this->translations.size(); ++i) {
            auto& a = this->translations[i - 1];
            auto& b = this->translations[i];
            if (a != b) {
               all_equal = false;
               break;
            }
         }
         if (all_equal) {
            size_t to = blob.size();
            for (auto& offset : offsets) {
               offset = to;
            }
            blob += this->translations[0];
            blob += '\0';
            return;
         }
         //
         // Else, fall through.
      }
      for (size_t i = 0; i < offsets.size(); i++) {
         auto& s = this->translations[i];
         //
         // NOTE: DO NOT serialize empty strings with a -1 offset; Bungie doesn't do that and I assume it's for a 
         // reason.
         //
         if constexpr (loc_string_table_details::optimization == loc_string_table_details::optimization_type::duplicates) {
            //
            // If the string table buffer already contains a string that is exactly identical to 
            // this string, then use that string's offset instead of adding a duplicate.
            //
            offsets[i] = -1;
            size_t j    = 0; // end   of existing string
            size_t k    = 0; // start of existing string
            size_t size = s.size();
            for (; j < blob.size(); j++) {
               if (blob[j] != '\0')
                  continue;
               #if _DEBUG
                  std::string existing = blob.substr(k, j - k);
               #endif
               if (j - k == size) {
                  if (strncmp(s.data(), blob.data() + k, size) == 0) {
                     offsets[i] = k;
                     break;
                  }
               }
               k = j + 1;
            }
            if (offsets[i] >= 0) {
               continue;
            }
         }
         offsets[i] = blob.size();
         blob += s;
         blob += '\0';
      }
   }
}