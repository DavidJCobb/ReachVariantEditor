#include "ui/main_window.h"
#include <QtWidgets/QApplication>

#include "game_variants/base.h"
#include "helpers/endianness.h"
#include "helpers/files.h"
#include "helpers/stream.h"
#include "services/ini.h"
#if _DEBUG
   #include <QDebug>
   #include <QDirIterator>
#endif

int main(int argc, char *argv[]) {
   ReachINI::get().load();
   //
   QApplication a(argc, argv);
   //
   #if _DEBUG // log all qt resources to the "output" tab in the debugger
      QDirIterator it(":", QDirIterator::Subdirectories);
      while (it.hasNext()) {
         qDebug() << it.next();
      }
   #endif
   //
   ReachVariantTool w;
   w.show();
   return a.exec();
}

//
// FIREFIGHT PLANS
//
//  - We need to decode the "athr" file block; official and built-in Firefight variants use 
//    that instead of "chdr".
//
//  - Finish adding UI for all Firefight options.
//
//     - Base Wave Traits
//
//     - Custom skull configuration
//
//     - Bonus Wave configuration
//

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
//     - The compiler interprets "0x" and "0b" as valid integer literals.
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
//     - A disabled team with a non-zero initial designator is invalid: MCC considers the file 
//       corrupt. What about an enabled team with a zero initial designator?
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
//           - Add a context menu that lets the users copy all selected items or the full log. 
//             (Yes, that'd be redundant, but I see little issue with that.)
//
//        - Syntax highlighting in the code editor
//
//     - OPENING A ZERO-BYTE FILE SHOWS AN INCORRECT ERROR MESSAGE (WinAPI code 0x3EE: the volume 
//       for the file has been externally altered).
//
//     - Opening a file with an MPVR block that is too large (size > 0x5028) yields only a very 
//       generic error message stating that "something went wrong while reading the multiplayer 
//       data."
//
//        - Wonder if we should alert the user to this case and then give them the option to 
//          open the file anyway. Would require us to implement a "load process" complimentary 
//          to the existing "save process" so we can pass data, options, etc., down to load 
//          code.
//
//     = UX FOR FILE SAVE PROCESS: Do we want to generate the file data to write before asking 
//       the user where to save it? That would allow us to suggest a different file extension 
//       for editor-only (i.e. xRVT) variants.
//
//        - If we make a custom confirmation dialog, we can do things like text formatting 
//          (i.e. bolding the "you will not lose work" bit) and having a mandatory delay before 
//          clicking "yes."
//
//     = THE object.place_at_me OPCODE ALLOWS ASSIGNING TO no_object AS A WAY OF CREATING AN 
//       OBJECT WITHOUT BOTHERING TO KEEP A REFERENCE TO IT. WE NEED TO GIVE OPCODES A WAY TO 
//       SIGNAL THAT THEY ALLOW ASSIGNING THEIR RETURN VALUES TO "NONE", AND WE NEED TO 
//       IMPLEMENT THE SYNTAX (FOR COMPILING AND DECOMPILING) AS SIMPLY NOT USING AN ASSIGN 
//       STATEMENT I.E. (basis.place_at_me(...)) INSTEAD OF (no_object = basis.place_at_me(...)).
//
//        - We should test if object.place_between_me_and also has this behavior.
//
//     = DOCUMENTATION
//
//        - We need to decompile all official gametypes and decode them as much as we can, and 
//          provide aliases for them. We should provide these as "source scripts."
//
//           - DONE:
//              - Assault
//              - Capture the Flag TU (includes Flag Slayer)
//                 - Speedflag
//              - Freeze Tag
//              - Headhunter TU
//              - Infection (Alpha Zombies)
//              - Invasion
//              - Invasion Slayer
//              - Juggernaut
//              - King of the Hill (use TU file; I misunderstood the scoring the first time around)
//              - Oddball
//              - Race (but double-check it again now that we've improved our tools)
//              - Rally
//                 - Rocket Hog Race
//              - Skeeball / "HaloBall"
//              - Slayer
//                 - Power Slayer
//                 - Slayer TU (includes Buddy Slayer code)
//              - Stockpile
//                 - Speedpile
//              - Territories
//
//           - PENDING:
//              - Halo Chess (high-complexity; low-priority)
//              - Invasion: Boneyard
//              - Invasion: Breakpoint
//              - Invasion: Spire
//
//     - The load process can't handle out-of-range indices in indexed lists for files that 
//       use the xRVT file block, e.g. accessing widget 1 in a gametype that has no widgets. 
//       However, we're the only ones generating files like this so malformed data like that 
//       shouldn't occur (if it does, we screwed up and we'll take the L). This is worth noting 
//       but not a priority to fix at present.
//
//        = The reason we "can't handle" this is because we do postprocess (including the code 
//          to finalize indexed lists) immediately at the end of loading the core MP data, but 
//          we would end up "seeing" the xRVT chunk after. If the script content is moved to 
//          the xRVT chunk and it has an out-of-range reference to a widget or something, then 
//          by the time we actually load that script content, we've already shrunk the indexed 
//          lists to fit and we'll probably get a bad pointer or something.
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
//        - Test the film clip opcode, now that MCC has theater. Race+ would be a good test 
//          candidate: saving a film checkpoint the first time each lap is completed by the 
//          player in the lead would be useful.
//
//        - Test what happens if script code forces a player to control a dead biped. Based 
//          on bugs I'm seeing in Halo Chess+ I suspect this would lead to the camera angle 
//          flipping 180, to a constant vibrating/impact noise, and to the screen being 
//          faded to a very deep blue (almost black), with the fade mostly vanishing when 
//          you pause the game.
//
//        - Test the "reset round" flags -- specifically, test what happens when they're 
//          turned off. (This isn't related to Megalo but oh well, this is my in-game test 
//          list now)
//
//        - What happens if we attach the player to an object that is destroyed or equipped 
//          on contact (e.g. a powerup, a landmine; an armor ability or weapon when they 
//          are not carrying one)?
//
//        - Re-test setting a vehicle's maximum health; use a constant like 150; see if it 
//          still sets current health to 1 without changing max health and if so, document 
//          that.
//
//        - Can you assign a player to neutral_team, or reassign their team, during a team 
//          game?
//
//        - In team games, can you assign an object to a team that isn't present in a match? 
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
//     = POSSIBLE COMPILER ENHANCEMENTS
//
//        - #pragma: region
//          #pragma: region: [name]
//          #pragma: endregion
//
//        - #pragma: same-scope alias shadowing: [warn|error] on [alias name]
//
//     = GAMETYPE PLANS
//
//        - Vanilla+
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
//        - Tetris?
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
//     - Consider adding multiple bitcount getters to the MP object e.g. header_bitcount, 
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
//  - POTENTIAL EDITOR IMPROVEMENTS:
//
//     - String table: warn when loaded count exceeds max count
//
// ======================================================================================
//
//  - Consider adding an in-app help manual explaining the various settings and 
//    traits.
//
//  - In Alpha Zombies, where are the "Humans" and "Zombies" strings near the top of the 
//    variant used? *Are* they used?
//
//  - STRING TABLE EDITING
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