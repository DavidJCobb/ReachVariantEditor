#include "GametypeIconCombobox.h"
#include <QIcon>

namespace {
   const char* _icons[] = {
      "Capture the Flag",
      "Slayer",
      "Oddball",
      "King of the Hill",
      "Juggernaut",
      "Territories",
      "Assault",
      "Infection",
      "VIP",
      "Invasion",
      "Invasion Slayer",
      "Stockpile",
      "Action Sack",
      "Race and Rally",
      "Rocket Race",
      "Grifball",
      "Soccer",
      "Headhunter",
      "Crosshair",
      "Wheel",
      "Insane",
      "Bunker",
      "Health",
      "Defend Castle",
      "Arrow In Box",
      "Shapes",
      "Forerunner Terminal",
      "8-Ball",
      "Noble Team Insignia",
      "Covenant Insignia",
      "Capture Waypoint",
      "Defend Waypoint",
      "Resource Waypoint",
      "Triangle Waypoint",
      "Target Waypoint",
      "Arrow-Out Waypoint",
      "Ammo",
      "Campaign Skull",
      "Forge",
   };
}
GametypeIconCombobox::GametypeIconCombobox(QWidget* parent = nullptr) : IconOnlyCombobox(parent) {
   for (uint8_t i = 0; i < std::extent<decltype(_icons)>::value; i++) {
      auto name = tr(_icons[i], "Gametype Icon");
      this->addItem(QIcon(QString(":/ReachVariantTool/Resources/gametype_icons/%1.png").arg(i)), name);
      this->setItemData(i, name, Qt::ToolTipRole);
      this->setExpandedIconSize(QSize(24, 24));
   }
}