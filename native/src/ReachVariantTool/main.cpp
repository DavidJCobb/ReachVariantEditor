#include "ReachVariantTool.h"
#include <QtWidgets/QApplication>

#include "game_variants/base.h"
#include "helpers/endianness.h"
#include "helpers/files.h"
#include "helpers/stream.h"
#include "formats/sha1.h"
#include "services/ini.h"

int main(int argc, char *argv[]) {
   ReachINI::INISettingManager::GetInstance().Load();
   //
   QApplication a(argc, argv);
   ReachVariantTool w;
   w.show();
   return a.exec();
}

//
// TODO:
//
//  - Investigate Firefight, Campaign, and Forge variants. Consider splitting 
//    MPVR up into multiple classes in order to allow us to load the other 
//    types.
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
//     - File header
//
//        - Implement editing of the created and edited datestamps.
//
//        - Add a button to zero out the XUID and set the "is online ID" 
//          bool to false. Label it "Strip Xbox LIVE Identifying Info."
//
//     - Implement editing for localized strings; start with team names.
//
//        - Alternatively, hide the button for that and save string editing 
//          for a future release.
//
//     - Player Traits pages should not be selectable if a variant isn't 
//       open.
//
//        - Ditto for specific team options.
//
//     - Strongly consider splitting all of the main window's pages into 
//       their own *.ui files as subclasses of QFrame. The main window could 
//       then be made to contain QFrames that we promote to the subclasses.
//
//        - We already prefix basically every important widget so this 
//          wouldn't really change much. We could probably even do a simple 
//          find-and-replace for the widget names here.
//
//        - We could potentially set the split-up pages to be friendly to 
//          Qt Designer if we compile our program as a DLL (that's what that 
//          static linkage error re: QDESIGNER_WIDGET_EXPORT was about; it's 
//          not that we were failing to dynamically link with Qt but rather 
//          that the program containing the widget-definition needs to be a 
//          DLL that Qt Designer can load). Might be overkill, though.
//
//     - The models I built for the Option Toggles tree-views suck. They were 
//       good as a "just build one of these for the first time and get it 
//       working at all" thing but I should redesign them. Among other things, 
//       I should have the model classes pull directly from the bitsets if 
//       possible (I don't know if the QModelIndex class allows for that; it 
//       seems to have to wrap a pointer).
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
//     - Remove the option to allow/disallow editing of unsafe Custom Game 
//       options. I only added it after mistaking the cause of some CTDs I 
//       was getting; there aren't any actual unsafe options (besides having 
//       out-of-bounds values in some bitfields).
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