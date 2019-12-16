let g_megaloConditions = [];

class MegaloConditionFunction {
   constructor(id, details, args) {
      this.id   = id;
      this.name = details.name;
      this.desc = details.desc || "";
      this.line = details.line;
      this.args = args || [];
      //
      if (g_megaloConditions[id])
         throw new Error("Condition ID " + id + " is already in use!");
      g_megaloConditions[id] = this;
   }
}

new MegaloConditionFunction(0, { name: "None" });

new MegaloConditionFunction(1, {
   name: "Compare Variables",
   desc: "Compare the values of two variables.",
   line: "Variable %1 is %invert %3 to variable %2.",
}, [
   { type: "var", name: "a" },
   { type: "var", name: "b" },
   { type: "compare-operator", name: "operator" },
]);

new MegaloConditionFunction(2, {
   name: "Is In Shape",
   desc: "Test whether an object is inside of another object's Shape.",
   line: "Object %1 is %invert inside of the Shape of %2.",
}, [
   { type: "object", name: "object" },
   { type: "object", name: "boundary" },
]);

new MegaloConditionFunction(3, {
   name: "Player Killer Type",
   line: "%1's killer type was %invert %2.",
}, [
   { type: "player",   name: "dead player" },
   { type: "killer-type-flags", name: "flags" },
]);

new MegaloConditionFunction(4, {
   name: "Compare Team Disposition",
   line: "Team %1 %invert has a %3 disposition with team %2.",
}, [
   { type: "team", name: "a" },
   { type: "team", name: "b" },
   { type: "team-disposition", name: "disposition" },
]);

new MegaloConditionFunction(5, {
   name: "Timer Is Zero",
   line: "Timer %1 has %invert reached zero.",
}, [
   { type: "timer", name: "timer" },
]);

new MegaloConditionFunction(6, {
   name: "Object Type",
   line: "Object %1 is %invert of type %2.",
}, [
   { type: "object",      name: "object" },
   { type: "object-type", name: "type" },
]);

new MegaloConditionFunction(7, {
   name: "Team Has Players",
   line: "Team %1 %invert has one or more players on it.",
}, [
   { type: "team", name: "team" },
]);

new MegaloConditionFunction(8, {
   name: "Object Out Of Bounds",
   line: "Object %1 is %invert out of bounds.",
}, [
   { type: "object", name: "object" },
]);

new MegaloConditionFunction(9, {
   name: "Deprecated 09",
   line: "This function always returns false.", // always false, normally; is it always true if inverted?
}, [
   { type: "player", name: "player" },
]);

new MegaloConditionFunction(10, {
   name: "Player Assisted Kill Of",
   line: "Player %1 %invert assisted in the killing of player %2.",
}, [
   { type: "player", name: "attacker" },
   { type: "player", name: "target" },
]);

new MegaloConditionFunction(11, {
   name: "Object Matches Filter",
   line: "Object %1 %invert matches filter %2.",
}, [
   { type: "object",        name: "object" },
   { type: "object-filter", name: "filter" },
]);

new MegaloConditionFunction(12, {
   name: "Player Is Not Respawning",
   line: "Player %1 is not %invert currently respawning.",
}, [
   { type: "player", name: "player" },
]);

new MegaloConditionFunction(13, {
   name: "Armor Ability Is In Use",
   line: "Armor Ability %1 is %invert in use by whoever has it equipped.",
}, [
   { type: "object", name: "object" },
]);

new MegaloConditionFunction(14, {
   name: "Species Is Spartan",
   line: "Player %1 is %invert a Spartan.",
}, [
   { type: "player", name: "player" },
]);

new MegaloConditionFunction(15, {
   name: "Species Is Elite",
   line: "Player %1 is %invert an Elite.",
}, [
   { type: "player", name: "player" },
]);

new MegaloConditionFunction(16, {
   name: "Species Is Monitor",
   line: "Player %1 is %invert a Monitor.",
}, [
   { type: "player", name: "player" },
]);

new MegaloConditionFunction(17, {
   name: "Unknown 17",
   line: "This may be checking whether the match is %invert taking place in Matchmaking.",
});

