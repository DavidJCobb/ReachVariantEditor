class MParseError {
   constructor(item, message) {
      this.item    = item;
      this.message = message;
   }
}
class MValidator {
   constructor() {
      this.errors = [];
   }
   report(item, message) {
      this.errors.push(new MParseError(item, message));
   }
}