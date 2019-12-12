class GameVariantMapPermissions {
   constructor(stream) {
      this.mapIDs = []; // std::vector<uint16_t>
      this.exceptionType = 1; // 0 = can only play on these maps; 1 = can play on any maps but these
      //
      if (stream)
         this.parse(stream);
   }
   parse(stream) {
      let count = stream.readBits(6, false);
      this.mapIDs = [];
      for(let i = 0; i < count; i++)
         this.mapIDs[i] = stream.readUInt16();
      this.exceptionType = stream.readBits(1); // 0 = can only play on these maps; 1 = can play on any maps but these
   }
}