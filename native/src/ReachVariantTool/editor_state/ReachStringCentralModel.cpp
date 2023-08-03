#include "./ReachStringCentralModel.h"
#include <QFont>
#include "../game_variants/types/multiplayer.h"
#include "../editor_state.h"

void ReachStringCentralModel::_stored_string::reload(const ReachString& src) {
   this->all_same_language = true;
   for (size_t j = 0; j < reach::language_count; ++j) {
      this->localizations[j] = QString::fromUtf8(src.languages()[j].c_str());

      if (this->all_same_language) {
         for (size_t k = 0; k < j; ++k) {
            if (this->localizations[j] != this->localizations[k]) {
               this->all_same_language = false;
               break;
            }
         }
      }
   }
}

ReachStringCentralModel::ReachStringCentralModel(QObject* parent) : QAbstractItemModel(parent) {
   auto& editor = ReachEditorState::get();
   QObject::connect(&editor, &ReachEditorState::variantAcquired, this, [this]() { this->_reload_all(); });
   QObject::connect(&editor, &ReachEditorState::stringModified, this, &ReachStringCentralModel::_reload_one);
   QObject::connect(&editor, &ReachEditorState::stringTableModified, this, &ReachStringCentralModel::_reload_all);
   QObject::connect(&editor, &ReachEditorState::variantAbandoned, this, [this]() { this->_clear(); });
}

const ReachStringCentralModel::_stored_string* ReachStringCentralModel::_entry_for_qmi(const QModelIndex& qmi) const {
   if (!qmi.isValid())
      return nullptr;
   int row = qmi.row();
   if (row < 0 || row >= this->_contents.size())
      return nullptr;
   return &this->_contents[row];
}

void ReachStringCentralModel::_clear() {
   this->beginResetModel();
   this->_contents.clear();
   this->endResetModel();
}
void ReachStringCentralModel::_reload_all() {
   this->beginResetModel();

   this->_contents.clear();

   const auto* mp = ReachEditorState::get().multiplayerData();
   if (!mp) {
      this->endResetModel();
      return;
   }

   const auto& table = mp->scriptData.strings;
   for (size_t i = 0; i < table.strings.size(); ++i) {
      const auto& string = table.strings[i];

      auto& entry = this->_contents.emplace_back();
      entry.reload(string);
   }

   this->endResetModel();
}
void ReachStringCentralModel::_reload_one(uint32_t index) {
   if (index > this->_contents.size()) {
      this->_reload_all();
      return;
   }

   const auto* mp = ReachEditorState::get().multiplayerData();
   if (!mp)
      return;
   const auto& table = mp->scriptData.strings;
   if (index >= table.strings.size())
      return;

   auto& entry = this->_contents[index];
   entry.reload(table.strings[index]);

   QModelIndex qmi = this->index(index, 0);
   emit this->dataChanged(qmi, qmi);
}

#pragma region Overrides
int ReachStringCentralModel::columnCount(const QModelIndex& parent) const {
   return 1;
}
QVariant ReachStringCentralModel::data(const QModelIndex& qmi, int role) const {
   if (!qmi.isValid())
      return {};
   auto* entry = this->_entry_for_qmi(qmi);
   if (!entry) {
      if (qmi.row() == this->_contents.size()) {
         switch (role) {
            case Qt::DisplayRole:
               return tr("<no name>", "none string");
            case Qt::FontRole:
               return QFont("FontFamily", -1, -1, true); // italics
            case StringIndexRole:
               return -1;
            case StringLocalizationsIdenticalRole:
               return true;
         }
      }
      return {};
   }
   if (role >= Qt::UserRole && role < Qt::UserRole + reach::language_count) {
      return entry->localizations[role - Qt::UserRole];
   }
   switch (role) {
      case Qt::DisplayRole:
      case Qt::ToolTipRole:
         return entry->localizations[0];
      case StringIndexRole:
         return qmi.row();
      case StringLocalizationsIdenticalRole:
         return entry->all_same_language;
   }
   return {};
}
Qt::ItemFlags ReachStringCentralModel::flags(const QModelIndex& qmi) const {
   if (!qmi.isValid())
      return {};
   return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
}
QVariant ReachStringCentralModel::headerData(int section, Qt::Orientation orientation, int role) const {
   if (section != 0)
      return {};
   if (orientation != Qt::Orientation::Horizontal)
      return {};
   if (role != Qt::DisplayRole)
      return {};
   return tr("String");
}
QModelIndex ReachStringCentralModel::index(int row, int column, const QModelIndex& parent) const {
   return this->createIndex(row, column, nullptr);
}
QModelIndex ReachStringCentralModel::parent(const QModelIndex& index) const {
   return {};
}
int ReachStringCentralModel::rowCount(const QModelIndex& parent) const {
   return this->_contents.size() + 1; // include a "none" row
}
#pragma endregion


