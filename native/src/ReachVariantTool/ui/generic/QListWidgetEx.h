#pragma once
#include <functional>
#include <QListWidget>

class QListWidgetEx : public QListWidget {
   Q_OBJECT
   public:
      using copy_transform_functor_t = std::function<void(QString&, QListWidgetItem*)>;
      //
   public:
      QListWidgetEx(QWidget* parent = nullptr) : QListWidget(parent) {}
      //
      void copyAllToClipboard();
      void setCopyTransformFunctor(copy_transform_functor_t);
      //
   protected:
      virtual void keyPressEvent(QKeyEvent* event) override;
      //
      copy_transform_functor_t _copyTransformFunctor = nullptr;
};
