#include "GametypeIconCombobox.h"
#include <QIcon>
#include <QAbstractItemView>

namespace {
   const char* _icons[] = {
      "Capture the Flag", // 0
      "Slayer",
      "Oddball",
      "King of the Hill",
      "Juggernaut",
      "Territories",
      "Assault",
      "Infection",
      "VIP",
      "Invasion",
      "Invasion Slayer", // 10
      "Stockpile",
      "Action Sack",
      "Race and Rally",
      "Rocket Race",
      "Grifball",
      "Soccer",
      "Headhunter",
      "Crosshair",
      "Wheel",
      "Insane", // 20
      "Bunker",
      "Health",
      "Defend Castle",
      "Arrow In Box",
      //"Shapes", // 25 pre-MCC
      "Infinity", // 25 post-MCC
      "Forerunner Terminal",
      "8-Ball",
      "Noble Team Insignia",
      "Covenant Insignia",
      "Capture Waypoint", // 30
      /*// Inside of MPVR (not including its header), the engine icon is stored as a 5-bit number and -1 means "none," so these icons aren't usable.
      "Defend Waypoint",
      "Resource Waypoint",
      "Triangle Waypoint",
      "Target Waypoint",
      "Arrow-Out Waypoint",
      "Ammo",
      "Campaign Skull",
      "Forge",
      //*/
   };
}
GametypeIconCombobox::GametypeIconCombobox(QWidget* parent = nullptr) : IconOnlyCombobox(parent) {
   for (uint8_t i = 0; i < std::extent<decltype(_icons)>::value; i++) {
      auto name = tr(_icons[i], "Gametype Icon");
      if (i == 25) {
         this->addItem(QIcon(QString(":/ReachVariantTool/gametype_icons/%1_MCC.png").arg(i)), name);
      } else {
         this->addItem(QIcon(QString(":/ReachVariantTool/gametype_icons/%1.png").arg(i)), name);
      }
      this->setItemData(i, name, Qt::ToolTipRole);
   }
   this->setExpandedIconSize(QSize(24, 24));
}