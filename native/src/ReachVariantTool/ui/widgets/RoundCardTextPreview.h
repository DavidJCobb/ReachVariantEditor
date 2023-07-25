#pragma once
#include <QBrush>
#include <QFont>
#include <QPen>
#include <QWidget>

class RoundCardTextPreview final : public QWidget {
   protected:
      struct {
         QBrush  box_fill;
         QPen    box_line;

         QPen text_title_pen;
         QPen text_body_pen;

         QString variant_name;
         QString objective;
         QString allegiance;
      } _fields;
      struct {
         QFont font_title;
         QFont font_body;
      } _state;

   public:
      RoundCardTextPreview(QWidget* parent = nullptr);

      virtual QSize sizeHint() const override;
      virtual bool hasHeightForWidth() const override { return true; }
      virtual int heightForWidth(int w) const override {
         auto s = sizeHint();
         return (float)s.height() / s.width();
      }

      enum class ColorPreset {
         FFA,
         RedTeam,
      };
      void setColorPreset(ColorPreset);

      QBrush boxFill() const;
      void setBoxFill(QBrush);

      QPen boxLine() const;
      void setBoxLine(QPen);

      QPen textBodyPen() const { return this->_fields.text_body_pen; }
      void setTextBodyPen(QPen v) { this->_fields.text_body_pen = v; }

      QPen textTitlePen() const { return this->_fields.text_title_pen; }
      void setTextTitlePen(QPen v) { this->_fields.text_title_pen = v; }

      QString variantName() const;
      void setVariantName(const QString&);

      QString objective() const;
      void setObjective(const QString&);

      QString allegiance();
      void setAllegiance(const QString&);
      
      virtual void paintEvent(QPaintEvent*) override;
};