#include "ui/main_window.h"
#include <QtWidgets/QApplication>

#include "game_variants/base.h"
#include "helpers/endianness.h"
#include "helpers/files.h"
#include "helpers/stream.h"
#include "formats/sha1.h"
#include "services/ini.h"

int main(int argc, char *argv[]) {
   ReachINI::get().load();
   //
   QApplication a(argc, argv);
   ReachVariantTool w;
   w.show();
   return a.exec();
}

//
// CURRENT PLANS:
//
//  = [OPTIONAL] Consider renaming all of the opcode arg types: instead of using classnames 
//    prefixed with the superclass name e.g. Megalo::OpcodeArgValueObject, consider nesting 
//    them under a namespace with simplified names e.g. Megalo::types::object.
//
//  - [OPTIONAL] If we can modify references to indexed data to optionally use aliases if 
//    the Decompiler& so wills, then so much the better; however, this would require extend-
//    ing the generic variable handling so that a scope-indicator definition can specify a 
//    custom decompile functor.
//
//  - Start work on the compiler.
//
//     = RENAME "ABSTRACT PROPERTIES" TO "ACCESSORS." NOTE THAT THIS TERMINOLOGY CHANGE IS 
//       ALREADY USED IN THE NEXT TO-DO LIST ITEM.
//
//        - CAN WE FIND A BETTER NAME THAN "STATIC" FOR THINGS LIKE player[0], TOO?
//
//     - SHORT-TERM PLANS
//
//        - VariableReference string constructor: we need to catch the following syntax 
//          errors:
//
//           - name[1 2]
//           - name[ ]   // we check for an empty index string but not a whitespace-only one
//
//        - VariableReference string constructor: we need to trim the index string when 
//          committing a RawPart.
//
//        - Compiling assignments
//
//           - If the lefthand side is an accessor, then compile the accessor name, strip 
//             the accessor from the VariableReference, and compile the VariableReference 
//             by creating the assignment LHS argument and passing it.
//
//        - Compiling comparisons
//
//           - Compiler::_applyConditionModifiers
//
//        - Get rid of the Script::Comparison class and just use Script::Statement.
//
//           - We can do this once we've actually written the code to compile comparisons 
//             into Opcodes. The Compiler class already has a (negate_next_condition) bool, 
//             and we can add an enum for the current condition joiner, and use that and 
//             the bool when compiling; the Statement itself doesn't need to retain the 
//             "negated" bool (which is the only purpose of the Comparison class).
//
//     - Script::VariableReference::_transclude still needs to be coded. It is responsible 
//       for replacing a Part with the contents of an Alias.
//
//     - VariableReference should detect and disallow triply-nested variables (var.var.var).
//
//     - Alias resolution needs to be completed, including disallowing the shadowing of all 
//       built-ins, and allowing an alias to refer to a built-in.
//
//        - This includes handling enum values and other such content. To accomplish this, 
//          we'll need to make it so that OpcodeArgTypeinfo can specify a DetailedEnum or 
//          DetailedFlags (currently it can specify an initializer list of const char*s).
//
//     - The compiler needs code to compile non-function-call assignments and comparisons.
//
//        - This includes abstract getters and setters, which will require additional 
//          validation in some cases (e.g. no += for a getter unless that's supported via 
//          a defined OpcodeFuncToScriptMapping::arg_operator).
//
//           - I'm pretty sure all property setters support all operators e.g. += by way 
//             of OpcodeFuncToScriptMapping::arg_operator, but we should still have code 
//             for operators which lack that support.
//
//     - The compiler needs code to compile a top-level Block that has just closed.
//
//        - We can't compile nested blocks when they close because we want a consistent 
//          trigger order with Bungie's output; compiling nested blocks as they close will 
//          result in their triggers preceding the triggers of their containing blocks.
//
//     - The compiler needs code to parse variable declarations, including scope-relative 
//       declarations e.g. (declare player.number[0]).
//
//     - Compiling should fail if multiple triggers use the same event type (via the "on" 
//       keyword; requires that the Compiler keep track of what events have been used so 
//       far).
//
//     - We're gonna have to take some time to work out how to negate if-statements that 
//       mix OR and AND, in order to make elseif and else work. Fortunately, we only need 
//       to know that when it comes time to actually compile the blocks... assuming we 
//       don't decide to just drop support for else(if)s at that point. (Think about it: 
//       first of all, they abstract away conditions, so you can't see the full "cost" 
//       of your code; and second, there's no code to detect equivalent ifs and decompile 
//       them to else(if)s yet. We should support else(if)s if we can but they aren't by 
//       any means essential; we've already decided that conciseness is not required.)
//
//     - Currently every parse error is a fatal error, throwing an exception and stopping 
//       all further parsing. However, 90% of these should be non-fatal errors instead. 
//       We need to build a system for logging non-fatal errors, and then begin converting 
//       fatal errors over.
//
//        - Basically, if something is incorrect but the parser is still capable of 
//          understanding it (e.g. var.var.var as opposed to a misplaced keyword), then 
//          it should be a non-fatal error.
//
//        - Some non-fatal errors will require additional handling. For example, if a 
//          function fails to parse (unrecognized name, bad argument(s), etc.), then we 
//          will need code to log a non-fatal error and skip the function's argument list.
//
//        = List of errors that should be non-fatal:
//
//           - Unrecognized variable reference (or other "resolve" failures)
//           - Some kinds of invalid variable references (before the "resolve" step)
//              - Index is not an integer, or is an alias of a non-integer
//           - Unrecognized function name
//           - Bad function arguments
//           - Bad function return type
//           - Bad operator (e.g. comparison operator outside of an if-condition)
//           - Property setter that only supports operator "=" invoked with another operator
//           - Assigning the return value of a function call to a property setter
//           - Assigning to a constant integer or other read-only value
//           - Alias name that shadows a built-in
//           - Alias name that is an integer literal
//           - Invalid event name for "on" keyword
//           - "On" keyword used for a nested trigger
//           - Multiple triggers using the same event type via "on"
//
//     = AUDITING
//
//        - Exception safety for anything that gets heap-allocated.
//
//        - DO A PROJECT-WIDE SEARCH FOR THE WORD "TODO".
//
//  - Decompiler: work on a better text editor in-app, with horizontal scrolling, line 
//    numbers, syntax highlighting, code folding, etc..
//
//  - Script editor window: MPVR space usage meter: we should also show absolute counts 
//    out of maximums for triggers, conditions, actions, number of strings, and perhaps 
//    other data items if we use two or three columns as well. We could even correlate 
//    these to blocks on the bar if we give them distinct colors (e.g. a colored box 
//    before each limit like "[ ] Conditions: 317 / 512").
//
//     - WE MESSED UP WITH PLAYER TRAITS. We need the maximum possible bitcount for a 
//       set of player traits, not the current bitcount, because traits are alterable 
//       in-game. Like -- Jump Height is 10 bits if it's set and 1 bit if it's not, 
//       right? So consider what happens if you make a gametype that's just 5 bits shy 
//       of the filesize limit, with a set of player traits that has no Jump Height: if 
//       the user changes that trait in-game, then they break the file.
//
//       So, what we need to do is: make ReachPlayerTraits::bitcount a static member 
//       again, and always add the bitcount for Jump Height plus one (for the presence 
//       bit). The space that Jump Height *can* use should always *display* as in use.
//
//     = THE METER NEEDS TO UPDATE WHENEVER INDEXED LISTS OR THEIR CONTAINED OBJECTS ARE 
//       ALTERED IN ANY WAY.
//
//     - Continue improving the code for the meter: add bitcount getters to more objects 
//       so we aren't just reaching into them from outside and counting stuff ourselves. 
//       Consider adding multiple bitcount getters to the MP object e.g. header_bitcount, 
//       standard_options_bitcount, etc., or perhaps a single getter that switch-cases on 
//       an enum indicating what we want counted.
//
//  - Modify the Vector3 type so that it's just treated as three script arguments. That odd 
//    "multi-part argument" syntax I came up with the parens should probably only be used 
//    for flags, and maybe not even for that.
//
//     = HOLD OFF ON THIS UNTIL THE PARSER'S IN GOOD SHAPE.
//
//  - Format string arguments: use QStrings so we have UTF-8 support, and modify the 
//    escaping code in the decompiler to do UTF-8 printable checks.
//

