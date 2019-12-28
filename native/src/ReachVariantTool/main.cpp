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
//     - Forge variants are just Megalo variants with additional data at 
//       the end: two bits for flags (the first of which is another "open 
//       channel voice" flag); two bits for edit mode (enum; probably from 
//       Halo 3's "only party leader can edit" setting); six bits for the 
//       respawn time to use in edit mode (seconds); and then player traits 
//       for players in edit mode.
//
//       It'd be trivial to add support for these, and perhaps even to let 
//       users switch a game variant between Megalo and Forge modes. I wonder 
//       if that would allow access to Forge via Custom Games.
//
//     - Firefight hasn't been decoded (see below).
//
//     - Campaign has been decoded, but I'd be surprised if it were used for 
//       anything other than Matchmaking. Campaign variants are very minimal; 
//       there isn't much to load at all.
//
//     = It will be easier for the UI to support wildly divergent game modes 
//       once we've split everything up and moved the code into different 
//       files.
//
//  - Make sure that our code fails gracefully if we try to load a Campaign, 
//    Firefight, or Forge variant.
//
//     - Firefight hasn't been reverse-engineered yet. Now that we know 
//       that the file formats are bit-aligned, we could conceivably figure 
//       it out by testing every possible value for every possible setting 
//       and paying careful attention to what bits change where... but 
//       that would take a tremendous amount of time.
//
//        - Firefight Limited, at least, only has 0x346 bytes of SHA-1-
//          hashed data.
//
//  - See about reorganizing all of our UI code -- moving everything into 
//    the "ui" subfolder, etc..
//
//     - Split up game_variants/base.h.
//
//  - Investigate overhauling bit_or_byte_reader and its writer counterpart: 
//    consider making a single class that handles bits and bytes, and offering 
//    two classes (one for bits and one for bytes) that serve as interfaces 
//    and accessors to it.
//
//  - Delete helpers/pointer_to_member.h.
//
//  - Fix cobb::sprintfp. IIRC the functions Microsoft provides for printf 
//    with positional parameter support -- they assert if the buffer is too 
//    small, rather than returning the number of chars written or the number 
//    of chars that would've been written.
//
//     - We use cobb::sprintfp to stringify OpcodeArgValueScalar when it 
//       holds a player or team stat index. That's the only place we use it. 
//       If we do a UI for trigger editing, we'd probably want to use QString 
//       instead so we can have localization support, and that has its own 
//       positional parameter implementation.
//
//       We might well be able to remove cobb::sprintfp entirely; we could 
//       just switch it to normal cobb::sprintf for now and then later move 
//       to QString.
//
//  - Consider making INISettingManager consistent with my newer naming and 
//    coding styles.
//
//     - Consider making it configurable such that a program could use it 
//       to have multiple INI files, and then moving it to helpers. I'm 
//       thinking that you could template the settings object on a getter 
//       that returns INISettingManager& such that each settings object 
//       auto-registers itself with whichever manager [is returned by the 
//       getter] it's templated on. Then, you could have one manager per 
//       INI file, each with its own path and contents.
//
//       We don't need all that for this program, but that's what it'd 
//       take for me to want to move the INI stuff to helpers (while 
//       keeping the actual INI setting definitions separate since those 
//       are program-specific; ditto for the manager and a setting typedef 
//       on it).
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
//     - Implement editing for localized strings; start with team names.
//
//        - Alternatively, hide the button for that and save string editing 
//          for a future release.
//
//           - Yeah, just remove the button entirely for now.
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
//        - After we do this, I wonder if we should make ReachEditorState 
//          a QObject and have it emit a signal when a variant is loaded; 
//          then, each individual UI page could just QObject::connect to 
//          catch that, instead of us needing some central place to call 
//          on everything.
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
//        - This will be somewhat easier if we split each page into its 
//          own widget, I think.
//
//     - Remove the option to allow/disallow editing of unsafe Custom Game 
//       options. I only added it after mistaking the cause of some CTDs I 
//       was getting; there aren't any actual unsafe options (besides having 
//       out-of-bounds values in some bitfields).
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