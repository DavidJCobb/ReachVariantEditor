#define reach_main_window_setup_combobox(w, field) \
   QObject::connect(w, QOverload<int>::of(&QComboBox::currentIndexChanged), [](int value) { \
      auto data = ReachEditorState::get().multiplayerData(); \
      if (!data) \
         return; \
      data->##field = value; \
   });
#define reach_main_window_setup_flag_checkbox(w, field, mask) \
   QObject::connect(w, &QCheckBox::stateChanged, [](int state) { \
      auto data = ReachEditorState::get().multiplayerData(); \
      if (!data) \
         return; \
      if (state == Qt::CheckState::Checked) \
         data->##field |= mask ; \
      else \
         data->##field &= ~ mask ; \
   });
#define reach_main_window_setup_bool_checkbox(w, field) \
   QObject::connect(w, &QCheckBox::stateChanged, [](int state) { \
      auto data = ReachEditorState::get().multiplayerData(); \
      if (!data) \
         return; \
      data->##field = (state == Qt::CheckState::Checked) ; \
   });
#define reach_main_window_setup_spinbox(w, field) \
   QObject::connect(w, QOverload<int>::of(&QSpinBox::valueChanged), [](int value) { \
      auto data = ReachEditorState::get().multiplayerData(); \
      if (!data) \
         return; \
      data->##field = value; \
   });