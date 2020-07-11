//
// Yes, I know, preprocessor macros are ugly, but templates don't support pointers-to-members-of-members, 
// so this is the only way to do this short of manually copying-and-pasting code. There's a file that 
// #undefs all of these, so we can at least avoid pollution.
//
#pragma region Main window
   #pragma region Custom Game options
      #define reach_main_window_setup_cg_combobox(w, field) \
         QObject::connect(w, QOverload<int>::of(&QComboBox::currentIndexChanged), [](int value) { \
            auto data = ReachEditorState::get().customGameOptions(); \
            if (!data) \
               return; \
            data->##field = value; \
         });
      #define reach_main_window_setup_cg_flag_checkbox(w, field, mask) \
         QObject::connect(w, &QCheckBox::stateChanged, [](int state) { \
            auto data = ReachEditorState::get().customGameOptions(); \
            if (!data) \
               return; \
            if (state == Qt::CheckState::Checked) \
               data->##field |= mask ; \
            else \
               data->##field &= ~ mask ; \
         });
      #define reach_main_window_setup_cg_bool_checkbox(w, field) \
         QObject::connect(w, &QCheckBox::stateChanged, [](int state) { \
            auto data = ReachEditorState::get().customGameOptions(); \
            if (!data) \
               return; \
            data->##field = (state == Qt::CheckState::Checked) ; \
         });
      #define reach_main_window_setup_cg_spinbox(w, field) \
         QObject::connect(w, QOverload<int>::of(&QSpinBox::valueChanged), [](int value) { \
            auto data = ReachEditorState::get().customGameOptions(); \
            if (!data) \
               return; \
            data->##field = value; \
         });
      #define reach_main_window_setup_cg_spinbox_dbl(w, field) \
         QObject::connect(w, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [](double value) { \
            auto data = ReachEditorState::get().customGameOptions(); \
            if (!data) \
               return; \
            data->##field = value; \
         });
   #pragma endregion
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

#pragma region Objects tracked by ReachEditorState
   #define reach_state_pane_setup_flag_checkbox(getter, w, field, mask) \
      QObject::connect(w, &QCheckBox::stateChanged, [](int state) { \
         auto data = ReachEditorState::get().##getter##(); \
         if (!data) \
            return; \
         if (state == Qt::CheckState::Checked) \
            data->##field |= mask ; \
         else \
            data->##field &= ~ mask ; \
      });
   #define reach_state_pane_setup_combobox(getter, w, field) \
      QObject::connect(w, QOverload<int>::of(&QComboBox::currentIndexChanged), [](int value) { \
         auto data = ReachEditorState::get().##getter##(); \
         if (!data) \
            return; \
         data->##field = value; \
      });
   #define reach_state_pane_setup_spinbox(getter, w, field) \
      QObject::connect(w, QOverload<int>::of(&QSpinBox::valueChanged), [](int value) { \
         auto data = ReachEditorState::get().##getter##(); \
         if (!data) \
            return; \
         data->##field = value; \
      });
#pragma endregion