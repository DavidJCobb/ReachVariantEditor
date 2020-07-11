#pragma region Main window
   #pragma region MP
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
      #define reach_main_window_setup_spinbox_dbl(w, field) \
         QObject::connect(w, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [](double value) { \
            auto data = ReachEditorState::get().multiplayerData(); \
            if (!data) \
               return; \
            data->##field = value; \
         });
   #pragma endregion
   #pragma region FF
      #define reach_main_window_setup_ff_combobox(w, field) \
         QObject::connect(w, QOverload<int>::of(&QComboBox::currentIndexChanged), [](int value) { \
            auto data = ReachEditorState::get().firefightData(); \
            if (!data) \
               return; \
            data->##field = value; \
         });
      #define reach_main_window_setup_ff_flag_checkbox(w, field, mask) \
         QObject::connect(w, &QCheckBox::stateChanged, [](int state) { \
            auto data = ReachEditorState::get().firefightData(); \
            if (!data) \
               return; \
            if (state == Qt::CheckState::Checked) \
               data->##field |= mask ; \
            else \
               data->##field &= ~ mask ; \
         });
      #define reach_main_window_setup_ff_bool_checkbox(w, field) \
         QObject::connect(w, &QCheckBox::stateChanged, [](int state) { \
            auto data = ReachEditorState::get().firefightData(); \
            if (!data) \
               return; \
            data->##field = (state == Qt::CheckState::Checked) ; \
         });
      #define reach_main_window_setup_ff_spinbox(w, field) \
         QObject::connect(w, QOverload<int>::of(&QSpinBox::valueChanged), [](int value) { \
            auto data = ReachEditorState::get().firefightData(); \
            if (!data) \
               return; \
            data->##field = value; \
         });
      #define reach_main_window_setup_ff_spinbox_dbl(w, field) \
         QObject::connect(w, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [](double value) { \
            auto data = ReachEditorState::get().firefightData(); \
            if (!data) \
               return; \
            data->##field = value; \
         });
   #pragma endregion
#pragma endregion

#pragma region Player traits
#define reach_traits_pane_setup_combobox(w, field) \
   QObject::connect(w, QOverload<int>::of(&QComboBox::currentIndexChanged), [](int value) { \
      auto traits = ReachEditorState::get().playerTraits(); \
      if (!traits) \
         return; \
      traits->##field = value; \
   });
#define reach_traits_pane_setup_spinbox(w, field) \
   QObject::connect(w, QOverload<int>::of(&QSpinBox::valueChanged), [](int value) { \
      auto traits = ReachEditorState::get().playerTraits(); \
      if (!traits) \
         return; \
      traits->##field = value; \
   });
#pragma endregion

#pragma region Specific team
#define reach_team_pane_setup_combobox(w, field) \
   QObject::connect(w, QOverload<int>::of(&QComboBox::currentIndexChanged), [](int value) { \
      auto team = ReachEditorState::get().multiplayerTeam(); \
      if (!team) \
         return; \
      team->##field = value; \
   });
#define reach_team_pane_setup_flag_checkbox(w, field, mask) \
   QObject::connect(w, &QCheckBox::stateChanged, [](int state) { \
      auto team = ReachEditorState::get().multiplayerTeam(); \
      if (!team) \
         return; \
      if (state == Qt::CheckState::Checked) \
         team->##field |= mask ; \
      else \
         team->##field &= ~ mask ; \
   });
#define reach_team_pane_setup_spinbox(w, field) \
   QObject::connect(w, QOverload<int>::of(&QSpinBox::valueChanged), [](int value) { \
      auto team = ReachEditorState::get().multiplayerTeam(); \
      if (!team) \
         return; \
      team->##field = value; \
   });
#pragma endregion