// OLD PLANS BELOW

//  - DECOMPILER
//
//        = TOP-PRIORITY TASKS FOR AFTER WE HAVE A COMPILER: RETAIN A BUILD CAPABLE OF 
//          COMPILING SO THAT WE CAN TEST STUFF IN-GAME ALONGSIDE WORKING ON THESE TASKS.
//
//           - Redesign the GameVariant class so that it no longer lines up exactly with 
//             the actual structure of the file. I want to group all script content under 
//             a blanket MegaloScript class and otherwise just make things more orderly.
//
//              - We shouldn't do this until we've documented, somewhere, the full struct-
//                ure of all elements in a script file, including offsets. Right now, the 
//                only explicit documentation of the file structure that we have on hand 
//                IS our in-memory struct definitions.
//
//

//
//  - PARSER
//
//     - Port it from JavaScript. The validate/analyze step basically IS compiling and 
//       should be named as such, though we'll get to the "actually generating triggers 
//       and opcodes in-memory" step later.
//
//     - Test round-trip operation.
//
//     - Finish the code for compiling function calls.
//
//     - Generate triggers and opcodes.
//
//  - IN-GAME TESTS
//
//     - Some game-namespace numbers that refer to social options are unknown; identify 
//       them.
//
//  - POTENTIAL COMPILER IMPROVEMENTS
//
//     - We could potentially build a compiler that compiles as it parses, holding only 
//       the current blocks and the current statement in memory, and generating an opcode 
//       as appropriate when it finishes parsing a statement. (Trigger objects, on the 
//       other hand, would be created as non-if-blocks open; we'd also create block 
//       objects that hold pointers to their owned triggers; and we'd insert the opcodes 
//       into the triggers as we parse.)
//
//       If we hit an invalid opcode that isn't a fatal parse error, then we just stuff 
//       a "none" opcode into the trigger-under-construction, remember the error, and 
//       print it at the end before discarding all constructed trigger data.
//
//       This would significantly reduce the amount of memory we end up using, since we 
//       wouldn't need to have two different representations of the script ("parsed" and 
//       "compiled") existing together in memory; we also wouldn't need to remember pos-
//       itional data (line numbers, column numbers, and such) for absolutely everything 
//       from the moment we see it to the moment we're done compiling. It may also be 
//       simpler, conceptually.
//
//  - POTENTIAL EDITOR IMPROVEMENTS:
//
//     - String table: warn when loaded count exceeds max count
//
//     - String table: offer option to recover work if string buffer is too large to save
//
// ======================================================================================
//
//  - Consider adding an in-app help manual explaining the various settings and 
//    traits.
//
//  - In Alpha Zombies, where are the "Humans" and "Zombies" strings near the top of the 
//    variant used? *Are* they used?
//
//  - When editing single-string tables, can we use the table's max length to enforce a 
//    max length on the UI form fields?
//
//  - STRING TABLE EDITING
//
//     - If we start editing a string that is in use by a Forge label, we should 
//       be blocked from changing its localizations to different values. This 
//       requires the ability to check *what* is using a string which in turn 
//       requires that all cobb::reference_tracked_object subclasses support 
//       dynamic casts -- we need to add a dummy virtual method to that superclass.
//
//     = NOTE: Some strings don't properly show up as in-use, but this is because 
//       not all data that can use a string uses cobb::reference_tracked_object 
//       yet. While all opcode argument types now subclass it, they don't all use 
//       its functionality yet.
//
//        = URGENT; MUST BE COMPLETED BEFORE RELEASE OF ANY SCRIPT EDITOR BETA
//
//     - Consider having a button to prune unreferenced strings. Alternatively, 
//       consider having a button to list them and let the user select which ones 
//       to delete; a mod author may wish to embed an unused string into the table 
//       to sign their work, state the script version, etc.. We may even wish to 
//       automate that process (a string entry could technically be used for 
//       binary data as long as we avoid null bytes).
//
//        - It won't be safe to implement this until we're properly tracking all 
//          string references.
//
//    - If a changed string is in use by player traits, the main window needs to 
//      be updated (i.e. if the user renames player traits through the string 
//      editor rather than the script traits editor). Ditto for anything else 
//      that can show up in the main window (script options, etc.).
//
//  - Work on script editor
//
//     - String table editing - MAIN FUNCTIONALITY DONE
//
//        - We need to make sure that the total length of all strings falls below 
//          the string table buffer size.
//
//           - When saving, we enforce this with an assert; we should come up with 
//             an error-handling system for saving files, and possibly be willing 
//             to devise a file format for projects-in-progress.
//
//        - We also need to make sure that the compressed size of the string table 
//          leaves enough room for script content, and that's more challenging. 
//          Essentially we either need to be able to tolerate faults when saving 
//          (without losing user data or forcing the user to fix things before 
//          saving again -- what if they're short on time?), OR we need to maintain 
//          bit-aligned copies of all script-related data in memory (as if saving a 
//          file) and show a warning in the UI when that exceeds some threshold.
//
//           - The size of non-scripted data can vary but not enough to matter; 
//             some player traits and odds and ends use a presence bit, and of 
//             course the variant metadata can be up to 127 widechars. We should 
//             just assume the max possible length for all non-scripted data and 
//             then measure the scripted data against the space that remains. (In 
//             any case, we have to allow for the maximum length of the title, 
//             description, author, and editor, or the gametype might be corrupted 
//             when resaving it in-game with longer metadata text. Similarly we 
//             should also assume that there will always be TU1 data, both because 
//             we always add that when saving and because the game may possibly 
//             add it when resaving.)
//
//        - It would be helpful to have something that can alert the user to any 
//          unused strings.
//
//     - Script content
//
//        - Need to modify how we handle saving script content: we need to serialize 
//          from the triggers. Currently we just serialize from the raw data.
//
//        - It would also be a good idea to give each opcode argument a pointer 
//          to its owning opcode. The ideas above -- Use Info for Forge labels and 
//          such -- require being able to locate a given opcode arg within the 
//          broader script, after having collected a list of opcode args.
//
//           - Sadly, we cannot give nested triggers a reference to their parents, 
//             because technically, one nested trigger can be called from multiple 
//             containing blocks (i.e. a subroutine instead of a nested block). I 
//             haven't paid close enough attention to know if any of Bungie's 
//             content does this, but the format should allow it.
//
//             What we could do instead is have triggers maintain a list of callers. 
//             Separately we could try and differentiate between a nested block and 
//             a subroutine while things are in-memory.
//
//  - When we rebuild navigation in the main window, all tree items are expanded. 
//    This is an ugly hack to make the fact that we don't remember and restore 
//    their states less noticeable. Can we improve anything in this area?
//
//  - Can we improve our error-reporting system to (at least optionally) store 
//    the stream bit/byte offset at the time of failure? Every failure point 
//    has access to the stream object, I believe.
//
//  - We'd benefit from being able to warn on unsaved changes. Easiest way 
//    would be to keep two copies of a variant in memory, and compare them.
//
//     - To do this, we need operator== for absolutely everything that can 
//       be in a game variant. Only C++20 lets you quickly declare those.
//
//        - First, we need to decide how we're going to distinguish between 
//          identity checks (are these the same object?) and equality checks 
//          (are these objects identical?). JavaScript has === but C++ does 
//          not.
//
//           - OPERATOR== SHOULD BE AN EQUALITY COMPARISON. IF YOU WANT AN 
//             IDENTITY COMPARISON, THEN COMPARE ADDRESSES I.E. &A == &B.
//
//  - UI
//
//     - Investigate the possibility of linking option-editing fields to 
//       their toggles, i.e. displaying an indicator if they've been toggled 
//       to disabled/hidden, and letting the user right-click them and jump 
//       to the toggle (irrespective of its state).
//
//        - We could also use a different text color for the field labels; 
//          we've set up "buddy" relationships between them (the Qt counter-
//          part to HTML's <label for="..." />).
//
//           - Yeah, but buddy relationships are one-way; given a field, we 
//             have no way to find the label.
//
//  - Investigate Firefight, Campaign, and Forge variants.
//
//     - Firefight hasn't been decoded (see below).
//
//     - Campaign has been decoded, but I'd be surprised if it were used for 
//       anything other than Matchmaking. Campaign variants are very minimal; 
//       there isn't much to load at all. I don't have any on hand to test 
//       with, and I think they were only ever used for Xbox 360 Matchmaking 
//       anyway.
//
// ==========================================================================
//
//  - Begin testing to identify further unknown information in Reach.
//
//     - Unknown values for Health Regen Rate
//
//        - Presumed to be "decay."
//
//           - It doesn't take effect when your shields are up.
//
//           - Does it prevent shield regen?
//
//           - Can it kill?
//
//     - Shield Vampirism
//
//        - Requires someone to test with.
//
//     - Sensors: Directional Damage Indicator
//
//        - None of the values affect whether I get a DDI on splash damage 
//          from my own grenades; they probably only affect damage dealt by 
//          other players.
//
//  = KNOWN INFO:
//
//     - Custom blocks are stripped out of the file when re-saved in-game, 
//       no matter whether they're before or after the _eof block. If we 
//       want to store extra data alongside a gametype (e.g. for script 
//       editing purposes), we need to use a co-save.
//
//     - The Grenade Regeneration trait is a trait-bool. If enabled for a 
//       player, that player will receive one frag and one plasma on every 
//       fifth second of the round.
//
//     - Active Camo trait:
//
//        - 2: Fades very, very slightly on movement; fades on attacks. Note 
//             that we tested movement with a machine gun turret so we can't 
//             judge the fade for sure.
//        - 5: Doesn't fade on movement; fades on attacks
//
// ==========================================================================
//
//  - Notes:
//
//     - In order to make custom widgets and make Qt Designer support them, 
//       you have to use QDESIGNER_WIDGET_EXPORT like all the online tutorials 
//       say... but something that the tutorials don't make obvious is, you 
//       need to be able to compile the widget definition and its code into a 
//       DLL to provide Qt Designer with. Like, you're not supposed to link 
//       with a DLL; you're supposed to make a DLL.
//