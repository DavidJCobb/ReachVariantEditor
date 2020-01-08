#pragma once
#include <QComboBox>
#include <QPushButton>
#include <QWidget>
#include "../../formats/localized_string_table.h"

class ReachStringPicker : public QWidget {
   public:
      using Flags = ReachStringFlags::type;
   public:
      ReachStringPicker(QWidget* parent = nullptr, uint32_t flags = 0);
      //
      void setAllowNoString(bool) noexcept;
      void setLimitedToSingleLanguageStrings(bool) noexcept;
      void setTarget(MegaloStringRef& r) noexcept {
         this->_targetRef = &r;
         this->refreshSelection();
      }
      //
   protected:
      MegaloStringRef* _targetRef = nullptr;
      bool _allowNoString = false;
      bool _limitToSingleLanguageStrings = false;
      QComboBox* _combobox = nullptr;
      QPushButton* _button = nullptr;
      //
      void refreshList();
      void refreshSelection();
      //
      void refreshListItem(uint32_t index);
};