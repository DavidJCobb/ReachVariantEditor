#pragma region Main window
#define reach_main_window_update_combobox(w, field) \
   { \
      auto widget = w; \
      const QSignalBlocker blocker(widget); \
      widget->setCurrentIndex(mp->##field ); \
   };
#define reach_main_window_update_flag_checkbox(w, field, mask) \
   { \
      auto widget = w; \
      const QSignalBlocker blocker(widget); \
      widget->setChecked((mp->##field & mask ) != 0); \
   };
#define reach_main_window_update_bool_checkbox(w, field) \
   { \
      auto widget = w; \
      const QSignalBlocker blocker(widget); \
      widget->setChecked(mp->##field ); \
   };
#define reach_main_window_update_spinbox(w, field) \
   { \
      auto widget = w; \
      const QSignalBlocker blocker(widget); \
      widget->setValue( mp->##field ); \
   };
#pragma endregion

#pragma region Player traits
#define reach_traits_pane_update_combobox(w, field) \
   { \
      auto widget = w; \
      const QSignalBlocker blocker(widget); \
      widget->setCurrentIndex((int)traits->##field ); \
   }
#define reach_traits_pane_update_spinbox(w, field) \
   { \
      auto widget = w; \
      const QSignalBlocker blocker(widget); \
      widget->setValue((int)traits->##field ); \
   }
#pragma endregion

#pragma region Specific team
#define reach_team_pane_update_flag_checkbox(w, field, mask) \
   { \
      QCheckBox* widget = w; \
      const QSignalBlocker blocker(widget); \
      widget->setChecked((team->##field & mask) != 0); \
   };
#define reach_team_pane_update_combobox(w, field) \
   { \
      auto widget = w; \
      const QSignalBlocker blocker(widget); \
      widget->setCurrentIndex(team->##field ); \
   };
#define reach_team_pane_update_spinbox(w, field) \
   { \
      auto widget = w; \
      const QSignalBlocker blocker(widget); \
      widget->setValue(team->##field ); \
   };
#pragma endregion