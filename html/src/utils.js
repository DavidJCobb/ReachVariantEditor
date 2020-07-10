function _byteswap_ulong(n) {
   let a = n & 0xFF;
   let b = (n >>> 0x08) & 0xFF;
   let c = (n >>> 0x10) & 0xFF;
   let d = (n >>> 0x18);
   return d | (c << 0x08) | (b << 0x10) | (a << 0x18);
}
function _byteswap_ushort(n) {
   let a = n & 0xFF;
   let b = (n >>> 0x08) & 0xFF;
   return b | (a << 0x08);
}
function _byteswap_uint64(n) {
   // >>> is unavailable for BigInt and >> produces 
   // incorrect results if the sign bit is set
   let str = n.toString(16);
   let bytes = [];
   for(let i = 0; i < 8; i++) {
      let j = i * 2;
      bytes[i] = str.substring(j, j + 2);
   }
   bytes = "0x" + bytes.reverse().join("");
   //console.log(bytes);
   return BigInt(bytes);
}

function _highest_set_bit(n) {
   if (n instanceof BigInt)
      throw new TypeError("This function is not implemented for BigInt. Implementation difficulties include BigInt lacking a counterpart to >>>.");
   let r = 0;
   while (n >>>= 1)
      ++r;
   return r;
}
function _bitcount(n) {
   return _highest_set_bit(n) + 1;
}
function _bitswap(value, count) {
   let result = 0;
   for(let i = 0; i < count; i++) {
      let bit = (value & (1 << i)) ? 1 : 0;
      result |= bit << (count - i - 1);
   }
   return result;
}

function __purecall() {
   console.error("Pure virtual function call!");
   throw new Error("Pure virtual function call!");
}