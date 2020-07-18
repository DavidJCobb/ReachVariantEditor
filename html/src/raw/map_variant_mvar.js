class MVVector {
   constructor(a, b, c) {
      this[0] = 0;
      this[1] = 0;
      this[2] = 0;
      if (a instanceof Array || a instanceof MVVector) {
         this[0] = a[0] || 0;
         this[1] = a[1] || 0;
         this[2] = a[2] || 0;
      } else if (+a === a) {
         this[0] = a || 0;
         this[1] = b || 0;
         this[2] = c || 0;
      }
   }
   get x() { return this[0]; }
   get y() { return this[1]; }
   get z() { return this[2]; }
   set x(v) { return this[0] = v; }
   set y(v) { return this[1] = v; }
   set z(v) { return this[2] = v; }
   enforce_single_precision() {
      for(let i = 0; i < 3; ++i)
         this[i] = Math.fround(this[i]);
   }
   length() { return Math.sqrt(Math.fround(this[0]*this[0] + this[1]*this[1] + this[2]*this[2])); }
   normalize() {
      let l = this.length();
      let a = Math.abs(l);
      if (0.0001 <= a) {
         a = 1.0 / l;
         this[0] *= a;
         this[1] *= a;
         this[2] *= a;
         this.enforce_single_precision();
      }
   }
}

function _sub10A8A0(a, b, c) {
   let xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8;
   //
   xmm5 = a.y;
   xmm0 = 0;
   xmm4 = a.z;
   xmm7 = a.z * xmm0;
   xmm8 = a.x;
   xmm6 = a.x * xmm0;
   xmm0 = a.x + a.y + xmm7;
   xmm1 = xmm6 + a.y + xmm7;
   if (Math.abs(xmm1) > Math.abs(xmm0)) {
      xmm0 = xmm6 - a.y;
      b.y = a.z - xmm6;
      b.x = a.y - xmm7;
   } else {
      xmm6 -= xmm7;
      xmm0 = a.y - a.x;
      b.y = xmm6;
      b.x = a.z - a.y;
   }
   b.z = xmm0;
   b.normalize();
   xmm5 = a.x;
   xmm1 = b.x;
   xmm3 = a.z;
   xmm0 = a.y;
   xmm8 = a.x * b.y - a.y;
   xmm3 *= b.y;
   xmm2 = xmm3 * b.x;
   xmm0 = b.z * a.x;
   xmm4 = b.z * a.y - xmm3;
   c.z = xmm8;
   xmm2 -= xmm0;
   c.x = xmm4;
   c.y = xmm2;
   c.normalize();
}

function _sub4DC8E0(bitcount, mapBounds, out) {
   //
   // Determines the proper bitcounts to use for object position coordinates, given the 
   // map bounds and the baseline bitcount specified.
   //
   function highest_bit_set(value) {
      let r = 0;
      while (value >>= 1)
         ++r;
      return r;
   }
   
   let rangesByAxis = [
      mapBounds.x.max - mapBounds.x.min,
      mapBounds.y.max - mapBounds.y.min,
      mapBounds.z.max - mapBounds.z.min
   ];
   out[0] = bitcount;
   out[1] = bitcount;
   out[2] = bitcount;
   let min_step; // register XMM6 // minimum possible representable distance
   const MINIMUM_UNIT_16BIT = 0.00833333333; // hex 0x3C088889 == 0.00833333376795F
   if (bitcount > 0x10) {
      //
      // something to do with the "extra" bits; if bitcount == 0x10 then min_step is just the constant
      //
      min_step = MINIMUM_UNIT_16BIT;
      let ecx  = bitcount - 0x10; // (ecx = (dword)bitcount + 0xFFFFFFF0) i.e. (ecx = bitcount + -10)
      let xmm0 = 1 << ecx; // 1 << cl
      min_step /= xmm0;
      //
      // I think that min_step is something like our "effective precision," where 
      // the target is 16 bits (0x10) for 0.01-gradian steps, and if we have 
      // more bits, then we can use smaller steps with min_step being the step 
      // size...
      //
   } else {
      //
      // something to do with the "missing" bits; if bitcount == 0x10 then min_step is just the constant
      //
      min_step = 1 << (0x10 - bitcount);
      min_step *= MINIMUM_UNIT_16BIT;
      //
      // ...whereas if we have fewer than 16 bits, then we need to use a larger 
      // (i.e. less precise) step size.
      //
   }
   if (min_step >= 0.0001) { // hex 0x38D1B717 == 9.99999974738e-05
      min_step *= 2;
      for(let i = 0; i < 3; ++i) {
         let xmm0 = Math.ceil(rangesByAxis[i] / min_step);
         let edx  = Math.floor(xmm0); // truncate to integer
         edx = Math.min(0x800000, edx);
         let ecx = -1;
         if (edx) { // asm: TEST EDX, EDX; JE
            ecx = 31;
            if (edx >= 0) // asm: JS
               ecx = highest_bit_set(edx);
         }
         let r8 = 0;
         if (ecx != -1) {
            let eax = (1 << ecx) - 1;
            r8  = ecx + (((edx & eax) != 0) ? 1 : 0);
         }
         let eax = Math.min(26, r8);
         out[i] = eax;
      }
   } else {
      for(let i = 0; i < 3; ++i)
         out[i] = 26;
   }
}