#pragma region ReachStringDependentModel
ReachStringDependentModel::ReachStringDependentModel(QObject* parent) : QSortFilterProxyModel(parent) {
   this->setNoSort();
   this->sort(0);
}

void ReachStringDependentModel::setAllowNone(bool v) {
   auto& prop = this->_config.allow_none;
   if (prop == v)
      return;
   prop = v;
   this->invalidate();
}
void ReachStringDependentModel::setRequireAllLanguagesSame(bool v) {
   auto& prop = this->_config.all_same_language_only;
   if (prop == v)
      return;
   prop = v;
   this->invalidateFilter();
}

void ReachStringDependentModel::addBlacklistedString(const QString& v) {
   auto& prop = this->_config.blacklist;
   for (const auto& item : prop)
      if (item == v)
         return;
   prop.push_back(v);
   this->invalidateFilter();
}
void ReachStringDependentModel::clearBlacklistedStrings() {
   auto& prop = this->_config.blacklist;
   if (prop.empty())
      return;
   prop.clear();
   this->invalidateFilter();
}

bool ReachStringDependentModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const {
   if (!QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent))
      return false;

   auto* src_model = this->sourceModel();

   QModelIndex qmi = src_model->index(sourceRow, 0, sourceParent);

   if (!this->_config.allow_none) {
      const auto data = src_model->data(qmi, ReachStringCentralModel::StringIndexRole);
      if (data.toInt() < 0)
         return false;
   }
   bool all_languages_same = src_model->data(qmi, ReachStringCentralModel::StringLocalizationsIdenticalRole).toBool();
   if (this->_config.all_same_language_only) {
      if (all_languages_same == false)
         return false;
   }
   if (!this->_config.blacklist.empty()) {
      auto _allow_localization = [this](const QVariant& data) -> bool {
         if (data.userType() != QMetaType::QString)
            return true;
         const auto localization = data.toString();
         for (const auto& entry : this->_config.blacklist) {
            if (localization == entry)
               return false;
         }
         return true;
      };

      if (all_languages_same) {
         const auto data = src_model->data(qmi, ReachStringCentralModel::LocalizationEnglishRole);
         if (!_allow_localization(data))
            return false;
      } else {
         for (size_t i = 0; i < reach::language_count; ++i) {
            const auto data = src_model->data(qmi, Qt::UserRole + i);
            if (!_allow_localization(data))
               return false;
         }
      }
   }

   return true;
}
bool ReachStringDependentModel::lessThan(const QModelIndex& source_lhs, const QModelIndex& source_rhs) const {
   auto* src_model = this->sourceModel();

   if (this->_config.allow_none) {
      const auto data_lhs = src_model->data(source_lhs, ReachStringCentralModel::StringIndexRole);
      const auto data_rhs = src_model->data(source_rhs, ReachStringCentralModel::StringIndexRole);
      if (data_lhs.toInt() < 0)
         return true;
      if (data_rhs.toInt() < 0)
         return false;
   }

   return QSortFilterProxyModel::lessThan(source_lhs, source_rhs);
}
#pragma endregion