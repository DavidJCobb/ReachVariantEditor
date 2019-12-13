class GameVariantPlayerRatingParams {
   constructor(stream) {
      this.unknownFloats = [];
      for(let i = 0; i < 15; i++)
         this.unknownFloats[i] = 0;
      this.unknownFlag = false;
      //
      if (stream)
         this.parse(stream);
   }
   parse(stream) {
      this.unknownFloats = [];
      for(let i = 0; i < 15; i++)
         //
         // TODO: JavaScript is using the wrong endianness here; C++ on x86 apparently isn't
         //
         this.unknownFloats[i] = stream.readFloat();
      this.unknownFlag = stream.readBits(1) != 0;
   }
}