class LFOUnk30 { // sizeof >= 0x1C
   sub6078F0(stream) {
      let sw = stream.readBits(2);
      this.unk10 = sw;
      let xmm3 = 0.0977517142892;
      let xmm0 = 0;
      let xmm4 = 0.0488758571446;
      let xmm2 = 200;
      let xmm1;
      switch (sw) {
         case 1: {
            let a = stream.readBits(0xB);
            if (!a) {
               this.unk00 = 0;
            } else {
               if (a == 0x7FF) {
                  this.unk00 = 200; // float
               } else {
                  --a;
                  a *= 0.0977517142892;
                  a += 0.0488758571446;
                  this.unk00 = a;
               }
            }
         }; return;
         //
         case 3: {
            let eax = stream.readBits(sw + 8);
            if (!eax) {
               xmm1 = xmm0;
            } else if (eax == 0x7FF) {
               xmm1 = xmm2;
            } else {
               xmm1 = (eax - 1) * xmm3 + xmm4;
            }
            this.unk00 = xmm1;
            eax = stream.readBits(0xB);
            if (eax == 0) {
               xmm1 = xmm0;
               this.unk04 = xmm1;
            } else if (eax == 0x7FF) {
               xmm1 = xmm2;
               this.unk04 = xmm1;
            } else {
               xmm1 = (eax - 1) * xmm3 + xmm4;
               this.unk04 = xmm1;
            }
         }; break;
         //
         case 2: {
            let eax = stream.readBits(0xB);
            if (!eax) {
               xmm1 = xmm0;
            } else if (eax == 0x7FF) {
               xmm1 = xmm2;
            } else {
               xmm1 = (eax - 1) * xmm3 + xmm4;
            }
            this.unk00 = xmm1;
         }; break;
         //
         default:
            return;
      }
      let eax = stream.readBits(0xB);
      if (!eax) {
         xmm1 = xmm0;
      } else if (eax == 0x7FF) {
         xmm1 = xmm2;
      } else {
         xmm1 = (eax - 1) * xmm3 + xmm4;
      }
      this.unk08 = xmm1;
      eax = stream.readBits(0xB);
      if (!eax) {
         xmm1 = xmm0;
      } else if (eax == 0x7FF) {
         xmm1 = xmm2;
      } else {
         xmm1 = (eax - 1) * xmm3 + xmm4;
      }
      this.unk0C = xmm1;
   }
   constructor(stream) {
      this.unk00 = 0; // float (shape dimension 0? width or length; radius for cylinder)
      this.unk04 = 0; // float (shape dimension 1? width or length)
      this.unk08 = 0; // float (shape dimension 2? top or bottom; top for cylinder)
      this.unk0C = 0; // float (shape dimension 3? top or bottom; bottom for cylinder)
      this.unk10 = 0; // byte  (shape type)
      this.spawnSequence = 0; // 11 // byte; UI clamps this to [-100, 100]
      this.respawnTime  = 0; // 12 // byte
      this.unk13 = 0; // byte
      this.forgeLabelIndex = -1; // 14 // word
      this.unk16 = 0; // byte // flags? maybe?
      this.unk17 = 0; // byte
      this.unk18 = 0; // qword
      this.team = -1; // 1A // byte
      this.unk17 = 8; // byte
      if (!stream)
         return;
      this.sub6078F0(stream); // read shape
      //
      let eax = stream.readBits(8);
      if (eax & 0x80000000) // test if signed
         eax |= 0xFFFFFF00;
      this.spawnSequence = eax & 0xFF;
      //
      this.respawnTime = stream.readBits(8);
      this.unk13 = stream.readBits(5); // teleporter channel? 's got the right number of bits
      if (stream.readBits(1)) { // absence bit
         this.forgeLabelIndex = -1; // word
      } else {
         this.forgeLabelIndex = stream.readBits(8); // word
      }
      this.unk16 = stream.readBits(8); // byte
      this.unk17 = stream.readBits(4) - 1; // object color?
      if (!stream.readBits(1)) {
         this.team = stream.readBits(3); // byte
      } else {
         this.team = -1; // byte
      }
      //
      if (this.unk13 == 1) {
         this.unk18 = stream.readBits(8); // byte
         return;
      } else if (this.unk13 <= 0xB) {
         return;
      } else if (this.unk13 <= 0xE) {
         this.unk18 = stream.readBits(5); // byte
         this.unk19 = stream.readBits(5); // byte
      } else if (this.unk13 == 0x13) {
         this.unk18 = stream.readBits(8) - 1; // byte
      }
   }
}

