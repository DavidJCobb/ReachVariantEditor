#include "editor_file_block.h"

RVTEditorBlock::RVTEditorBlock() : ReachFileBlock('xRVT', ReachFileBlock::any_size) {
   this->found = this->expected; // TODO: we do this so the block writes properly if it wasn't already present in the file, but this is ugly; make it better
   this->found.size = 0;
}
RVTEditorBlock::~RVTEditorBlock() {
   for (auto* sub : this->subrecords)
      if (sub)
         delete sub;
   this->subrecords.clear();
}
bool RVTEditorBlock::read(reach_block_stream& stream) noexcept {
   auto bytes = stream.bytes;
   if (!ReachFileBlock::read(bytes))
      return false;
   while (stream.is_in_bounds(0x10)) { // has a subrecord header
      auto sub = new subrecord;
      this->subrecords.push_back(sub);
      bytes.read(sub->signature, cobb::endian_t::big);
      bytes.read(sub->version,   cobb::endian_t::big);
      bytes.read(sub->flags,     cobb::endian_t::big);
      bytes.read(sub->size,      cobb::endian_t::big);
      //
      sub->data.allocate(sub->size);
      auto base = sub->data.data();
      for (uint32_t i = 0; i < sub->size; ++i)
         bytes.read(base[i]);
   }
   return true;
}
void RVTEditorBlock::write(cobb::bytewriter& stream) const noexcept {
   ReachFileBlock::write(stream);
   for (auto* sub : this->subrecords) {
      if (!sub)
         continue;
      #if _DEBUG
         if (sub->size < sub->data.size())
            __debugbreak();
      #endif
      stream.write(sub->signature, cobb::endian_t::big);
      stream.write(sub->version,   cobb::endian_t::big);
      stream.write(sub->flags,     cobb::endian_t::big);
      stream.write(sub->size,      cobb::endian_t::big);
      //
      auto base = sub->data.data();
      for (uint32_t i = 0; i < sub->size; ++i)
         stream.write(base[i]);
   }
   ReachFileBlock::write_postprocess(stream);
}
void RVTEditorBlock::adopt(std::vector<RVTEditorBlock::subrecord*>& adoptees) noexcept {
   auto& list = this->subrecords;
   auto  size = list.size();
   auto  add  = adoptees.size();
   list.resize(size + add);
   for (size_t i = 0; i < add; ++i)
      list[size + i] = adoptees[i];
   adoptees.clear();
}
RVTEditorBlock::subrecord* RVTEditorBlock::lookup(uint32_t signature) const {
   for (auto* sub : this->subrecords)
      if (sub && sub->signature == signature)
         return sub;
   return nullptr;
}