#include "ReachVariantTool.h"
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
//  - Make the "engine icon" and "engine category" alterable.
//
//     - Done, but I still need to determine whether I'm allowed to use the 
//       icons I'm currently using in the icon drop-down box.
//
//        - I've put the icons into my .gitignore until I know for sure, so 
//          when I get an answer (or if I make replacements that are usable) 
//          I need to take that back out.
//
//        - Consider just redrawing them by eye, like you did for NorthernUI.
//
//     - It'd be nice if the icon drop-down box's popup showed the icons at 
//       a larger size. However, it's devilishly difficult to do this in Qt, 
//       if it's even possible at all without totally reimplementing the 
//       drop-down widget.
//
//  - Consider making it so that you can open files by dragging them onto the 
//    window.
//
//  - We'd benefit from being able to warn on unsaved changes. Easiest way 
//    would be to keep two copies of a variant in memory, and compare them.
//
//  - When we rebuild navigation in the main window, all tree items are expanded. 
//    This is an ugly hack to make the fact that we don't remember and restore 
//    their states less noticeable. Can we improve anything in this area?
//
//  - Can we improve our error-reporting system to (at least optionally) store 
//    the stream bit/byte offset at the time of failure? Every failure point 
//    has access to the stream object, I believe.
//
//  - The engine category drop-down needs to be able to handle the presence 
//    of an invalid category when loading a file. Testing shows that custom 
//    category indices (including "Community") don't work: they don't appear 
//    in the MCC at all.
//
//     - At the very least, we should warn on load and then reset the category 
//       based on the icon, using the "Insane" category for any unconventional 
//       icons.
//
//  - Investigate Firefight, Campaign, and Forge variants.
//
//     - Firefight hasn't been decoded (see below).
//
//     - Campaign has been decoded, but I'd be surprised if it were used for 
//       anything other than Matchmaking. Campaign variants are very minimal; 
//       there isn't much to load at all.
//
//  - See about reorganizing all of our UI code -- moving everything into 
//    the "ui" subfolder, etc..
//
//  - Investigate overhauling bit_or_byte_reader and its writer counterpart: 
//    consider making a single class that handles bits and bytes, and offering 
//    two classes (one for bits and one for bytes) that serve as interfaces 
//    and accessors to it.
//
//  - Change how we load blocks: we should loop over all blocks in the file, 
//    instead of assuming that CHDR and MPVR must be at the start. I think 
//    the game itself makes that assumption but I'm not sure (wouldn't be 
//    hard to hex-edit a dummy block between them and find out; maybe I 
//    should).
//
//     - We could have GameVariant store a std::vector of ReachFileBlock 
//       instances, and each instance could have a pointer to a "data" 
//       object with virtual read/save methods. You wouldn't call those 
//       directly; rather, you'd call read/save on the block, and it would 
//       call read/save on its data object (and this in turn would allow 
//       the block to do things like block length fixup after writing).
//
//  - UI
//
//     - The models I built for the Option Toggles tree-views suck. They were 
//       good as a "just build one of these for the first time and get it 
//       working at all" thing but I should redesign them. Among other things, 
//       I should actually store the model items as real trees.
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
//     - Shield Delay
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
//        - Probably a bool trait. Could try setting off explosives near 
//          oneself to see.
//
//        - May require someone to test with.
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