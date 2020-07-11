//
// Yes, I know, preprocessor macros are ugly, but templates don't support pointers-to-members-of-members, 
// so this is the only way to do this short of manually copying-and-pasting code. There's a file that 
// #undefs all of these, so we can at least avoid pollution.
//
// The arguments common to all of these macros are, in order:
//
//    getter
//       The name of a getter function on ReachEditorState. It should return a pointer.
//
//    w
//       The widget to set up.
//
//    field
//       A member (or member of a member) of the type returned by the specified getter. When accessing 
//       nested members, use "." as a separator, not "::".
//
// EXAMPLE:
///   #include "widget_macros_setup_start.h"
///   reach_main_window_setup_flag_checkbox(customGameOptions, this->ui.fieldBetrayalBooting, social.flags, ReachCGSocialOptions::flags_t::betrayal_booting);
//    // customGameOptions returns ReachCustomGameOptions*; that has a member ReachCustomGameOptions::social::flags.
///   #include "widget_macros_setup_end.h" // undefines the macros
//
#define reach_main_window_setup_flag_checkbox(getter, w, field, mask) \
   QObject::connect(w, &QCheckBox::stateChanged, [](int state) { \
      auto data = ReachEditorState::get().##getter##(); \
      if (!data) \
         return; \
      if (state == Qt::CheckState::Checked) \
         data->##field |= mask ; \
      else \
         data->##field &= ~ mask ; \
   });
#define reach_main_window_setup_bool_checkbox(getter, w, field) \
   QObject::connect(w, &QCheckBox::stateChanged, [](int state) { \
      auto data = ReachEditorState::get().##getter##(); \
      if (!data) \
         return; \
      data->##field = (state == Qt::CheckState::Checked) ; \
   });
#define reach_main_window_setup_combobox(getter, w, field) \
   QObject::connect(w, QOverload<int>::of(&QComboBox::currentIndexChanged), [](int value) { \
      auto data = ReachEditorState::get().##getter##(); \
      if (!data) \
         return; \
      data->##field = value; \
   });
#define reach_main_window_setup_spinbox(getter, w, field) \
   QObject::connect(w, QOverload<int>::of(&QSpinBox::valueChanged), [](int value) { \
      auto data = ReachEditorState::get().##getter##(); \
      if (!data) \
         return; \
      data->##field = value; \
   });
#define reach_main_window_setup_spinbox_dbl(getter, w, field) \
   QObject::connect(w, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [](double value) { \
      auto data = ReachEditorState::get().##getter##(); \
      if (!data) \
         return; \
      data->##field = value; \
   });