#include <cstdint>

namespace Megalo {
   enum class explicit_object_handle : int8_t;
   enum class explicit_player_handle : int8_t;
   enum class explicit_team_handle   : int8_t;

   union explicit_handle_union {
      int8_t untyped = -1;
      explicit_object_handle object;
      explicit_player_handle player;
      explicit_team_handle   team;
   };
}