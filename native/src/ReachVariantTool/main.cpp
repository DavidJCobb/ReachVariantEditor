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
//  - Begin bringing decompile code from the new system back to the old system. Redo index 
//    types as needed.
//
//     - formatted strings and their tokens
//     - specific-variable
//        - OpcodeArgValueObjectTimerVariable is only used in one place: to set which of an 
//          object's local timer vars is used as its progress bar, and who can see that 
//          progress bar. I think we should set that up as a new intrinsic akin to assignment 
//          and comparison, with the syntax: object_var.timer_var.set_visibility(who)
//
//        - OpcodeArgValueObjectPlayerVariable is only used in one place: an opcode that is 
//          incorrectly named: it does not act as a "boundary visibility filter," but instead 
//          is responsible for showing hill ownership status in KOTH FFA. I've asked friends 
//          if they can get me details; I also have KOTH decompiled (as much as is possible 
//          at present) and can find the exact place this appears.
//
//  - Remove postprocessing.
//
//     - The game-variant-data constructor should fill all indexed lists with dummy elements. 
//       Dummy elements need to have a flag; when we load the "real" elements, strip the flag 
//       from existing elements instead of deleting and replacing them.
//
//        - After loading the contents of an indexed list, iterate backwards over it and 
//          delete any dummy elements whose refcounts are zero. Stop deleting at the first 
//          dummy element with a non-zero refcount, or at the first non-dummy element; if 
//          any dummy elements have non-zero refcounts, show warnings for all such dummies.
//
//     - Pass the game-variant-data down through the load process. Set up indexed-object 
//       pointers during load.
//
//     - And now we don't need postprocessing!
//
//  = Ditch the new opcode argument system entirely and revert fully back to the old one.
//
//     - Remove all factory functions from OpcodeArgValue and its subclasses, and have them 
//       all use typeinfos. Then, amend OpcodeArgBase to require typeinfos, and remove the 
//       "band-aid" code from the read/write methods for Condition and Action.
//
//     - If we modify OpcodeArgValue::write to append to a cobb::bitarray128, then we can 
//       use it both for saving to a file and for counting the total size of all loaded 
//       opcodes -- needed for my eventual plans to show the space usage for each part of 
//       the variant file, to help script authors better assess how much room they have to 
//       work with.
//
//        = Actually, we don't even need to do this. All of the script stuff saves via a 
//          bitwriter, right? Just make a throwaway bitwriter for the counting and save 
//          only the script fields to it; then, check its size.
//
//     - One thing that the new system did right was to nest the types under a dedicated 
//       namespace and give them shorter names, i.e. Megalo::types::number and such. We 
//       should reorganize the old system to use the same convention.
//
//  - If we can modify references to indexed data to optionally use aliases if the 
//    Decompiler& so wills, then so much the better; however, this would require extending 
//    the generic variable handling so that a scope-indicator definition can specify a 
//    custom decompile functor.
//
//  - When decompiling is done, begin porting the compiler work from the JavaScript 
//    prototype.
//

// OLD PLANS BELOW

