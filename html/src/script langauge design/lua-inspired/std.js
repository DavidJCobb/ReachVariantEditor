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