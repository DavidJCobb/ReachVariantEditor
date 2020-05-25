#pragma once
#include <cstdint>
#include <vector>
#include "../formats/block.h"
#include "../helpers/memory.h"

class RVTEditorBlock : public ReachFileBlock {
   public:
      RVTEditorBlock();
      ~RVTEditorBlock();
      //
      static constexpr uint32_t signature_megalo_string_table = 'mstr';
      static constexpr uint32_t signature_megalo_script       = 'mscr';
      //
      struct subrecord {
         uint32_t signature = 0;
         uint32_t version   = 0;
         uint32_t flags     = 0;
         uint32_t size      = 0;
         cobb::generic_buffer data;
      };
      std::vector<subrecord*> subrecords;
      //
      bool read(reach_block_stream&) noexcept;
      void write(cobb::bytewriter&) const noexcept;
      //
      void adopt(std::vector<subrecord*>&) noexcept;
      subrecord* lookup(uint32_t signature) const;
      //
      inline bool has_subrecords() const noexcept { return this->subrecords.size(); }
};