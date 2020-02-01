Array.prototype.last = function() {
   return this[this.length - 1];
}
Array.prototype.item = function(i) {
   if (i < 0)
      return this[this.length + i];
   return this[i];
}

function assert(cond, message) {
   if (!cond)
      throw new Error(`Assertion failed: ${message}.`);
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
      let keys = Object.keys(this.list);
      let n    = 0;
      return {
         next() {
            return i < keys.length ? { value: this.list[keys[n++]], done: false } : { done: true };
         }
      };
   }
}