class MParseError {
   constructor(item, message) {
      this.item    = item;
      this.message = message;
   }
}
class MValidator {
   constructor() {
      this.errors  = [];
      this.suspend = false;
      this.pending = []; // pending errors
      //
      this.state = {
         aliases: [],
      };
   }
   report(item, message) {
      let error = new MParseError(item, message);
      if (this.suspend) {
         this.pending.push(error);
         return;
      }
      this.errors.push(error);
   }
   //
   set_errors_suspended(state, discard) {
      this.suspend = !!state;
      if (!state) {
         if (!discard)
            this.errors = this.errors.concat(this.pending);
         this.pending = [];
      }
   }
   //
   trackAlias(alias) {
      this.state.aliases.push(alias);
   }
   untrackAlias(alias) {
      let list  = this.state.aliases;
      let index = list.indexOf(alias);
      if (index > 0)
         list.splice(index, 1);
   }
   untrackAliases(aliases) {
      this.state.aliases = this.state.aliases.filter(function(e) { return aliases.indexOf(e) < 0; });
   }
   forEachTrackedAlias(functor) {
      let list = this.state.aliases;
      let size = list.length;
      for(let i = size - 1; i >= 0; i--) { // iterate in reverse order so newer aliases properly shadow older ones
         let alias = list[i];
         if (functor(alias))
            break;
      }
   }
}