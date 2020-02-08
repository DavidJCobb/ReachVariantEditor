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
// TODO:
//
//  - DECOMPILER
//
//     - Finish implementing OpcodeArgTypeinfo, and have all opcodes refer to instances 
//       of that instead of factory functions.
//
//        - OpcodeArgTypeinfo instances need to be able to hold a factory function; they 
//          should accept a lambda as a constructor argument and store it locally. The 
//          factory function should NOT be a method; it should be a function pointer 
//          member, because we want to create OpcodeArgTypeinfo instances, not subclasses.
//
//           = Should they also overlap with what the JavaScript parser implementation 
//             defines as the "MScriptTypename" class?
//
//        - Every OpcodeArgValue should have a static OpcodeArgTypeinfo member; THAT is 
//          what we'll pass to the opcode functions' argument lists.
//
//           - Enums, flags-masks, and similar are an exception; see next bullet point.
//
//     - Merge all enums into a single OpcodeArgValue subclass that needs to be construc-
//       ted with an OpcodeArgTypeinfo& provided. Have the enum pull its values (and by 
//       implication its bitfield size) from the typeinfo object. We'll have one typeinfo 
//       instance per enum -- so basically, the OpcodeArgValue just has all of the common 
//       code/data for all enums, with the typeinfos actually defining different enums.
//
//        - And do the same for flags-masks.
//
//        - Get all of the index lists -- incidents, MP object types, and so on -- to 
//          conform to a common pattern so we can do the same for them.
//
//     - Once the above is all done, we can begin working on the decompile functions for 
//       OpcodeArgValue classes. Enums would be a great place to start, especially since 
//       they include assignment and comparison operators.
//
//        - There's a single type that acts as an "object and player variable;" it's 
//          used by the very last action in the actions list, and appears to serve as 
//          both that opcode's context and its parameter. We should look at the class 
//          and see if we can't split this argument in two -- sure, the game engine may 
//          treat it as "one argument," but if we can "lie" and still have the same 
//          binary output, then that makes things easier for us.
//
//          If that's not possible, we have a fallback: OpcodeArgValue::decompile takes 
//          a flags parameter that is currently unused. We can define a flag to indicate 
//          whether the argument is being decompiled as a context (i.e. "this") or as an 
//          argument; then, we can make the "opcode and player variable" argument both 
//          the context AND the argument (i.e. same_arg.function(same_arg)).
//
//     - Once it's done, test all official gametypes (including Freeze Tag!) against 
//       the current parser work in JS.
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
//        - We'll need to update the saving code; see multiplayer.cpp line 255.
//
//  - IN-GAME TESTS
//
//     - Some game-namespace numbers that refer to social options are unknown; identify 
//       them.
//
//     - The opcodes to select a new Hill are listed as unknown; I think one selects by 
//       Spawn Sequence and the other selects at random. KOTH would be a good way to 
//       check that.
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
// ======================================================================================
//
//  - Consider adding an in-app help manual explaining the various settings and 
//    traits.
//
//  - GameVariantDataMultiplayer::stringTableIndexPointer is going to break if strings 
//    are reordered, or if enough strings are removed for the index to be invalid. We 
//    need to wrap that in a cobb::reference_tracked_object as we have string references 
//    elsewhere.
//
//     = URGENT; NEEDS FIXING
//
//  - In Alpha Zombies, where are the following strings used? None of our current stuff 
//    for reference_tracked_object is catching them (the user still has the option to 
//    delete them). "Humans" "Zombies" "Infection" They're the first three strings in 
//    the variant.
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