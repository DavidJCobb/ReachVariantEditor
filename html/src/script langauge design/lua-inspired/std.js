function _make_enum(list) {
   let out = {};
   for(let entry of list)
      out[entry] = Symbol(entry);
   return out;
}

Array.prototype.last = function() {
   return this[this.length - 1];
}
Array.prototype.item = function(i) {
   if (i < 0)
      return this[this.length + i];
   return this[i];
}

function assert(cond, message, extra) {
   if (!cond) {
      if (extra) // Firefox's debugger is awful and often "loses" the actual stack frame that threw, so use this to dump pertinent locally-scoped objects
         console.warn(extra);
      throw new Error(`Assertion failed: ${message}.`);
   }
}

class Collection {
   constructor(values) {
      this.list = values;
      for(let i in values)
         this[i] = values[i];
   }
   find(functor) {
      let list = this.list;
      for(let i in list) {
         let item = list[i];
         if (functor(item))
            return item;
      }
      return void 0;
   }
   [Symbol.iterator]() {
      let list = this.list;
      let keys = Object.keys(list);
      let n    = 0;
      return {
         next() {
            return n < keys.length ? { value: list[keys[n++]], done: false } : { done: true };
         }
      };
   }
}