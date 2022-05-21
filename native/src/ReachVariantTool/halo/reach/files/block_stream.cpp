#include "block_stream.h"
extern "C" {
   #include "../../../../zlib/zlib.h" // interproject ref
}

namespace halo::reach {
   extern file_block_stream read_next_file_block(bytereader& stream) {
      if (!stream.is_in_bounds()) {
         return file_block_stream::create_invalid();
      }
      util::four_cc signature = 0;
      //
      uint32_t pos_block_start = stream.get_position();
      file_block_header header;
      stream.read(header);
      if (header.size < 0xC) { // indicates a bad block, because this should include the size of the header itself
         return file_block_stream::create_invalid();
      }
      uint32_t pos_block_end = pos_block_start + header.size;
      if (header.signature == '_cmp') { // compressed block
         uint8_t  decompressed_unk00 = 0;
         uint32_t decompressed_size  = 0;
         {
            auto e = stream.endianness();
            stream.read(decompressed_unk00);
            stream.set_endianness(std::endian::big);
            stream.read(decompressed_size);
            stream.set_endianness(e);
         }

         if (!stream.is_in_bounds(header.size - 0xC - 1 - 4)) // subtract size of block header, size of decompressed_unk00, and size of decompressed_size
            return file_block_stream::create_invalid();
         auto input_buffer  = stream.read_buffer(header.size - 0xC - sizeof(decompressed_unk00) - sizeof(decompressed_size));
         auto output_buffer = cobb::generic_buffer(decompressed_size);
         uint32_t len = decompressed_size;
         int resultCode = uncompress((Bytef*)output_buffer.data(), (uLongf*)&len, input_buffer.data(), input_buffer.size());
         if (resultCode != Z_OK)
            return file_block_stream::create_invalid();
         //
         file_block_stream result(std::move(output_buffer));
         result.read(header);
         result.set_position(0); // rewind to start of decompressed header
         result.decompressed_unk00 = decompressed_unk00;
         result.was_compressed     = true;
         //
         stream.set_position(pos_block_end);
         //
         return result;
      }
      stream.set_position(pos_block_end);
      //
      cobb::generic_buffer result_buffer(header.size);
      memcpy(result_buffer.data(), stream.data() + pos_block_start, header.size);
      file_block_stream result(std::move(result_buffer));
      result.read(result.header);
      return result;
   }
}