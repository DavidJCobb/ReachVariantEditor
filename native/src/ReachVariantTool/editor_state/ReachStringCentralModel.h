#pragma once
#include <array>
#include <vector>
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include "../formats/localized_string_table.h"

class ReachStringCentralModel : public QAbstractItemModel {
   Q_OBJECT;
   public:
      ReachStringCentralModel(QObject* parent = nullptr);

      static constexpr const Qt::ItemDataRole StringIndexRole                  = (Qt::ItemDataRole)(Qt::UserRole + 50);
      static constexpr const Qt::ItemDataRole StringLocalizationsIdenticalRole = (Qt::ItemDataRole)(Qt::UserRole + 51);

      static constexpr const Qt::ItemDataRole LocalizationChineseSimpRole  = (Qt::ItemDataRole)(Qt::UserRole + (int)reach::language::chinese_simplified);
      static constexpr const Qt::ItemDataRole LocalizationChineseTradRole  = (Qt::ItemDataRole)(Qt::UserRole + (int)reach::language::chinese_traditional);
      static constexpr const Qt::ItemDataRole LocalizationEnglishRole      = (Qt::ItemDataRole)(Qt::UserRole + (int)reach::language::english);
      static constexpr const Qt::ItemDataRole LocalizationFrenchRole       = (Qt::ItemDataRole)(Qt::UserRole + (int)reach::language::french);
      static constexpr const Qt::ItemDataRole LocalizationGermanRole       = (Qt::ItemDataRole)(Qt::UserRole + (int)reach::language::german);
      static constexpr const Qt::ItemDataRole LocalizationItalianRole      = (Qt::ItemDataRole)(Qt::UserRole + (int)reach::language::italian);
      static constexpr const Qt::ItemDataRole LocalizationJapaneseRole     = (Qt::ItemDataRole)(Qt::UserRole + (int)reach::language::japanese);
      static constexpr const Qt::ItemDataRole LocalizationKoreanRole       = (Qt::ItemDataRole)(Qt::UserRole + (int)reach::language::korean);
      static constexpr const Qt::ItemDataRole LocalizationPolishRole       = (Qt::ItemDataRole)(Qt::UserRole + (int)reach::language::polish);
      static constexpr const Qt::ItemDataRole LocalizationPortugeseRole    = (Qt::ItemDataRole)(Qt::UserRole + (int)reach::language::portugese);
      static constexpr const Qt::ItemDataRole LocalizationSpanishRole      = (Qt::ItemDataRole)(Qt::UserRole + (int)reach::language::spanish);
      static constexpr const Qt::ItemDataRole LocalizationSpanishLatAmRole = (Qt::ItemDataRole)(Qt::UserRole + (int)reach::language::mexican);

   protected:
      struct _stored_string {
         std::array<QString, reach::language_count> localizations;
         bool all_same_language = false;

         void reload(const ReachString&);
      };
      std::vector<_stored_string> _contents;

      void _clear();

      void _reload_all();
      void _reload_one(uint32_t index);

      const _stored_string* _entry_for_qmi(const QModelIndex& qmi) const;
      _stored_string* _entry_for_qmi(const QModelIndex& qmi) {
         return const_cast<_stored_string*>(std::as_const(*this)._entry_for_qmi(qmi));
      }

   public:
      #pragma region Overrides
      virtual int           columnCount(const QModelIndex& parent = QModelIndex()) const override;
      virtual QVariant      data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
      virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
      virtual QVariant      headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
      virtual QModelIndex   index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
      virtual QModelIndex   parent(const QModelIndex& index) const override;
      virtual int           rowCount(const QModelIndex& parent = QModelIndex()) const override;
      #pragma endregion

      void refresh() {
         this->_reload_all();
      }
};

class ReachStringDependentModel : public QSortFilterProxyModel {
   protected:
      struct {
         bool all_same_language_only = false;
         bool allow_none             = false;
         std::vector<QString> blacklist;
      } _config;

      virtual bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
      virtual bool lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const override;

   public:
      ReachStringDependentModel(QObject* parent = nullptr);

      bool allowNone() const { return this->_config.allow_none; }
      void setAllowNone(bool v);

      bool requireAllLanguagesSame() const { return this->_config.all_same_language_only; }
      void setRequireAllLanguagesSame(bool v);

      void addBlacklistedString(const QString&);
      void clearBlacklistedStrings();
      const std::vector<QString>& blacklistedStrings() const { return this->_config.blacklist; }

      void setAlphabeticalSort() {
         this->setSortRole(Qt::DisplayRole);
      }
      void setNoSort() {
         // The advantage of doing this over setting the sort column to -1 (to disable sorting) 
         // is that this lets us force the "none" option to the top, for widgets that allow you 
         // to select no string.
         this->setSortRole(ReachStringCentralModel::StringIndexRole);
      }
};