class LoadedForgeObject {
   loadPosition(stream, mapBounds) {
      const bitcount = 21;
      //
      let rbp60 = [0, 0, 0];
      let a = stream.readBits(1); // can't understand how this is used
      if (a) {
         if (mapBounds) {
            _sub4DC8E0(bitcount, mapBounds, rbp60);
         } else {
            //
            // TODO: initialize rbp60 to some static values set by sub$+4DC4F0, which is 
            // ultimately called by a virtual member function somewhere
            //
         }
      } else {
         if (mapBounds) {
            _sub4DC8E0(bitcount, mapBounds, rbp60);
         } else {
            if (!stream.readBits(1)) {
               let b = stream.readBits(2, false);
               if (b != -1) {
                  //
                  // TODO: call _sub4DC8E0 but with different args than usual
                  //
               }
            }
         }
      }
      this.position.x = stream.readBits(rbp60[0], false); // compressed float
      this.position.y = stream.readBits(rbp60[1], false); // compressed float
      this.position.z = stream.readBits(rbp60[2], false); // compressed float   
      //
      let range = mapBounds.x.max - mapBounds.x.min;
      let xmm3, xmm1;
      this.position.x = (0.5 + this.position.x) * (range / (1 << rbp60[0])) + mapBounds.x.min;
      //
      range = mapBounds.y.max - mapBounds.y.min;
      this.position.y = (0.5 + this.position.y) * (range / (1 << rbp60[1])) + mapBounds.y.min;
      //
      range = mapBounds.z.max - mapBounds.z.min;
      this.position.z = (0.5 + this.position.z) * (range / (1 << rbp60[2])) + mapBounds.z.min;
   }
   loadAxisAngleAxis(stream) {
      //
      // this is supposed to produce floats, but JS stores all numbers as doubles, 
      // so there will be *slight* inaccuracies
      //
      function normalizeVector(coords) {
         let xmm2 = Math.sqrt(Math.fround(coords[0]*coords[0] + coords[1]*coords[1] + coords[2]*coords[2]));
         let xmm1 = Math.abs(xmm2);
         if (0.0001 <= xmm1) {
            xmm1 = 1.0 / xmm2;
            coords[0] *= xmm1;
            coords[1] *= xmm1;
            coords[2] *= xmm1;
            for(let i = 0; i < 3; ++i)
               coords[i] = Math.fround(coords[i]);
         }
      }
      //
      const lookup_table = [
         [0x0000A, 0x002], [0x00015, 0x003],
         [0x0002A, 0x005], [0x00055, 0x008],
         [0x000AA, 0x00C], [0x00155, 0x011],
         [0x002AA, 0x019], [0x00555, 0x023],
         [0x00AAA, 0x033], [0x01555, 0x048],
         [0x02AAA, 0x067], [0x05555, 0x092],
         [0x0AAAA, 0x0D0], [0x15555, 0x126],
         [0x2AAAA, 0x1A1], [0x55555, 0x24E],
         [0xAAAAA, 0x343], [0x155555, 0x9D04],
         [0xAAAAAA, 0x687], [0x555555, 0x93B],
         [0x2AAAAAA, 0x1A1F], [0x5555555, 0x24F4],
         [0xAAAAAAA, 0x3440]
      ];
      const bitcount = 20;
      //
      let bits = stream.readBits(bitcount, false);
      //
      let r9   = bitcount - 6;
      let r11  = Math.floor(bits / lookup_table[r9][0]);
      let r8   = lookup_table[r9][0] * r11;
      let ebx  = bits - r8;
      //
      let eax = Math.floor(ebx / lookup_table[r9][1]);
      let ecx = lookup_table[r9][1] * eax;
      let xmm2 = eax;
      eax *= 2;
      ebx -= ecx;
      ecx = lookup_table[r9][1] - 1;
      let xmm0 = ecx;
      --ecx;
      //
      let xmm1 = Math.fround(2.0 / xmm0);
      xmm2 = Math.fround(xmm2 * xmm1 - 1.0);
      xmm1 *= 0.5;
      xmm2 += xmm1;
      { // JS uses doubles but these calculations are singles
         xmm0 = Math.fround(xmm0);
         xmm1 = Math.fround(xmm1);
         xmm2 = Math.fround(xmm2);
      }
      if (eax == ecx)
         xmm2 = 0;
      //
      ecx  = lookup_table[r9][1] - 1;
      xmm1 = ebx;
      xmm0 = ecx;
      eax = ebx * 2;
      --ecx;
      let xmm3 = Math.fround(2.0 / xmm0);
      xmm1 *= xmm3;
      xmm3 *= 0.5;
      xmm1 -= 1.0;
      xmm1 += xmm3;
      { // JS uses doubles but these calculations are singles
         xmm0 = Math.fround(xmm0);
         xmm1 = Math.fround(xmm1);
         xmm2 = Math.fround(xmm2);
         xmm3 = Math.fround(xmm3);
      }
      if (eax == ecx)
         xmm1 = 0;
      //
      let vector = [this.unk20, this.unk24, this.unk28];
      if (r11 <= 5) {
         switch (r11) {
            case 0:
               vector[0] = 1;
               vector[1] = xmm2;
               vector[2] = xmm1;
               break;
            case 3:
               vector[0] = -1;
               vector[1] = xmm2;
               vector[2] = xmm1;
               break;
            case 1:
               vector[0] = xmm2;
               vector[1] = 1;
               vector[2] = xmm1;
               break;
            case 4:
               vector[0] = xmm2;
               vector[1] = -1;
               vector[2] = xmm1;
               break;
            case 2:
               vector[0] = xmm2;
               vector[1] = xmm1;
               vector[2] = 1;
               break;
            case 5:
               vector[0] = xmm2;
               vector[1] = xmm1;
               vector[2] = -1;
               break;
            default:
               vector[0] = 0;
               vector[1] = 0;
               vector[2] = 1;
         }
      }
      normalizeVector(vector);
      this.axisAngleAxis.x = vector[0];
      this.axisAngleAxis.y = vector[1];
      this.axisAngleAxis.z = vector[2];
   }
   loadAxisAngleAngle(raw) {
      const PI = 3.1415927; // float
      //
      let xmm1 = raw * 0.0003834952 - PI + 0.0001917476;
      this.rawAxisAngleAngle = raw;
      this.rawAxisAngleRads  = xmm1;
      //
      let xmm6 = Math.fround(xmm1);
      let rsp20 = new MVVector();
      let rsp30 = new MVVector();
      (function(a, b, c) {
         let xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8;
         //
         xmm0 = 0;
         xmm4 = a.z; // correct, notwithstanding precision differences
         xmm7 = a.z * xmm0; // correct
         xmm6 = a.x * xmm0; // correct
         xmm0 = a.x + a.y + xmm7; // correct, notwithstanding precision differences
         xmm1 = xmm6 + a.y + xmm7; // correct
         if (Math.abs(xmm1) > Math.abs(xmm0)) {
            xmm0 = xmm6 - a.y;
            b.y = a.z - xmm6;
            b.x = a.y - xmm7;
         } else {
            xmm6 -= xmm7; // correct
            xmm0 = a.y - a.x; // slightly wrong, possibly explainable by precision differences
            b.y = xmm6;
            b.x = a.z - (a.y * 0); // correct
         }
         b.z = xmm0;
         b.normalize(); // correct, notwithstanding precision differences
         xmm5 = a.x;
         xmm1 = b.x;
         xmm3 = a.z;
         xmm0 = b.x * a.y; // correct
         xmm8 = a.x * b.y - (b.x * a.y); // correct
         xmm3 *= b.y; // correct
         xmm2 = xmm3 * b.x; // wrong, possibly explainable by precision differences
         xmm0 = b.z * a.x; // correct, notwithstanding precision differences
         xmm4 = b.z * a.y - xmm3; // correct
         c.z = xmm8;
         xmm2 -= xmm0; // correct, but this would be due to xmm2's incredibly small value initially
         c.x = xmm4;
         c.y = xmm2;
         c.normalize();
      })(this.axisAngleAxis, rsp20, rsp30);
      // good up to here?
      this.rotation.x = rsp20.x;
      this.rotation.z = rsp20.z;
      //
      let xmm7;
      let xmm0;
      /*
         bool result = (xmm6 == PI);
         if (isNaN(xmm6) || isNaN(PI)) { // when would this ever be true?
            result = (xmm6 == -PI);
            if (isNaN(xmm6) || isNaN(-PI)) // when would this ever be true?
               result = false;
               //
               // The comparisons use UCOMISS, and the weird-ass NaN checks are JP branches. 
               // UCOMISS should only set the parity flag (PF) if either or both operands 
               // are NaN, but the code here is written as if the *constant* is the one 
               // that might be NaN.
         }
         if (result) {
            xmm7 = 0;
            xmm0 = -1.0;
         } else {
            xmm7 = sinf(xmm6);
            xmm0 = cosf(xmm6);
         }
      */
      if (xmm6 == PI) {
         xmm7 = 0;
         xmm0 = -1.0;
      } else {
         xmm7 = Math.sin(xmm6);
         xmm0 = Math.cos(xmm6);
      }
      (function(a, b, xmm2, xmm3) {
         a.enforce_single_precision();
         b.enforce_single_precision();
         let cx = Math.fround(xmm3); // correct, notwithstanding precision differences
         let cy = Math.fround(xmm2); // correct, notwithstanding precision differences
         //
         let xmm5 = b.y;
             xmm3 = Math.fround((b.x * a.x) + (b.y * a.y) + (b.z * a.z)) * Math.fround(1.0 - cx); // wrong digits and sign, right mag
         let xmm4 = a.x * cx; // correct
         let xmm8 = a.y * cx; // correct? it's positive zero in JS but negative in haloreach.dll
         let xmm7 = (b.x * xmm3) + (a.x * cx); // wrong sign and significant digits, right approx. magnitude
         let xmm1 = ((a.z * b.x) - (b.z * a.x)) * cy; // correct
         let xmm9 = a.z * cx; // correct
         // and discard xmm10
         let xmm0 = a.z * xmm5;
             xmm2 = (a.x * b.y) - (b.x * a.y) * cy;
         let xmm6 = ((b.z * a.y) - xmm0) * cy;
         // and discard xmm11
         xmm7 -= xmm6; // wrong
         // and discard xmm6
         a.x = xmm7;
         // and discard xmm7
         xmm0 = (xmm3 * b.y) + xmm8 - xmm1;
         // and discard xmm8
         a.y = xmm0;
         // and discard xmm9
         xmm3 = b.z + xmm9 - xmm2;
         a.z = xmm3;
      })(this.rotation, this.axisAngleAxis, xmm7, xmm0);
      this.rotation.normalize();
   }
   constructor(stream, mapBounds, owner) {
      this.loaded = false;
      this.owner  = owner || null;
      //
      this.unk00 = 0;
      //
      // objectSubcat
      //    In the MAP file, look at the SCNR (scenario) tag's Sandbox Palette. This 
      //    structure defines categories (Sandbox Palette), subcategories (Entries), 
      //    and objects in subcategories (Entry Variants); when an object is nested 
      //    directly under a category, this works by having a subcategory with a name 
      //    and only a single contained object. Forge count and price limits are 
      //    defined per subcategory. Treat all this as a flat array of subcategories, 
      //    and this is the index of a subcategory.
      //
      this.objectSubcat = 0xFFFF; // 02
      //
      this.unk04 = -1; // dword
      this.position = new MVVector(); // 08, 0C, 10
      this.rotation = new MVVector(); // 14, 18, 1C // possibly local-forward unit vector
      this.axisAngleAxis = new MVVector(); // 20, 24, 28 // most likely local-up unit vector
      this.unk2C = -1; // word
      //
      // objectSubtype
      //    Index of an object within a subcategory.
      //
      //    As an example, Coliseum Walls on Tempest have objectSubcat 71 and 
      //    objectSubtype 8.
      //
      this.objectType = 0; // 2E
      //
      this.unk2F = 0; // padding?
      this.unk30 = null; // struct; all remaining fields are its members
      /*//
      this.unk30 = 0; // int64?
      this.unk38 = 0; // int64?
      this.unk40 = 0;
      this.unk41 = 0; // padding?
      this.unk42 = 0; // padding?
      this.unk43 = 0;
      this.unk44 = 0xFFFF;
      this.unk46 = 0; // padding?
      this.unk48 = 0;
      this.unk4A = 0xFF;
      this.unk4B = 0;
      //*/
      if (!stream)
         return;
      //
      let presence = stream.readBits(1);
      if (!presence)
         return;
      this.loaded = true;
      this.unk00 = stream.readBits(2, false);
      if (!stream.readBits(1))
         this.objectSubcat = stream.readBits(8, false);
      let absence = stream.readBits(1);
      if (!absence)
         this.objectType = stream.readBits(5, false); // value in the range of [0, 31]
      else
         this.objectType = 0xFF; // -1
      this.loadPosition(stream, mapBounds);
      if (stream.readBits(1)) {
         this.axisAngleAxis.x = 0;
         this.axisAngleAxis.y = 0;
         this.axisAngleAxis.z = 1;
         //
         // if this is an axis-angle unit vector, then this would mean that the angle 
         // is just yaw.
         //
      } else {
         this.loadAxisAngleAxis(stream);
      }
      let a = stream.readBits(14, false); // TODO: processing
      this.loadAxisAngleAngle(a);
      this.unk2C = stream.readBits(10, false) - 1;
      this.unk30 = new LFOUnk30(stream);
   }
   get forgeLabel() {
      if (!this.owner)
         return void 0;
      if (!this.unk30)
         return null;
      let index = this.unk30.forgeLabelIndex;
      if (index < 0)
         return null;
      let entry = this.owner.forgeLabels.strings[index];
      if (!entry)
         return NaN;
      return entry.content;
   }
   get rotationDegrees() {
      //
      // NOTE: Rotations shown here may not be the same as those seen in Forge in 
      // cases of equivalent angles. For example, roll 0 pitch 0 yaw -180 is the 
      // same angle as roll 180 pitch 180 yaw 0.
      //
      // NOTE: this is also wrong anyway since i don't know bungie's euler 
      // conventions
      //
      // Bungie's Euler conventions: lefthanded. X is forward, Z is vertical.
      // The game uses inconsistent axes for positions and rotations: X is 
      // pitch, not roll; Y is yaw, not pitch; and Z is roll, not yaw.
      //
      let aa  = this.axisAngleAxis;
      let rad = this.rawAxisAngleRads;
      let rot = new MVVector();
      rot.z = Math.atan2(aa.y * Math.sin(rad) - aa.x * aa.z * (1 - Math.cos(rad)), 1 - (aa.y*aa.y + aa.z*aa.z) * (1 - Math.cos(rad)));
      rot.y = Math.asin(aa.x * aa.y * (1 - Math.cos(rad)) + aa.z * Math.sin(rad));
      rot.x = Math.atan2(aa.x * Math.sin(rad) - aa.y * aa.z * (1 - Math.cos(rad)), 1 - (aa.x*aa.x + aa.z*aa.z) * (1 - Math.cos(rad)));
      let conv = function(r) { return r * 180 / Math.PI; }
      rot.x = conv(rot.x);
      rot.y = conv(rot.y);
      rot.z = conv(rot.z);
      return rot;
   }
}