//  - DECOMPILER
//
//     - New opcode argument system
//
//        - Icon indices
//
//        - Requisition palette indices
//
//        - Variable argument values
//
//           - The generic variable system does not properly report load errors for 
//             "biped" property access. It also doesn't have any provisions for "team" 
//             property access, since apparently we didn't actually have full error-
//             checking for that in the old system (we didn't validate the "which" in 
//             that specific case).
//
//        - Other argument value types
//
//           - Format strings (tokens.h in the old system)
//
//           - Player sets
//
//           - specific_variable.h
//
//           - Waypoint icons
//
//           - Widget stuff
//
//        - The opcodes themselves
//
//        = Ensure that error handling has parity with what's in the release build.
//
//           - VariableScopeIndicatorValueList::postprocess needs proper error reporting.
//
//        = NOTE: POSTPROCESSING: I want to set up a queue on the game variant data 
//          such that any objects that need postprocessing can just register themselves 
//          in the queue and then we process (and then empty) that queue after load. 
//          I've already created IGameVariantDataObjectNeedingPostprocess for this 
//          purpose. I need to create the queue system itself and the code for it.
//
//           = ALTERNATIVELY, IMPLEMENT THE "NO POSTPROCESSING" PLANS BELOW EARLIER THAN 
//             PLANNED, SO WE CAN SKIP THIS WORK ENTIRELY.
//
//           - An object can only add itself to the queue if it has access to the queue 
//             (i.e. through the containing game variant object) during load. Currently 
//             this access is not available to the new opcode argument system; I need 
//             to modify all load methods/functors to take a game variant pointer as an 
//             argument, and modify any typeinfos that can refer to indexed data to 
//             register themselves for postprocess whenever they DO refer to indexed 
//             data.
//
//           = Actually, coming back to all of this ages later, it looks like the new 
//             OpcodeArgTypeinfo has a flag indicating that a type "may need post-
//             processing." I had probably intended for opcodes to do the registration 
//             rather than arguments registering themselves, but that still means that 
//             the game variant needs to be passed at least as far down as the opcode.
//
//        - Swap everything in and see if it works.
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
//           - I can completely remove the need for a "postprocess" step if I just don't 
//             dynamically instantiate most of the content. For example, a game variant 
//             can have sixteen options, right? So what if I make it so that the common 
//             superclass we're using, indexed_refcountable, has an "is_defined" bool on 
//             it, and then, the moment the game variant is created in memory, create 
//             sixteen dummy options? Then, I can track their refcounts before they're 
//             "loaded:" when we find an opcode that refers to the objects, we set up a 
//             pointer from the opcode to the dummy, and when we finally get around to 
//             loading the options, we flag those that we load as no longer being dummies.
//
//              - As a bonus, this also allows us to display load-time warnings if a 
//                script refers to an out-of-bounds option. At the end of the load process, 
//                for each indexed list, we would iterate backwards over the list and 
//                delete any dummies with a zero refcount; we stop deleting at the first 
//                entry we find that either isn't a dummy or has a non-zero refcount; and 
//                if any dummy entries have a non-zero refcount we pop a warning.
//
//              - This idea isn't workable for strings in the string table, but we also 
//                don't need to change anything about how we handle those. Strings are 
//                loaded before most other script content and we can manually postprocess 
//                the few things that load earlier.
//
//           - I obviously misjudged the work involved in my current approach. With the 
//             previous system, you need one OpcodeArgValue subclass for every argument 
//             type, along with a boilerplate-filled "satellite" object: an instance of 
//             OpcodeArgTypeinfo. Somehow I thought it would be easier to just turn the 
//             whole thing inside-out -- have just the typeinfo, with non-member functors 
//             to manipulate the bits -- but all that means is that I have to constantly 
//             work with bits manually, rather than just on saving and exporting. We don't 
//             NEED to keep the data in a "friendly" intermediate format but it's so, so 
//             much easier to.
//
//     - Syntax
//
//         - We need to create a "declare" keyword for variable declarations. It should 
//           take a variable name and, optionally, a value (i.e. "declare foo" or 
//           "declare foo = bar"). The decompiler then needs to generate these declara-
//           tions.
//
//         - The decompiler should auto-generate aliases for all indexed data items i.e. 
//           stats, options, and traits, and decompile code for specific typeinfos should 
//           use these. Currently OpcodeArgTypeinfo::decode_functor_t doesn't take an 
//           argument that can hold decompile options so for now, let's define a constexpr 
//           bool somewhere which controls whether aliases are being auto-generated; that 
//           way, if we ever make this optional, it'll be easier to track down everything 
//           that needs to respond to the option.
//
//


