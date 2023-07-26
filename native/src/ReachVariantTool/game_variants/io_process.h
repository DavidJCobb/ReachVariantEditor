#pragma once
#include <cstdint>
#include <vector>
#include "editor_file_block.h"
#include "../helpers/stream.h"

class GameVariantSaveProcess {
   public:
      struct flag {
         flag() = delete;
         enum type : uint64_t {
            none = 0,
            //
            uses_xrvt_scripts = 0x0000000000000001,
            uses_xrvt_strings = 0x0000000000000002,
            save_bare_mglo    = 0x0000000000000004,
         };
      };
      using flags_t = std::underlying_type_t<flag::type>;
      //
      cobb::bit_or_byte_writer writer;
      flags_t flags = flag::none;
      std::vector<RVTEditorBlock::subrecord*> xrvt_subrecords;
      //
      inline void set_flag(flags_t f) noexcept { this->flags |= f; }
      inline void clear_flag(flags_t f) noexcept { this->flags &= ~f; }
      inline void modify_flag(flags_t f, bool state) noexcept { (state) ? this->set_flag(f) : this->clear_flag(f); }
      inline bool has_flag(flags_t f) const noexcept { return this->flags & f; }
      //
      RVTEditorBlock::subrecord* create_subrecord(uint32_t signature) noexcept;
      inline bool has_subrecords() const noexcept { return !this->xrvt_subrecords.empty(); }
      //
      inline bool variant_is_editor_only() const noexcept {
         //
         // Return true if the variant will not function properly in-game because some data had to 
         // be stored in a manner that is invisible to the game, i.e. the user hasn't lost any work 
         // and can still edit things.
         //
         return this->flags & (flag::uses_xrvt_scripts | flag::uses_xrvt_strings);
      }
      //
      ~GameVariantSaveProcess();
};