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
//     - Port namespace, etc., definitions from JavaScript.
//
//     - Begin creating a Compiler singleton with parser logic.
//
//        - Code to parse blocks, assignments, and comparisons
//
//           = Mostly done, though some things are obviously missing since we can't handle 
//             variable references, etc..
//
//           = Assignments should allow (var = func()) but NOT (var += func()).
//
//           = Assignments should fail when assigning to a constant integer, or to an alias 
//             of a constant integer. (We'll probably have to do this when compiling, not 
//             sooner.)
//
//        - Code to parse function calls (for now, don't bother with args)
//
//        - Code to interpret variable references
//
//           - Basic references
//
//           - Aliases
//
//        - Code to parse variable declarations. Remember that you can declare variables 
//          in any scope (i.e. "player.number[0]" without a specific player specified), 
//          that specifying an initial value is optional, and that only some types can 
//          even have initial values in the first place.
//
//        - Code to generate Opcode*s from assignments, comparisons, and function calls
//
//           - By the time we hit this point, we'll need the Compiler to have a reference 
//             to the game variant MP data just like Decompiler does, since we'll need to 
//             set up references to indexed list items and whatnot.
//
//              - We're also gonna wanna write teardown code and make sure that anything 
//                we're doing can survive an exception being thrown, i.e. make sure we 
//                don't leak things allocated with (new) when we throw exceptions. That 
//                will require careful coding at every site that can raise an exception.
//
//           - Once we've parsed a function's name (and context, for thiscalls), we can 
//             identify the opcode function being invoked. We can then create an Opcode 
//             instance and its args, and ask the args to handle their own parsing: have 
//             a function like OpcodeArgValue::compile(Compiler&, uint8_t part), with it 
//             having access to the "extract" functions for argument parsing. That function 
//             should be able to return one of four status codes: success; failure; need 
//             another argument; can receive another argument. The latter two are for 
//             varargs types e.g. Shape (which always knows how many more arguments it 
//             needs) and Formatted String (which receives a format string and between 0 
//             and 2 additional tokens, inclusive).
//
//              = NO. THIS WON'T WORK FOR THINGS THAT USE AN ALTERNATE SYNTAX, E.G. 
//                COMPARISONS, ASSIGNMENTS, AND THE "Modify Grenades" OPCODE WHEREIN THE 
//                PROPERTY NAME IS ITSELF AN OPCODE ARGUMENT.
//
//                How about this instead:
//
//                 - Move the string stuff -- scan, extract, etc. -- from Compiler to a 
//                   new base class. Perhaps "StringScanner."
//
//                 - Define Script::ParserPosition::operator+ and such.
//
//                 - To compile a function call argument, extract everything up to the 
//                   next ',' or ')' and stuff that content into a new StringScanner. 
//                   Then, have OpcodeArgValue::compile take both a reference to the 
//                   Compiler, a part index, and the StringScanner, and have it use the 
//                   StringScanner to extract the argument. (Compiler should also provide 
//                   a function that takes a QString and produces a variable reference.) 
//                   If we return a failure code, then the compiler can add the string-
//                   scanner's position to its own position and then raise the error.
//
//                    - If a non-failure code is returned, add the StringScanner's state 
//                      to the Compiler's state in order to advance to the end of the 
//                      parsed argument. Then, look for a ',' or ')'; if we find any-
//                      thing else instead (or hit EOF), raise an error; otherwise, 
//                      repeat for the next argument.
//
//                 - Once that's done? We can handle "Modify Grenades" via special-case 
//                   behavior for when the OpcodeFuncToScriptMapping has a blank primary 
//                   name and a defined (arg_name): we just pass the property name to 
//                   OpcodeArgValue::compile.
//
//           - Once we've parsed a function's name (and context, for thiscalls), we can 
//             identify the opcode function being invoked... but some Opcodes share the 
//             same name. Specifically, there are two (send_incident) opcodes, one of 
//             which accepts an additional integer argument. So how do we allow that? By 
//             simple trial-and-error. When we know the function's name and context, we 
//             grab a vector of ALL opcodes that have a matching name and context, and 
//             then we try each one until one of them returns a success code. If all of 
//             them return failure codes, then we raise an error. (If there was only one 
//             matching opcode, then we should raise whatever error text it gave us; 
//             otherwise: "The arguments did not match any of the signatures for the X 
//             function.")
//
//              - So: get a list of all matching functions; create an Opcode; back up the 
//                stream state. Then, for each function: clear the Opcode's arguments; 
//                create arguments based on the function signature; try parsing them; if 
//                we fail, restore stream state from the backup (i.e. rewind) and try the 
//                next function.
//
//        - Code to compile a Block and its contained Blocks.
//
//           - We can run this whenever we close a top-level Block. We can't run it the 
//             instant we close a nested Block because we want a consistent Trigger order 
//             with Bungie, and compiling nested Blocks on closure will result in inner 
//             Triggers being numbered before outer Triggers.
//
//        - Code to allow the aliasing of enum and flag values
//
//           - OpcodeArgTypeinfo has a system to declare names, but it requires a list of 
//             const char*s whereas basically everything we have now is based on DetailedEnum 
//             or DetailedFlags. We should upgrade it.
//
//        - We're gonna have to take some time to work out how to negate if-statements that 
//          mix OR and AND, in order to make elseif and else work. Fortunately, we only need 
//          to know that when it comes time to actually compile the blocks... assuming we 
//          don't decide to just drop support for else(if)s at that point. (Think about it: 
//          first of all, they abstract away conditions, so you can't see the full "cost" 
//          of your code; and second, there's no code to detect equivalent ifs and decompile 
//          them to else(if)s yet. We should support else(if)s if we can but they aren't by 
//          any means essential; we've already decided that conciseness is not required.)
//
//  - Decompiler: work on a better text editor in-app, with horizontal scrolling, line 
//    numbers, syntax highlighting, code folding, etc..
//
//  - Script editor window: work on a display on the bottom: a bar that should show how 
//    much space in the file has been used, with usage divided into colored regions based 
//    on the different parts of the file (showing details onmouseover). Think of it like 
//    the disk usage bar from the old pre-Vista Windows Disk Defragmenter.
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