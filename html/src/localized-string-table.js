const REACH_LANGUAGES = Object.freeze([
   { name: "English" },
   { name: "Japanese" },
   { name: "German" },
   { name: "French" },
   { name: "Spanish" },
   { name: "Mexican" },
   { name: "Italian" },
   { name: "Korean" },
   { name: "ChineseTrad" },
   { name: "ChineseSimp" },
   { name: "Portugese" },
   { name: "Polish", optional: true },
]);
const REACH_LANGUAGE_ENGLISH = 0;

class LocalizedString {
   constructor() {
      this.offsets = [];
      this.strings = [];
      for(let i = 0; i < REACH_LANGUAGES.length; i++) {
         this.offsets[i] = -1;
         this.strings[i] = "";
      }
   }
   parseOffsets(stream, table) {
      for(let i = 0; i < this.offsets.length; i++) {
         let has = stream.readBits(1) != 0;
         if (has)
            this.offsets[i] = stream.readBits(table.offsetBitlength);
         else
            this.offsets[i] = -1;
      }
   }
   parseStrings(stream, buffer) {
      for(let i = 0; i < this.offsets.length; i++) {
         let off = this.offsets[i];
         if (off == -1)
            continue;
         let frag = buffer.slice(off);
         let str  = new TextDecoder("utf-8").decode(frag);
         let n = str.indexOf("\0"); // TextDecoder keeps the null terminator, apparently
         if (n >= 0)
            str = str.substring(0, n);
         this.strings[i] = str;
      }
   }
   toString() {
      let eng = this.strings[REACH_LANGUAGE_ENGLISH];
      if (eng)
         return eng;
      for(let i = 1; i < REACH_LANGUAGES.length; i++)
         if (this.strings[i])
            return `[${REACH_LANGUAGES[i].name}]${this.strings[i]}`;
      return "";
   }
}

class LocalizedStringTable {
   constructor(maxStrings, maxBufferSize) {
      this.maxStrings    = maxStrings;
      this.maxBufferSize = maxBufferSize;
      //
      this.countBitlength      = _bitcount(maxStrings);
      this.bufferSizeBitlength = _bitcount(maxBufferSize);
      this.offsetBitlength     = _bitcount(maxBufferSize - 1);
      //
      this.strings = []; // Array<LocalizedString>
   }
   _makeBuffer(stream) {
      let uncompressedSize = stream.readBits(this.bufferSizeBitlength, false);
      let isCompressed     = stream.readBits(1) != 0;
      let size = uncompressedSize;
      //
      if (isCompressed)
         size = stream.readBits(this.bufferSizeBitlength, false);
      //console.log(`Localized string buffer; compressed size ${size} bytes, compressed state ${isCompressed}, expected to decompress to ${uncompressedSize} bytes.`);
      //
      let buffer = [];
      for(let i = 0; i < size; i++)
         buffer[i] = stream.readByte();
      //
      if (isCompressed) {
         {
            let o = 0;
            for(let i = 0; i < 4; i++)
               o |= buffer[i] << (i * 0x08);
            o = _byteswap_ulong(o);
            if (o != uncompressedSize)
               console.warn(`Uncompressed size in zlib header doesn't match uncompressed size in Bungie header (${o} versus ${uncompressedSize}).`);
         }
         if (pako) {
            //let bin  = new Uint8Array(buffer);
            let bin  = new Uint8Array(buffer.slice(4)); // skip the zlib header's uncompressed size
            let data = pako.inflate(bin);
            //console.log(`inflated data to ${data.length} bytes; ${uncompressedSize} expected`);
            buffer = data;
         } else
            buffer = null;
      } else
         buffer = new Uint8Array(buffer);
      return buffer;
   }
   parse(stream) {
      let count = stream.readBits(this.countBitlength, BIT_ORDER_UNKNOWN);
      for(let i = 0; i < count; i++) {
         let o = this.strings[i] = new LocalizedString();
         o.parseOffsets(stream, this);
      }
      if (count) {
         let buffer = this._makeBuffer(stream);
         if (buffer)
            for(let i = 0; i < count; i++)
               this.strings[i].parseStrings(stream, buffer);
      }
   }
}