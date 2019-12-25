#pragma once
#include <QValidator>

//
// Made specifically for Halo: Reach, which stores gamertags into user-created content as 
// 16-character ASCII strings
//

class QXBLGamertagValidator : public QValidator {
   public:
      QXBLGamertagValidator(bool l = true, QObject* parent = nullptr) : QValidator(parent), lax(l) {}
      bool lax = true;
      //
      virtual QValidator::State validate(QString& input, int& pos) const {
         if (lax) {
            for (auto c : input) {
               if (c > 255)
                  return QValidator::Invalid;
            }
            return QValidator::Acceptable;
         }
         for (auto c : input) {
            if (c >= QChar('A') && c <= QChar('Z'))
               continue;
            if (c == QChar(' '))
               continue;
            if (c >= QChar('0') && c <= QChar('9'))
               continue;
            return QValidator::Invalid;
         }
         return QValidator::Acceptable;
      }

      static QXBLGamertagValidator* getReachInstance() {
         static QXBLGamertagValidator instance(true);
         return &instance;
      }
};