//
//     - Finish implementing OpcodeArgTypeinfo, and have all opcodes refer to instances 
//       of that instead of factory functions.
//
//        - OpcodeArgTypeinfo instances need to be able to hold a factory function; they 
//          should accept a lambda as a constructor argument and store it locally. The 
//          factory function should NOT be a method; it should be a function pointer 
//          member, because we want to create OpcodeArgTypeinfo instances, not subclasses.
//
//           = They should also overlap with what the JavaScript parser implementation 
//             defines as the "MScriptTypename" class.
//
//        - Every OpcodeArgValue should have a static OpcodeArgTypeinfo member; THAT is 
//          what we'll pass to the opcode functions' argument lists.
//
//           = IN PROGRESS, BUT WE'VE HIT A TINY SNAG. There are several arguments that 
//             just wouldn't be practical to keep multi-part -- things like shapes, 
//             widget meter parameters, and waypoint icons. We may want to consider 
//             cheating a bit -- splitting these single arguments into multiple arguments. 
//             The binary representation *should* be equivalent.
//
//              - The problem is that if we split these apart, then some opcodes become 
//                varargs. Splitting them apart *internally* is going to be especially 
//                nasty, but what would be easier is to just split them apart in the 
//                script dialect -- so we'd still have a single OpcodeArgValueShape class 
//                internally, but in the script, you'd write that one argument out as 
//                multiple arguments.
//
//                In that case, opcodes are still varags within the script, so we'll have 
//                to handle that within the code for compiling function call arguments. 
//                That code will have to have access to the full list of arguments in the 
//                current call, it will have to be able to "consume" zero or more arg-
//                uments, and it will have to return the number consumed. That also means 
//                that we won't be able to just do a single count check on function call 
//                arguments; instead, we'll have to consume arguments until done and then 
//                check if there are extra arguments remaining (if there aren't enough 
//                arguments, then the consume function should return a negative result -- 
//                perhaps the number of arguments it did manage to consume, negated).
//
//                 = At the same time, we need to make sure that this doesn't cause 
//                   issues in cases where two opcodes have the same name but different 
//                   signatures: the two "send_incident" opcodes would be the sole case 
//                   of this. We should add to OpcodeArgTypeinfo an "is_varargs" bool 
//                   that would be set on any argument class that represents more than 
//                   one argument in script, like OpcodeArgValueShape:
//
//                    - When matching function names (from scripts) to opcodes, we prefer 
//                      the opcode with the same name, the same argument count, and no 
//                      is_varargs arguments. If no such opcode exists, we prefer the 
//                      opcode with the same name and any is_varargs arguments, and we 
//                      try to compile for that opcode. If no such opcode exists, then 
//                      there is an error: the user specified a non-existent opcode or 
//                      an argument count that matches no overload for the opcode name 
//                      they used.
//
//                    = ADD THE is_varargs FLAG TO OpcodeArgTypeinfo.
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
//     - We could greatly simplify our handling of opcode arguments by removing all of 
//       the OpcodeArgValue subclasses, and just having OpcodeArgValue store a uint64_t 
//       of binary data and a OpcodeArgTypeinfo reference; we would then make the 
//       OpcodeArgTypeinfo responsible for decoding that binary data.
//
//       Currently, we have OpcodeArgValue subclasses which are used to decode that 
//       data on load and retain it indefinitely, but we only really *need* to decode 
//       the data to accomplish the following:
//
//        - Correctness checks during load.
//
//        - Serializing to plain English.
//
//        - Serializing to script code.
//
//        - Inspecting and tampering with data during debugging.
//
//       None of those needs require us to actually retain the decoded data. None of 
//       those needs require us to ever decode the data into an orderly struct (though 
//       doing so makes debugging a lot easier). If we just load the raw bits and have 
//       the typeinfo decode them on demand for decompiling, then we remove the need to 
//       have both OpcodeArgValue subclasses and OpcodeArgTypeinfo instances for every 
//       argument type.
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