class TypedEnumInstance {
   constructor(type, value) {
      this.type  = type;
      this.value = value != void 0 ? value : type.default || 0;
   }
   set(v, force) {
      if (!force) {
      }
      this.value = v;
   }
   [Symbol.toStringTag]() {
      return this.type.name;
   }
   [Symbol.toPrimitive](hint) {
      switch (hint) {
         case "number":
         case "default":
            return this.value;
         case "string":
            return this.value + "";
      }
   }
}
class TypedEnum {
   constructor(details, values) {
      this.details = details;
      this.values = [];
      for(let f in values)
         if (values.hasOwnProperty(f))
            this.values.push({ name: f, value: values[f] });
      this.values.sort(function(a, b) { return a.value < b.value; });
   }
   forEachValue(functor) {
      for(let value of this.values)
         if (functor(value.name, value.value))
            return;
   }
   make(v) {
      return new TypedEnumInstance(this, v);
   }
}

class TypedBitfieldInstance {
   constructor(type, value) {
      this.type  = type;
      this.value = value != void 0 ? value : type.default || 0;
      //
      type.forEachFlag((function(name, value) {
         Object.defineProperty(this, name, {
            enumerable:   true,
            configurable: true,
            get() { return (this.value & value) != 0; },
            set(v) { this.modify_bits(value, v); return v; },
         });
      }).bind(this));
   }
   set(v) {
      this.value = v;
   }
   modify_bits(mask, state) {
      if (state)
         this.value |= mask;
      else
         this.value &= ~mask;
   }
   toString() {
      let str = "<";
      let unknowns = this.value;
      //
      let first = true;
      this.type.forEachFlag(function(name, mask) {
         if (this.value & mask) {
            if (!first)
               str += ", ";
            first = false;
            str += name;
            //
            unknowns &= ~mask;
         }
      });
      if (unknowns) {
         let i = 0;
         do {
            let bit = 1 << i;
            if (unknowns & bit) {
               if (!first)
                  str += ", ";
               first = false;
               str += "Unknown " + i;
               unknowns &= ~bit;
            }
         } while (unknowns && i < 32);
      }
      if (!first)
         str += "<no flags set>";
      str += ">";
      return str;
   }
   [Symbol.toStringTag]() {
      return this.type.name;
   }
   [Symbol.toPrimitive](hint) {
      switch (hint) {
         case "number":
         case "default":
            return this.value;
         case "string":
            return this.toString();
      }
   }
}
class TypedBitfield {
   constructor(details, flags) {
      this.details = details;
      this.flags = [];
      for(let f in flags)
         if (flags.hasOwnProperty(f))
            this.flags.push({ name: f, mask: flags[f] });
      this.flags.sort(function(a, b) { return a.mask < b.mask; });
   }
   forEachFlag(functor) {
      for(let flag of this.flags)
         if (functor(flag.name, flag.mask))
            return;
   }
   make(v) {
      return new TypedBitfieldInstance(this, v);
   }
}