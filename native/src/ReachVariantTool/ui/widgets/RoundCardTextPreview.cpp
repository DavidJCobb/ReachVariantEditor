#include "./RoundCardTextPreview.h"
#include <QDebug>
#include <QPainter>
#include <QPainterPath>
#include "../../editor_state.h"

namespace {
   // The game truncates strings that are too long. Including the null terminator, they used 
   // a 117-character buffer. Cute.
   constexpr const size_t max_string_length = 116;

   // Pixel sizes on 1920x1080px monitor:
   constexpr const size_t objective_box_w = 580;
   constexpr const size_t objective_box_h = 162;
   //
   constexpr const size_t objective_text_offset_x = 153;
   constexpr const size_t objective_text_offset_baseline_y =  87.5;
   //
   constexpr const size_t variant_name_text_offset_x = objective_text_offset_x;
   constexpr const size_t variant_name_text_offset_baseline_y = 56;

   constexpr const size_t allegiance_box_w = 161;
   constexpr const size_t allegiance_box_h = objective_box_h;
   //
   constexpr const size_t allegiance_text_offset_baseline_y = 146.5; // no x-offset; this text is centered

   constexpr const size_t box_corner_w = 20;
   constexpr const size_t box_corner_h = 19;

   constexpr const size_t box_gap = 12; // distance between boxes

   //
   // Qt offers no way to query this fron the actual loaded font (and its font APIs 
   // are aggressively terrible in general), so I had to stuff the font into multiple 
   // online analysis sites and get these metrics myself:
   //
   constexpr const int font_units_per_em = 2048;
   constexpr const int font_ascent       = 1767;
   constexpr const int font_descent      = -432;
   constexpr const int font_height       = font_ascent - font_descent;
   constexpr const int font_cap_height   = 1366; // measured 'D' and 'N', which don't dip below baseline

   void shift_qrect_from_baseline_to_text_top(QRect& rect, const QFont& font) {
      auto pixel_size = font.pixelSize();

      auto ascent = pixel_size * ((float)font_ascent / (float)font_units_per_em);
      rect.translate(0, -ascent);
   }
}

RoundCardTextPreview::RoundCardTextPreview(QWidget* parent) : QWidget(parent) {
   QSizePolicy policy(QSizePolicy::Preferred, QSizePolicy::Fixed);
   policy.setHeightForWidth(true);
   this->setSizePolicy(policy);

   // Default text:
   this->setVariantName("Game Variant Name Here");

   //
   // Default pens and brushes:
   //
   this->setColorPreset(ColorPreset::FFA);

   QFont base;
   base.setStyleStrategy((QFont::StyleStrategy)(QFont::NoFontMerging | QFont::NoSubpixelAntialias));
   auto  font_family = ReachEditorState::get().getReachEditingKitBodyTextFontFamily();
   if (font_family.has_value()) {
      base = QFont(font_family.value());
      base.setPixelSize(34);
      this->_state.font_title = base;
      //
      // HACKS:
      // 
      //  - Qt doesn't allow us to use sub-pixel precision for font sizes.
      // 
      //  - There doesn't appear to be any way to view the actual round card UI and its definition 
      //    using the Halo: Reach Editing Kit. I can't find any CHUD/CUI for it. Multiple tags refer 
      //    to `summary_editor` but that is very blatantly NOT the right UI: it doesn't have the box 
      //    for the player allegiance, the text is centered rather than left-aligned, the icon is 
      //    positioned all wrong, multiple non-Megalo flags are pulled from game state...
      // 
      //  - Therefore, there's no way to know the true intended font size, nor what font settings 
      //    Bungie may have used (e.g. stretch/condense).
      // 
      //  - Additionally, the game just straight-up renders fonts its own way. Even if you get the 
      //    EM height exactly right, some letters may be rendered wider, for example.
      // 
      // OBSERVATIONS:
      // 
      //  - 28px is almost perfect for height, but the width is considerably too small.
      // 
      //  - 29px is almost perfect for height, but the width is slightly too high. It varies based 
      //    on what symbols appear in a line; it's not off by a consistent proportion.
      // 
      //    Consider this text:
      // 
      //       Activate your active camo for different
      //       lengths of time to use different functions.
      // 
      //    The first line is 3.47% too long when rendered at 29px in Qt; the second, 2.23%, 
      //    despite having more characters in it. Accuracy, then, varies based on which characters 
      //    are actually present.
      // 
      // CONCLUSION:
      // 
      //  - Condense text to match the game somewhat better, but pessimize: condense by 2%.
      //
      base.setPixelSize(29);
      base.setStretch(98);
      this->_state.font_body = base;
      //
      // FURTHER OBSERVATIONS:
      // 
      //  - "MMMMMMMMMM" is 173px in-game on a 1920x1080px monitor, and 166px with a 98% stretch.
      //  - "IIIIIIIIII" is  56px in-game on a 1920x1080px monitor, and  56px with a 98% stretch.
      //  - "kkkkkkkkkk" is 104px in-game on a 1920x1080px monitor. and  98px with a 98% stretch.
      // 
      // Actually... What if it's ClearType?...
      //
   }
}

QSize RoundCardTextPreview::sizeHint() const {
   size_t inner_w = objective_box_w;
   size_t inner_h = objective_box_h;

   if (!this->_fields.allegiance.isEmpty()) {
      inner_w += allegiance_box_w + box_gap;
      if constexpr (allegiance_box_h > objective_box_h) {
         inner_h = allegiance_box_h;
      }
   }

   return QSize(inner_w, inner_h);
}

