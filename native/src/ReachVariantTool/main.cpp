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
//        - Kornman00 identified some of the Forge settings, but I'm not 100% clear on what 
//          the new names mean: https://github.com/KornnerStudios/KSoft.Blam/blob/5a81ac947990f7e817496fe32d1a1f0f16f09112/KSoft.Blam/RuntimeData/Variants/GameEngineSandboxVariant.cs
//
//     - When we set the "minimum count" on the Forge labels in our MOTL brute-force gametype, 
//       MCC's menus don't enforce this. Investigate.
//
//     = REORDERING OPTIONS WILL CAUSE THE MEGALO OPTION TOGGLES TO DESYNCH; WE NEED TO SWAP 
//       BITS WITHIN THOSE TOGGLES AS APPROPRIATE WHEN REORDERING AN OPTION.
//
//        - ADDING/REMOVING MEGALO OPTIONS ALSO NEEDS TO CLEAR THE RESPECTIVE TOGGLE-FLAGS.
//
//     - MCC seems to hang when clicking through the title screen if any game variant invokes 
//       the (game.show_message_to) opcode with (none) as the sound. Verify that this is 
//       consistent, and if so, check whether any opcodes that accept sounds allow the use 
//       of (none).
//
//     - Can we give (string_scanner::extract_integer_literal) the ability to understand 
//       non-base-10 integers i.e. 0x... for hex and 0b... for binary? It'd help with using 
//       variables as flags-masks.
//
//     - Don't forget to rename OpcodeFuncToScriptMapping::secondary_property_zeroes_result 
//       to ...::secondary_name_zeroes_return_value.
//
//     - Consider adding a new OpcodeFuncToScriptMapping flag for conditions: "secondary name 
//       inverts condition." Then, we could give "is_not_respawning" the more intuitive 
//       secondary name "is_respawning" with the negate flag set.
//
//     - Decompiler: consider moving TriggerDecompileState from trigger.h to decompiler.cpp 
//       since the trigger itself has minimal interaction with it. We could make it possible 
//       to pass a TriggerDecompileState* to Trigger::decompile and forward-declare the class 
//       there, but I'd like the "meat" of the decompile state to exist in the files for the 
//       decompiler itself.
//
//     = The compiler typically logs errors at the end of the affected object. In some cases, 
//       this goes especially awry; for example, if a for-each-object-with-label loop refers 
//       to an invalid trigger, the log is positioned at the end of the block rather than the 
//       start. To fix this, we'll need to modify how ParsedItem stores its start and end 
//       position: instead of separating values out, the start should be a string_scanner::pos 
//       and the end should be a single offset.
//
//       Once that's done, we'll want to audit all error messaging and whenever possible, have 
//       errors use the start of the relevant ParsedItem. (That still won't be perfect, since 
//       we often don't even create a ParsedItem until we've parsed enough text to know what 
//       we're dealing with, but it'll vastly improve certain situations like for loops having 
//       bad labels.)
//
//     = (DE)COMPILER UI
//
//        - Decompiler: before writing the decompiled text to the textbox, check if the content 
//          of the checkbox is non-empty/non-whitespace and differs from the text to be written; 
//          if so, confirm before overwriting it. (Currently needed since QTextEdit::setPlainText 
//          apparently yeets the entire undo history into the void. If we find a way to prevent 
//          that, then we may not need to be this careful.)
//
//        - Script warning/error log
//
//           - Set icons on each QListWidgetItem depending on whether it is a warning, error, 
//             or fatal error. Modify the QListWidgetEx's copy-transform functor to check the 
//             icon and prepend the message type to the copied text (e.g. "[ERROR]").
//
//           - Add a context menu that lets the users copy all selected items or the full log. 
//             (Yes, that'd be redundant, but I see little issue with that.)
//
//        - Syntax highlighting in the code editor
//
//     = COMPILER TESTS
//
//        - When Alpha Zombies is decompiled, recompiled, and decompiled again, the second 
//          decompile produces identical output to the first. However, the resulting file is 
//          not binary-identical to the "resaved in Release build" version. I've addressed the 
//          else(if) Bungie behavior and am not sure where the new discrepancy lies.
//
//     = DEFERRED TASKS
//
//        - If a string literal matches multiple existing strings, we throw a hard error; 
//          when we implement picking existing strings for unresolved string references, make 
//          an ambiguous string reference count as an unresolved one with no option to create 
//          the referenced string (because it already exists in multiple places).
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
//     = AUDITING
//
//        - Exception safety for anything that gets heap-allocated.
//
//        - DO A PROJECT-WIDE SEARCH FOR THE WORD "TODO".
//
//        - More generally, test every non-fatal error and every fatal error. Once the 
//          compiler's fully written, write a single test script containing every possible 
//          non-fatal error.
//
//     = RANDOM NOTES
//
//        - Kornman00 and Assembly both identify the unknown movement option as "double jump," 
//          but in my tests, it didn't seem to work. I even tested with Jumper Jumper disabled 
//          in case that was interfering, and nope. Nothin'.
//
//     = TESTS FOR ONCE WE HAVE A WORKING COMPILER
//
//        - Round-trip decompiling/recompiling/decompiling for all vanilla gametype scripts 
//          and for SvE Mythic Slayer. A successful test is one where both decompile actions 
//          produce identical (or semantically identical) output. Tests should include 
//          modified versions of the decompiled scripts that use aliases where appropriate 
//          (both because I want to provide such "source scripts" to script authors to learn 
//          from, and so we can test to ensure that aliases work properly).
//
//        - Test object.copy_rotation_from: we want to know whether the bool is an absolute 
//          bool or a relative bool (i.e. which is true and which is false). If it matches 
//          object.attach_to (which we should probably test again), then instead of a bool 
//          we should use OpcodeArgValueAttachPositionEnum. Moreover, we should reorder the 
//          arguments either way.
//
//        - Can object.face_toward be used to point objects upward or downward, or does it 
//          only affect the yaw?
//
//        - In team games, can you assign a player to a team that isn't present in a match? 
//          Some of my tests suggest you can't.
//
//        - Can user-defined functions be event handlers and still work? If so, that would 
//          indicate that nested blocks can be event handlers (something we don't currently 
//          allow).
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
//     = GAMETYPE PLANS
//
//        - Vanilla+
//
//           - Race+
//
//              - object.shape_contains only tests the centerpoints of vehicles (and 
//                possibly all objects including bipeds); this means that checkpoints 
//                and landmines don't react as accurately to large vehicles, to the 
//                point that especially large vehicles (e.g. sabres and some civilian 
//                vehicles) can render a race unplayable.
//
//                My solution to this is to spawn and attach Hill Markers to each 
//                vehicle, and test whether the vehicle, the biped, or any of the 
//                vehicle's Hill Markers (hereafter: "nodes") are inside of a shape. 
//                Each vehicle will need a custom assortment of nodes:
//
//                 - Warthogs and civilian vehicles will need four nodes: one at each 
//                   lower corner of the vehicle. The units in Megalo seem to be 
//                   whole Forge units, so precise positioning may be difficult.
//
//                    - We also have object.place_between_me_and, but KSoft.Tool 
//                      recently renamed that to "create tunnel." We need to test it 
//                      so we can figure out what that even means; if it indeed 
//                      creates an object equidistant between two existing objects, 
//                      then we can use it for sub-unit positioning.
//
//                 - A Sabre's nodes will form a rough triangle shape, with nodes at 
//                   the midpoints of each edge. We can't place a Sabre in Forge, so 
//                   we can't easily plan this out; fortunately, nodes can be any 
//                   immobile object, so we can simply spawn something visible and 
//                   trial-and-error our way to victory.
//
//                 - If we store the nodes as a linked list, then we can use fewer 
//                   variables. A recursive user-defined function could be used to 
//                   compare a shape boundary against any vehicle and its nodes.
//
//              - Edit the variant settings to have the mine proximity set to 10 feet. 
//                Do this in the main window, not the script editor window; the script 
//                editor window already flags that value as the default but that won't 
//                override the already-chosen value.
//
//           - Infection+
//
//              - Base on Alpha Zombies.
//
//              - We can fix the bug where rounds don't end if all zombies run out of 
//                lives, if we manually track each player's number of remaining lives 
//                and use this as the backbone for a handler that will run when all 
//                zombies are at the respawn screen. (The handler would end the round 
//                immediately if it thinks that all zombies are out of lives; otherwise 
//                it would end the round if all zombies remain dead for longer than the 
//                base spawn time plus the suicide and betrayal penalties. The second 
//                case is for emergencies e.g. host migration wiping the life tracker; 
//                it could be refined e.g. by tracking whether a zombie's most recent 
//                death was by suicide.)
//
//              - We can introduce Halo 3's "Next Zombie" option if we use a hidden 
//                scoreboard stat to track cross-round state.
//
//        - Minesweeper
//
//           - Horizontal board. A flag and a bomb spawn on an elevated platform that 
//             overlooks the entire board. Drop the flag on a square to mark it with a 
//             flag. Drop a bomb on a square to "click" it.
//
//           - A teleporter provides access between the overlook and the board. Maybe 
//             we can label each teleporter (e.g. "UP" and "DOWN") with a gametype 
//             label e.g. "MINESWEEP_TEXT_LABEL" where the spawn sequence controls the 
//             text.
//
//           - FFA: One person solves the board. Team: One team solves the board, with 
//             team members taking turns. In both cases there should be an option to 
//             let people who aren't working the board see where the mines are, to 
//             potentially add a party game aspect to things.
//
//              - Another good option: control the round end condition. By default, FFA 
//                should end the round on the first failure, and Team when all team 
//                members are killed; an alternate mode could allow other players/teams 
//                to continue from where a failing player/team left off, with the round 
//                ending on victory or when all players are dead.
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
//  - IN-GAME TESTS
//
//     - Some game-namespace numbers that refer to social options are unknown; identify 
//       them.
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