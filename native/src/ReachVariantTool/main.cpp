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
//  - Consider adding an in-app help manual explaining the various settings and 
//    traits.
//
//  - FORGE LABEL EDITING
//
//     - STRING PICKER WIDGET
//
//        - When the combobox is changed, the widget should send a signal and 
//          modify the target ReachStringRef.
//
//        - Make it possible to disable specific strings within the list, so 
//          that you can't set two Forge labels to use the same string.
//
//        - Clicking the button should open a string editor modal.
//
//     - Implement the buttons for adding, reordering, and removing labels. 
//       The user should not be allowed to remove a label that is in use by any 
//       part of the gametype script (cobb::reference_tracked_object has member 
//       functions we can use to check this).
//
//  - STRING TABLE EDITING
//
//     - The string list needs to be updated after changes to a string are saved.
//
//     - Strings can contain line breaks, but our UI doesn't easily allow for 
//       this. Modify the localized string editor: replace each QLineEdit with 
//       a multi-line plaintext box in the dialog box.
//
//     - Implement the "New" and "Delete" buttons, and make it so that double-
//       clicking on a string opens it for editing.
//
//        - (Once we have "New" implemented, we can also add a new Forge label 
//          for testing's sake -- once we get the buttons there implemented as 
//          well, of course.)
//
//  - Work on script editor
//
//     = MAKE THE EDITOR WINDOW A MODAL
//
//     - General thoughts
//
//        - Right now, we have a lot of disconnected data -- objects that refer 
//          to each other by index, such that these indices need to be manually 
//          fixed up when the referred-to objects are removed, reordered, and so 
//          on. Opcode arguments refer to Forge labels, scripted stats, scripted 
//          options, string indices, and more all by index. This is unavoidable 
//          when loading data -- some of these items load after the trigger 
//          content -- but after everything's loaded, I think we might benefit 
//          from doing a one-time fix-up on everything, swapping out indices for 
//          pointers.
//
//           = THIS IS IMPLEMENTED FOR FORGE LABELS; WE'LL HAVE TO GET AROUND TO 
//             IMPLEMENTING IT FOR EVERYTHING ELSE.
//
//     - Metadata strings
//
//     - Map Permissions - DONE
//
//     - Player rating parameters
//
//        - This doesn't refer to anything external and is not referred to by 
//          anything external, at least as far as anyone knows, so it should be 
//          super easy to just add an editor for real quick.
//
//     - Forge Labels
//
//        - Viewing - DONE
//
//        - Implement editing of basic properties - IN PROGRESS
//
//        - Implement editing of names
//
//           - The dialog for localized strings needs "Save," "Cancel," and 
//             "Save as New" buttons. When opened, it needs to be aware of 
//             what string it's editing (i.e. it needs a MegaloStringIndex* or 
//             a MegaloStringIndexOptional*) so that in the case of Save As New, 
//             it can update the incoming index field.
//
//        - Implement adding, removing, and reordering
//
//           - Reordering requires updating all trigger conditions and actions. 
//             Not all opcodes can specify a "none" label so removing may not 
//             be possible if the label is in use -- investigate.
//
//              - We should call a function that gets us a list of every opcode 
//                argument that uses any Forge label, and then scan that list 
//                to see if any of them refer to the label we want to delete. If 
//                so, we fail the deletion (and when we implement script editing, 
//                we can use this list to let the user jump to any opcode that 
//                uses the label). If the label is not in use, then we delete it; 
//                if it's in the middle of the label list, then this will shift 
//                the indices of all labels that come after it, which is why we 
//                need a list of ALL opcode arguments that use ANY Forge label: 
//                so we can adjust the ones whose labels have shifted.
//
//                 - It might be good to generate Forge label use info in advance 
//                   and let the user ask to view that.
//
//        - If there aren't enough labels to fully fill the window, then the last 
//          one gets stretched out. I've tried adding a spacer to the bottom of 
//          the (programmatically generated) layout, but that's not working.
//
//     - Scripted stats
//
//        - Basically all of the same work needs to be done as with Forge labels
//
//     - Scripted options
//
//        - Viewing
//
//        - Editing
//
//        - Removing or reordering
//
//           - Like with Forge labels, we need to fail if the option to be removed 
//             is in use by any opcodes, and we need to handle reordering the 
//             options (and fixing up option indices used in opcodes) that follow 
//             the one to be deleted. We also need to update the main window in 
//             response to the options being changed.
//
//        = NOTES
//
//           - Invasion Slayer is the only gametype I've found with a slider option 
//             (Vehicle Limit), but that option is flagged as hidden and I'm not sure 
//             it has a description (so what does it do?). I've tested and verified 
//             that slider options never display in MCC's UI, even when the disabled 
//             and hidden flags for them are cleared.
//
//     - Scripted traits
//
//        - Viewing
//
//        - Editing
//
//        - Removing or reordering
//
//           - Like with Forge labels, we need to fail if the traits-et to be removed 
//             is in use by any opcodes, and we need to handle reordering the 
//             traits (and fixing up traits indices used in opcodes) that follow 
//             the one to be deleted. We also need to update the main window in 
//             response to the traits being changed.
//
//     - String table editing
//
//        - String tables use a std::vector<ReachString> rather than using a 
//          std::vector<ReachString*>; that is, the ReachStrings are *in* the 
//          buffer. There are a lot of objects that have a function to "post-process 
//          string indices:" after the string table is loaded, it's passed to these 
//          objects so that they can take their string indices and store a pointer 
//          to the target ReachString. ALL OF THESE ARE GOING TO BREAK THE SECOND 
//          WE ALLOW THE CREATION OR REMOVAL OF STRINGS, OR MORE GENERALLY THE 
//          SECOND WE ALLOW ANYTHING THAT MIGHT INVALIDATE ITERATORS.
//
//          ReachStrings need to be heap-allocated. This would offer us a few 
//          benefits for run-time editing as well (see note above about everything 
//          being disconnected, and about switching indices for pointers as part of 
//          a global post-process for all data referred to be index).
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
//        - Defined for /game_variants/components/*.h, but not for anything 
//          in the Megalo subfolder. Those'll require virtual operators 
//          (which are allowed).
//
//        - Not defined for top-level game variant structures.
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
//  - Test whether custom block types between mpvr and _eof are kept in the 
//    file if it's resaved in-game with changes. I want to know if we can 
//    use a "cobb" block to store metadata; if we ever do a trigger editor, 
//    that would come in handy.
//
//     = Testing indicates that the game discards all unknown blocks when 
//       resaving a file. The only way to insert custom metadata would be 
//       to toss it into unused space in MPVR and hope the game ignores 
//       that.
//
//        - ...Or use a co-save, which would be better.
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
//     - Maximum jump height (game clamps or validates values; 420% did not 
//       work in-game despite being supported by the format).
//
//     - Sensors: Directional Damage Indicator
//
//        - None of the values affect whether I get a DDI on splash damage 
//          from my own grenades; they probably only affect damage dealt by 
//          other players.
//
//     - Test all engine option toggles
//
//  = KNOWN INFO:
//
//     - The Grenade Regeneration trait is a trait-bool. If enabled for a 
//       player, that player will receive one frag and one plasma on every 
//       fifth second of the round.
//
//     - Active Camo trait:
//
//        - 2: Fades very, very slightly on movement; fades on attacks
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