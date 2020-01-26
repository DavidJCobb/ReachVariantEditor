class MParseError {
   constructor(item, message) {
      this.item    = item;
      this.message = message;
   }
}
class MValidator {
   constructor() {
      this.errors = [];
      //
      this.state = {
         aliases: [],
      };
   }
   report(item, message) {
      this.errors.push(new MParseError(item, message));
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
      for(let alias of this.state.aliases)
         if (functor(alias))
            break;
   }
}