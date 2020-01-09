#pragma once
#include <QComboBox>
#include <QPushButton>
#include <QWidget>
#include "../../formats/localized_string_table.h"

class ReachStringPicker : public QWidget {
   Q_OBJECT
   public:
      using Flags = ReachStringFlags::type;
   public:
      ReachStringPicker(QWidget* parent = nullptr, uint32_t flags = 0);
      //
      void addBlacklistedString(const QString&) noexcept;
      const QList<QString>& blacklist() const noexcept { return this->_blacklist; }
      void clearBlacklistedStrings() noexcept;
      void setAllowNoString(bool) noexcept;
      void setBlacklistedStrings(const QList<QString>&) noexcept;
      void setBlacklistedStrings(QList<QString>&&) noexcept;
      void setLimitedToSingleLanguageStrings(bool) noexcept;
      void setTarget(MegaloStringRef& r) noexcept {
         this->_targetRef = &r;
         this->refreshSelection();
      }
      //
   signals:
      void selectedStringChanged(); // the string is changed via the combobox, or the user clicks "Save" or "Save as New" in the editing dialog box
      //
   protected:
      MegaloStringRef* _targetRef = nullptr;
      bool _allowNoString = false;
      bool _limitToSingleLanguageStrings = false;
      QComboBox* _combobox = nullptr;
      QPushButton* _button = nullptr;
      QList<QString> _blacklist;
      //
      void refreshList();
      void refreshSelection();
      //
      void refreshListItem(uint32_t index);
};