class MapVariant {
   constructor(stream) {
      stream.endianness = ENDIAN_LITTLE;
      this.signature = stream.readString(4);
      if (this.signature != "mvar")
         throw new Error(`MapVariant expected signature "mvar"; got "${this.signature}".`);
      let size = stream.readUInt32();
      if (size != 0x7028) {
         size = _byteswap_ulong(size);
         if (size != 0x7028)
            throw new Error(`MapVariant expected size 0x7028; got 0x${_byteswap_ulong(size).toString(16)} or 0x${size}.toString(16).`);
      }
      this.chunkVersion = _byteswap_ushort(stream.readUInt16()); // 08
      this.chunkFlags   = _byteswap_ushort(stream.readUInt16()); // 0A
      //
      this.hashSHA1 = stream.readBytes(0x14);
      this.hashContentLength = stream.readBytes(0x4);
      //
      {  // GameVariantHeader // header - same data as CHDR but fields aren't byte-aligned here
         let o = this.header = {};
         o.type       = stream.readBits(4, BIT_ORDER_UNKNOWN) - 1; // gvar_content_type
            // ^ "None" is encoded as 0 instead of -1 in mpvr; the whole enum is shifted up by 1
            //   when written to the file.
         // Note: As of the previous field, we are byte-aligned again.
         o.fileLength = _byteswap_ulong(stream.readUInt32());
         stream.reportOffset();
         o.unk08      = _byteswap_uint64(stream.readUInt64());
         o.unk10      = _byteswap_uint64(stream.readUInt64());
         o.unk18      = _byteswap_uint64(stream.readUInt64());
         o.unk20      = _byteswap_uint64(stream.readUInt64());
         stream.reportOffset();
         o.activity   = stream.readBits(3, false) - 1;
            // ^ "None" is encoded as 0 instead of -1 in mpvr; the whole enum is shifted up by 1
            //   when written to the file.
         o.gameMode   = stream.readBits(3, false);
         o.engine     = stream.readBits(3, false);
         o.mapID      = stream.readBits(32, false); // the ID of the map we're built on
         o.engineCategoryIndex = stream.readBits(8, false); // TODO: we need to sign-extend it from one byte to an SInt32
         o.createdBy   = new VariantContentAuthor();
         o.createdBy.parseBits(stream);
         o.modifiedBy  = new VariantContentAuthor();
         o.modifiedBy.parseBits(stream);
         let endianness = stream.endianness;
         stream.endianness = ENDIAN_BIG; // KSoft.Tool always uses big-endian, but MCC may be little-endian for this?
         o.title       = stream.readWidecharString(128, true);
         o.description = stream.readWidecharString(128, true);
         if (o.activity == 2)
            o.hopperID = stream.readUInt16(); // TODO: TEST ME (how?)
         else
            o.hopperID = null;
         if (this.gameMode == 1) {
            this.unk02A0 = stream.readBits(8, false);
            this.unk02A1 = stream.readBits(2, false);
            this.unk02A2 = stream.readBits(2, false);
            this.unk02A3 = stream.readBits(8, false);
            this.unk02A4 = stream.readBits(32, false);
         } else if (this.gameMode == 2) {
            this.unk02A0 = stream.readBits(2, false);
            this.unk02A4 = stream.readBits(32, false);
         }
      }
      this.unk02B0 = stream.readBits(8, false);
      if (this.unk02B0 != 0x1F)
         console.warn("unk2B0 != 0x1F; Reach would abort with an error here");
      this.unk02DC = stream.readBits(32, false);
      this.unk02E0 = stream.readBits(32, false);
      this.unk02B2 = stream.readBits(9, false);
      this.unk02B4 = stream.readBits(32, false); // another copy of the map ID
      this.unk02D9 = stream.readBits(1, false);
      this.unk02DA = stream.readBits(1, false);
      //
      this.boundingBox = {  // 02B8, float[6]
         x: { min: 0, max: 0 },
         y: { min: 0, max: 0 },
         z: { min: 0, max: 0 },
      };
      this.boundingBox.x.min = stream.readFloat({ endianness: ENDIAN_BIG }); // float
      this.boundingBox.x.max = stream.readFloat({ endianness: ENDIAN_BIG }); // float
      this.boundingBox.y.min = stream.readFloat({ endianness: ENDIAN_BIG }); // float
      this.boundingBox.y.max = stream.readFloat({ endianness: ENDIAN_BIG }); // float
      this.boundingBox.z.min = stream.readFloat({ endianness: ENDIAN_BIG }); // float
      this.boundingBox.z.max = stream.readFloat({ endianness: ENDIAN_BIG }); // float
      this.unk02D0 = stream.readBits(32, false);
      this.unk02D4 = stream.readBits(32, false);
      //
      this.forgeLabels = {
         stringCount: 0,
         strings: []
      };
      {
         let _fl = this.forgeLabels;
         //
         _fl.stringCount = stream.readBits(9, false); // value in the range of [0, 511], though there may be a lower limit
         for(let i = 0; i < _fl.stringCount; ++i) {
            _fl.strings[i] = { offset: 0, content: "" };
            let presence = stream.readBits(1);
            if (!presence)
               continue;
            _fl.strings[i].offset = stream.readBits(12, false);
         }
         if (_fl.stringCount > 0) {
            let dataLength   = stream.readBits(13, false);
            _fl.dataLength = dataLength;
            let isCompressed = stream.readBits(1);
            _fl.isCompressed = isCompressed;
            let buffer = [];
            if (isCompressed) {
               let compressed = [];
               let compSize   = stream.readBits(13, false);
               for(let i = 0; i < compSize; i++)
                  compressed[i] = stream.readByte();
               if (pako) {
                  //let bin  = new Uint8Array(compressed);
                  let bin  = new Uint8Array(compressed.slice(4)); // skip the zlib header's uncompressed size
try {
                  let data = pako.inflate(bin);
                  //console.log(`inflated data to ${data.length} bytes; ${uncompressedSize} expected`);
                  buffer = data;
                  if (buffer.length != dataLength)
                     console.warn(`Expected ~${dataLength} bytes; got ${buffer.length} (compressed into ${compSize})`);
} catch (e) { console.log(e); return; }
               } else
                  buffer = null;
            } else {
               for(let i = 0; i < dataLength; i++)
                  buffer[i] = stream.readByte();
               buffer = new Uint8Array(buffer);
            }
            _fl.buffer = buffer;
            if (buffer) {
               let size = buffer.length;
               for(let i = 0; i < _fl.stringCount; ++i) {
                  let s = _fl.strings[i];
                  for(let k = s.offset; k < size; ++k) {
                     if (buffer[k] == 0)
                        break;
                     s.content += String.fromCharCode(buffer[k]);
                  }
               }
            }
         }
      }
      if (this.unk2B2 > 0x100)
         console.warn("unk2B2 > 0x100; Halo: Reach would abort with a load failure at this point");
      this.forgeObjects = [];
      for(let i = 0; i < 0x28B; ++i) {
         this.forgeObjects[i] = new LoadedForgeObject(stream, this.boundingBox, this);
      }
      //
      // everything above confirmed accurate via memory inspection of haloreach.dll
      //
      this.unkD630 = [];
      for(let i = 0; i < 0x100; ++i) {
         this.unkD630[i] = { unk00: 0, unk01: 0, unk02: 0 };
         if (i < this.unk2B2) {
            this.unkD630[i].unk00 = stream.readBits(8, false);
            this.unkD630[i].unk01 = stream.readBits(8, false);
            this.unkD630[i].unk02 = stream.readBits(8, false);
         }
      }
   }
}