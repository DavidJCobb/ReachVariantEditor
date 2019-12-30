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