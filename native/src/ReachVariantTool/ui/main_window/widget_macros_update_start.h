//
// Yes, I know, preprocessor macros are ugly, but templates don't support pointers-to-members-of-members, 
// so this is the only way to do this short of manually copying-and-pasting code. There's a file that 
// #undefs all of these, so we can at least avoid pollution.
//
#pragma region Main window
   //
   // These assume that there is some variable (mp) which contains the field that you want to pull values 
   // from. Despite the name, that variable doesn't necessarily need to be multiplayer data in specific.
   //
   #define reach_main_window_update_combobox(w, field) \
      { \
         auto widget = w; \
         const QSignalBlocker blocker(widget); \
         widget->setCurrentIndex((int)mp->##field ); \
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
         widget->setValue((int)mp->##field ); \
      };
#pragma endregion