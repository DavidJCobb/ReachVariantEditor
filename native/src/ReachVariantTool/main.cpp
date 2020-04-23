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
//     = CAN WE FIND A BETTER NAME THAN "STATIC" FOR THINGS LIKE player[0]?
//
//     = REAL QUICK, WE SHOULD ADDRESS THE PROBLEM OF THE DECOMPILER NOT BEING ABLE TO 
//       HANDLE USER-DEFINED FUNCTIONS (I.E. "SUBROUTINE" TRIGGERS CALLED FROM MULTIPLE 
//       PLACES), ESPECIALLY RECURSION. THIS WILL BE SIGNIFICANT ONCE WE HAVE THE COMPILER 
//       WORKING; WE DON'T WANT TO BE UNABLE TO DECOMPILE OUR OWN COMPILER OUTPUT.
//
//       Specifically, we need to be able to handle this:
//
//          function foo()
//             action
//             if condition then
//                foo()
//             end
//             action -- this being here makes the "if" a nested trigger calling an ancestor
//          end
//          function bar()
//             action
//             if condition then -- no actions after this if, so it's the same trigger
//                bar()
//             end
//          end
//
//       So how do we make that possible? Well,...
//
//        - In decompiler.cpp, we should define a class called _TriggerTreeNode:
//
//             struct _TriggerTreeNode {
//                using list_t = std::vector<_TriggerTreeNode*>;
//                
//                int32_t index; // trigger index; -1 == root
//                list_t  callers;
//                list_t  callees;
//             }
//
//          Before decompiling triggers, we would scan all triggers in the list to construct 
//          a tree of _TriggerTreeNodes. Then, after the tree is fully constructed, we would 
//          construct a std::vector<uint32_t> of trigger indices that need to be decompiled 
//          as user-defined functions. A trigger needs to be decompiled as a user-defined 
//          function if any of the conditions are met:
//
//           - The trigger has more than one caller.
//
//           - The trigger's callees include itself.
//
//           - The trigger's callees include any of its callers.
//
//           - The trigger's callees include any of its callers' callers, or its callers' 
//             callers' callers, or so on.
//
//          (Kind of wondering whether traversing the tree to do that is the best idea; it 
//          may be better to have a flat list of _TriggerTreeNode*s that point to each 
//          other as appropriate, and then loop over them. Mainly, this would make it easier 
//          to ensure that we only run the above checks once per trigger even when a trigger 
//          is called from multiple places.)
//
//          User-defined functions would be decompiled in advance of all triggers, with 
//          auto-generated names (e.g. "trigger_1"). The function to decompile an individual 
//          Trigger object would need to accept a boolean indicating whether it should wrap 
//          its block header in a "function" declaration (or replace the block header 
//          entirely if it's a generic "do" block).
//
//           - If an event Trigger is told to decompile as a function, it should place the 
//             event name before the "function" keyword.
//
//           - Consider adding a decompiler option to suffix every block-opening line with 
//             a comment indicating the trigger's index; this would be useful for debugging 
//             the (de)compiler and the loader.
//
//     - SHORT-TERM PLANS
//
//        - The values for "game.death_event_damage_type" are entries in what KSoft calls the 
//          DamageReportingTypeHaloReach enum. What can we do with this knowledge?
//
//           - If we make it possible to define a NamespaceMember that refers to a constant 
//             integer, then we can create a namespace of named values for this enum.
//
//        - Now that GameEngineVariantDataMultiplayer::isBuiltIn has been identified, add it 
//          to the UI.
//
//        - Per Kornman00, IconIndex6 is a HUD Widget Icon; the list is here: https://github.com/KornnerStudios/KSoft.Blam/blob/master/KSoft.Blam/Games/HaloReach/Megalo/Proto/HaloReach_MegaloStaticDb_Xbox.xml#L1322
//
//        - Per Kornman00, IconIndex7 is an Engine Icon; the list is here: https://github.com/KornnerStudios/KSoft.Blam/blob/master/KSoft.Blam/Games/HaloReach/Megalo/Proto/HaloReach_MegaloStaticDb_Xbox.xml#L1361
//
//        - Kornman00 identified some of the Forge settings, but I'm not 100% clear on what 
//          the new names mean: https://github.com/KornnerStudios/KSoft.Blam/blob/5a81ac947990f7e817496fe32d1a1f0f16f09112/KSoft.Blam/RuntimeData/Variants/GameEngineSandboxVariant.cs
//
//        - The end of the compile process should run one final check: scan all Opcodes 
//          in all compiled triggers, and throw a compiler error if any have any nullptr 
//          arguments.
//
//     - COMPILER OWNERSHIP OF COMPILED CONTENT: Compiler SHOULD NOT RELINQUISH OWNERSHIP 
//       OF COMPILED TriggerS, ETC., WHEN COMPILING SUCCEEDS. RATHER, THE Compiler SHOULD 
//       HAVE A MEMBER FUNCTION WHICH "APPLIES" THE COMPILED CONTENT TO THE TARGET GAME 
//       VARIANT AND THEN RELEASES OWNERSHIP AND DISCARDS POINTERS.
//
//        - The "apply" function should assert if there are any unresolved string references 
//          (see below) or if there are any compiler errors.
//
//     - OpcodeArgValue::compile overrides on subclasses
//
//        - COMPILER NEEDS TO PROVIDE HELPERS. Specifically, while we have a function on 
//          string_scanner that can extract a string literal, we don't have a function to 
//          deal with escape codes e.g. "\n". (The exception is escape codes for the string's 
//          own delimiter, e.g. "That's a \"good\" idea..." and 'Hey, what\'s up?' Those are 
//          handled because they need to be handled in order to extract the string properly.) 
//          Similarly, an OpcodeArgValue will want to look up the index of a string literal 
//          in the string table, and validate any string indices passed by way of integer 
//          literals and alias names.
//
//           - Compiler::arg_to_variable has been provided as a means of converting an 
//             argument received as text to a VariableReference.
//
//        - TIMER RATE IS IMPLEMENTED AS A BASIC ENUM, BUT THIS WON'T WORK. We want it to be 
//          able to accept any integer literal, e.g. -000100, and treat it as an integer. 
//          Currently, because it uses the same compile function as any basic enum, it just 
//          matches as a string.
//
//           - Currently, this yields a non-fatal error about there being unexpected content 
//             at the end of the argument. That's just due to how enum parsing works, I think.
//
//     - COMPILER TESTS: We can test the compiler itself right now, on the understanding 
//       that any argument types that haven't had their own compile code written yet will 
//       yield non-fatal errors.
//
//        - References to "script_option[n]" aren't recognized. We probably need to define 
//          a scope NamespaceMember for this -- and make sure that VariableReference honors 
//          whether a scope can have an index when resolving a NamespaceMember.
//
//        - Format string arguments currently yield "too many arguments" as an error, and will 
//          until we write the code to resolve them. It's not a compiler problem.
//
//        - It seems like accessing any member of a NamespaceMember, e.g. (current_player.team), 
//          yields compiler errors regarding the first part (e.g. current_player) not having an 
//          index.
//
//        - Line numbers seem to be subtly off on several error messages and I'm not sure why.
//
//        - For some reason, write access to "score" still resolves to the read-only property 
//          despite the property being properly flagged, write-access being specified when 
//          resolving the VariableReference, and VariableReference::resolve having code to 
//          react to write access being specified. That code must be wrong somehow.
//
//     - Compiler: Unresolved string references: each list entry needs an "action" field 
//       which lists the action the script author decided to take. Available actions are: 
//       create a new string in the table; or use an existing string with a given index. 
//       Intended UI flow for when the compiler finds unresolved string references is:
//
//        - Alert the user to the unresolved string references.
//
//        - Allow the user to: cancel compiling; create all referenced strings en masse; 
//          or choose how to handle each string individually.
//
//       The Compiler should have a function which will resolve all unresolved string 
//       references that have an action set. The compiler's code to apply compiled content 
//       to the game variant should assert if there are any unresolved string references.
//
//       There are a few considerations we'll need to make:
//
//        - The same string content may be referred to in multiple places, i.e. multiple 
//          unresolved string references may target the same unresolved string. We need to 
//          ensure that we don't end up creating duplicates of these strings in the string 
//          table.
//
//        - The string table may not have room for the number of new strings needed.
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
//        - Added a "TODO" comment to Block::compile where we'd need to do this.
//
//        - HMM... I THINK IT WOULD ACTUALLY BE BEST TO HANDLE THIS BEFORE COMPILING BLOCKS. 
//          SPECIFICALLY, WHEN WE OPEN AN ELSE(IF) BLOCK, WE SHOULD CLONE THE ALREADY-COMPILED 
//          CONDITIONS OF THE PRECEDING (ELSE)IF BLOCK INTO THE NEW ELSE(IF) BLOCK. As for how 
//          to invert them? I think it would be enough to just set the "inverted" flag on all 
//          of the cloned conditions and swap all "and"/"or" relationships, but I'd need to 
//          test that with pen and paper (or rather, Notepad) to see if it holds up. Or Google 
//          it.
//
//     = DEFERRED TASKS
//
//        - The writeable "symmetry" property is only writeable in a "pregame" trigger. Can we 
//          enforce this, or at least generate a compiler warning for inappropriate access?
//
//           = DEFERRED UNTIL WE CAN TEST THE PROPERTY'S ACCESSIBILITY AS DESCRIBED A FEW 
//             BULLET POINTS BELOW.
//
//           - We'd want to start by giving Block a function which checks its own event type 
//             and the event types of its ancestor Blocks, looking for a "pregame" block. If 
//             none is found, we'd want to signal the need for a warning. The warning text 
//             should differ depending on whether we're inside of a user-defined function or 
//             not.
//
//           - After that, we'd need to give VariableScopeIndicatorValue the ability to 
//             specify that access is only recommended from a given Megalo::entry_type. (This 
//             would also be good for the "death event damage type" number value.) We'd also 
//             need to specify whether access limits apply to just writing or to any access.
//
//           - Finally, we'll need to check the event type for any VariableReference's 
//             containing Block. Probably easiest to do this either in VariableReference 
//             itself (at the end of the resolve process) or in Variable::compile. Wanna go 
//             with the former.
//
//           = NOTE: We need to test whether the writeable "symmetry" value can be read from 
//             outside of a pregame trigger. It's possible that the writeable "symmetry" value 
//             is ONLY meant to be accessed (not just modified) from a pregame trigger, with 
//             the other "symmetry_get" value used everywhere else.
//
//             If that's the case, then the "correct" approach would probably be to name both 
//             values "symmetry" and always prefer the read-only one for resolution if we are 
//             resolving anything outside of a pregame trigger. The problem with this is that 
//             it would create complications for user-defined functions -- specifically, user-
//             defined functions that use the "symmetry" value and that are invoked from both 
//             the pregame trigger and a non-pregame trigger. It would be impossible for such 
//             functions to actually indicate which "symmetry" value they should access, if 
//             we gave both values the same name.
//
//             If testing indicates that the writeable "symmetry" value is only readable from 
//             a pre-game trigger, then "symmetry" and "symmetry_get" should be renamed to 
//             "symmetry_pregame" and "symmetry", respectively.
//
//             In any case, however it plays out, we should document it in comments near the 
//             "symmetry" VariableScopeIndicatorValue definition and/or declaration.
//
//     = (DE)COMPILER UI
//
//        - Code editor with syntax highlighting
//
//        - UI for showing compiler warnings and errors; should include the ability to jump 
//          to the error location in the code
//
//        - UI needed for letting the script author deal with unresolved string references. 
//          We can start with something basic ("create all strings" or "cancel") and add 
//          full functionality to it after we're sure the compiler itself works.
//
//        - The string table needs a button to copy the content of a selected string as a 
//          string literal (i.e. with escape codes), and a button to copy the index of a 
//          selected string.
//
//     = AUDITING
//
//        - Exception safety for anything that gets heap-allocated.
//
//        - DO A PROJECT-WIDE SEARCH FOR THE WORD "TODO".
//
//        - Ensure that the error handling for comparing string literals to variables 
//          (or to each other) works properly.
//
//             Wondering whether using Compiler::skip_to in that situation will cause 
//             us to move one char too far, since we're using it from inside of a scan 
//             functor. Also wondering whether it'll just see the opening quotation mark 
//             for the string literal again, i.e. whether we need to advance by a single 
//             character before calling it.
//
//        - Ensure that the error handling for assigning a string literal to a variable 
//          works properly.
//
//             Wondering whether using Compiler::skip_to in that situation will cause 
//             us to move one char too far, since we're using it from inside of a scan 
//             functor. Also wondering whether it'll just see the opening quotation mark 
//             for the string literal again, i.e. whether we need to advance by a single 
//             character before calling it.
//
//        - More generally, test every non-fatal error and every fatal error. Once the 
//          compiler's fully written, write a single test script containing every possible 
//          non-fatal error.
//
//     = RANDOM NOTES
//
//        - It'd be cool if the "object player variable" type accepted either the index 
//          of an object.player variable, or a relative alias of an object.player var.
//
//        - Kornman00 and Assembly both identify the unknown movement option as "double jump," 
//          but in my tests, it didn't seem to work. I even tested with Jumper Jumper disabled 
//          in case that was interfering, and nope. Nothin'.
//
//     = TESTS FOR ONCE WE HAVE A WORKING COMPILER
//
//        - Round-trip decompiling/recompiling for all vanilla gametype scripts and for 
//          SvE Mythic Slayer. Tests should include modified versions of the decompiled 
//          scripts that use aliases where appropriate (both because I want to provide 
//          such "source scripts" to script authors to learn from, and so we can test to 
//          ensure that aliases work properly).
//
//        - KSoft.Tool seems to have switched around the "Teams Enabled" flag (misc 
//          options bit 0) and the "Perfection Medal Enabled" flag (misc options bit 3). 
//          We've got these set up properly for normal settings, but for scripts/Megalo, 
//          we will need to test BOTH FLAGS (game.teams_enabled and game.misc_unk0_bit3) 
//          to make sure we have them right (or fix them) there.
//
//        - Do user-defined functions actually work? Don't just test whether the game 
//          can load a script that contains triggers called from multiple places; test 
//          to ensure that if a trigger is called multiple times from multiple places in 
//          the same tick, it will actually run each time. The file format as designed 
//          allows for user-defined functions to exist, but it's unknown how the runtime 
//          will actually handle them.
//
//        - What happens when we perform an invalid assignment, such as the assignment 
//          of a number to an object? Be sure to check the resulting value of the target 
//          object and to try to access a member on it. I'm wondering whether the game 
//          does nothing, clears the target variable, crashes immediately, or breaks the 
//          variable such that meaningful use will crash. This will determine whether we 
//          add a compiler warning or compiler error for bad assignments.
//
//           - While we're at it, verify the exact result of assigning a number to a 
//             timer (which we know from vanilla scripts is valid) and of assigning a 
//             timer to a number (which I don't remember seeing in vanilla content).
//
//        - We need to test whether our subroutine (user-defined function) approach is 
//          actually supported by the game. It entails defining a trigger that is 
//          flagged as nested but actually called from multiple places, rather than 
//          being a block "inside" of another block. It would also be worthwhile to see 
//          if this is compatible with the subroutine being an event handler.
//
//        - Does the "Create Object" opcode use an absolute position offset or a relative 
//          one (i.e. using the "basis" object's rotation axes)? Are the units the same 
//          as in Forge, or are they scaled (i.e. script 1 = Forge 0.1)?
//
//        - Why did Bungie use Condition::or_group instead of a simple bool to indicate 
//          an OR-relationship with the next condition? The only reason I can think of 
//          is if they supported parenthetical conditions, but I haven't seen any 
//          obvious uses of this in gametype scripts. We should devise a test such as 
//          
//             (A and B) or (C and D)
//
//          Then, we should write the code without parentheses:
//
//             A and B or C and D
//
//          And then we should manually tamper with the Condition::or_group values in 
//          the compiled script, and test different setups in-game to see if any give 
//          us results comparable to what we'd see with parenthetical expressions. For 
//          our actual tests, each individual condition can just be a test to see if 
//          the player is inside of a given shape, and we can place overlapping shapes 
//          on the map.
//
//        - What happens if the script tries to use a variable that is not declared? We 
//          would need to tamper with the variant data manually to check this.
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
//     = THE METER NEEDS TO UPDATE WHENEVER INDEXED LISTS OR THEIR CONTAINED OBJECTS ARE 
//       ALTERED IN ANY WAY. BONUS POINTS IF WE ONLY UPDATE THE PART OF THE METER THAT 
//       ACTUALLY REPRESENTS THE CHANGED DATA.
//
//     - Continue improving the code for the meter: add bitcount getters to more objects 
//       so we aren't just reaching into them from outside and counting stuff ourselves. 
//       Consider adding multiple bitcount getters to the MP object e.g. header_bitcount, 
//       standard_options_bitcount, etc., or perhaps a single getter that switch-cases on 
//       an enum indicating what we want counted.
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