void RoundCardTextPreview::setColorPreset(ColorPreset preset) {
   this->_fields.box_line.setWidth(2); // actually 2 for horizontal borders and 3 for vertical, but we don't need pixel-perfect results
   this->_fields.text_title_pen.setColor(QColor::fromRgb(239, 239, 239));
   this->_fields.text_body_pen.setColor(QColor::fromRgb(224, 224, 224, 224)); // blind guess; seems like a consistent but semitransparent color

   switch (preset) {
      case ColorPreset::FFA:
         this->_fields.box_line.setColor(QColor::fromRgb(80, 178, 239));
         this->_fields.box_fill.setColor(QColor::fromRgb(24,  42,  75, 160));
         break;
      case ColorPreset::RedTeam:
         this->_fields.box_line.setColor(QColor::fromRgb(135, 135, 135));
         this->_fields.box_fill.setColor(QColor::fromRgb(135,   0,   0, 160));
         break;
   }

   // override qt's insensible default "no style; this brush does nothing" value:
   this->_fields.box_fill.setStyle(Qt::BrushStyle::SolidPattern);
}

QBrush RoundCardTextPreview::boxFill() const {
   return this->_fields.box_fill;
}
void RoundCardTextPreview::setBoxFill(QBrush v) {
   this->_fields.box_fill = v;
}

QPen RoundCardTextPreview::boxLine() const {
   return this->_fields.box_line;
}
void RoundCardTextPreview::setBoxLine(QPen v) {
   this->_fields.box_line = v;
}

QString RoundCardTextPreview::variantName() const {
   return this->_fields.variant_name;
}
void RoundCardTextPreview::setVariantName(const QString& v) {
   this->_fields.variant_name = v; // TODO: is this capped to 116 chars, too? or 128, since variant names in general are capped there?
}

QString RoundCardTextPreview::objective() const {
   return this->_fields.objective;
}
void RoundCardTextPreview::setObjective(const QString& v) {
   this->_fields.objective = v.left(max_string_length);
}

QString RoundCardTextPreview::allegiance() {
   return this->_fields.allegiance;
}
void RoundCardTextPreview::setAllegiance(const QString& v) {
   this->_fields.allegiance = v.left(max_string_length); // TODO: see if this has a smaller maxlength
}

void RoundCardTextPreview::paintEvent(QPaintEvent* event) {
   QPainter painter(this);
   const auto rect      = this->rect();
   const auto base_size = this->sizeHint();

   float scale = (float)rect.width() / (float)base_size.width();
   painter.scale(scale, scale);

   painter.setPen(this->boxLine());

   QPainterPath path_main;
   path_main.addRoundedRect(
      QRectF(0, 0, objective_box_w, objective_box_h),
      box_corner_w,
      box_corner_h
   );

   // Why doesn't any drawing API ever actually let us control the stroke alignment?
   auto draw_with_inner_stroke = [&painter](const QPainterPath& path, QPen pen, QBrush brush) {
      pen.setWidth(pen.width() * 2);
      painter.setPen(pen);

      auto prior     = painter.clipPath();
      bool use_prior = painter.hasClipping();
      painter.setClipPath(path);
      painter.fillPath(path, brush);
      painter.drawPath(path);

      painter.setClipPath(prior);
      painter.setClipping(use_prior);
   };
   
   if (!this->_fields.allegiance.isEmpty()) {
      QPainterPath path_allegiance;
      path_allegiance.addRoundedRect(
         QRectF(0, 0, allegiance_box_w, allegiance_box_h),
         box_corner_w,
         box_corner_h
      );
      draw_with_inner_stroke(path_allegiance, this->_fields.box_line, this->_fields.box_fill);

      // Qt positions the "top of the text" at the top of the rect. Stack Overflow 
      // answers suggest that they're referring to the top of the line box or the 
      // top of the ascent region.
      QRect text_rect{ 0, 0, allegiance_box_w, allegiance_box_h };
      text_rect.translate(0, allegiance_text_offset_baseline_y);
      shift_qrect_from_baseline_to_text_top(text_rect, this->_state.font_body);
      painter.setPen(this->textTitlePen());
      painter.setFont(this->_state.font_body);
      painter.drawText(text_rect, Qt::AlignHCenter, this->_fields.allegiance);

      painter.translate(allegiance_box_w + box_gap, 0);
      painter.setPen(this->boxLine());
   }

   draw_with_inner_stroke(path_main, this->_fields.box_line, this->_fields.box_fill);
   
   QRect variant_name_text_rect = rect.translated(variant_name_text_offset_x, variant_name_text_offset_baseline_y);
   shift_qrect_from_baseline_to_text_top(variant_name_text_rect, this->_state.font_title);
   //
   painter.setFont(this->_state.font_title);
   painter.setPen(this->textTitlePen());
   painter.drawText(variant_name_text_rect, Qt::AlignLeft, this->_fields.variant_name);

   QRect objective_text_rect = rect.translated(objective_text_offset_x, objective_text_offset_baseline_y);
   shift_qrect_from_baseline_to_text_top(objective_text_rect, this->_state.font_body);
   //
   painter.setFont(this->_state.font_body);
   painter.setPen(this->textBodyPen());
   painter.drawText(objective_text_rect, Qt::AlignLeft, this->_fields.objective);
}