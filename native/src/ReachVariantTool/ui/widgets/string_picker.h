#pragma once
#include <QComboBox>
#include <QPushButton>
#include <QWidget>
#include "../../formats/localized_string_table.h"

//
// ReachStringPicker
//
// You can make this widget point at any MegaloStringRef (i.e. any refcounted pointer to a ReachString) 
// and then have the widget manage that pointer for you: if the user changes which string that pointer 
// should use via the UI, the widget will automatically update that pointer. The typical use case is to 
// use this widget for string members of indexed data items, e.g. the name of a set of player traits; 
// when the user picks a different set of player traits, you repoint the widget at that set's "name" 
// pointer.
//
// With the typical use case, you shouldn't be updating the MegaloStringRef out from under the picker. 
// However, if for some reason you need to do that, you can get the picker to react to the change by 
// calling ReachStringPicker::refreshSelection directly.
//
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
      void refreshSelection();
      void setAllowNoString(bool) noexcept;
      void setBlacklistedStrings(const QList<QString>&) noexcept;
      void setBlacklistedStrings(QList<QString>&&) noexcept;
      void setLimitedToSingleLanguageStrings(bool) noexcept;
      void setTarget(MegaloStringRef& r) noexcept {
         this->_targetRef = &r;
         this->refreshSelection();
      }
      void clearTarget() noexcept {
         this->_targetRef = nullptr;
         this->refreshSelection();
      }
      //
   signals:
      void selectedStringChanged(); // the string is changed via the combobox, or the user clicks "Save" or "Save as New" in the editing dialog box
      void selectedStringSwitched(); // the string is changed via the combobox only
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
      //
      void refreshListItem(uint32_t index);
      //
      #if _DEBUG
         QString _debug_hierarchy;
      #endif
};