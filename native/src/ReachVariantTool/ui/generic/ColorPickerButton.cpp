/*

This file is provided under the Creative Commons 0 License.
License: <https://creativecommons.org/publicdomain/zero/1.0/legalcode>
Summary: <https://creativecommons.org/publicdomain/zero/1.0/>

One-line summary: This file is public domain or the closest legal equivalent.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
#include "ColorPickerButton.h"
#include <QColorDialog>

namespace {
   const QString _style("QPushButton { background-color: %1; color: %2; }");

   int _perceived_brightness(const QColor& color) {
      return sqrt(
         (color.red()   * color.red()   * 0.241F)
       + (color.green() * color.green() * 0.691F)
       + (color.blue()  * color.blue()  * 0.068F)
      );
   }
}
ColorPickerButton::ColorPickerButton(QWidget* parent) : QPushButton(parent) {
   QObject::connect(this, &ColorPickerButton::colorChanged, this, &ColorPickerButton::_updateColor);
   QObject::connect(this, &QPushButton::clicked, [this]() {
      auto color = QColorDialog::getColor(this->_color, this->window(), QString(), (QColorDialog::ColorDialogOptions)(this->hasAlpha() ? QColorDialog::ShowAlphaChannel : 0));
      if (!color.isValid())
         return;
      this->_color = color;
      this->_updateColor();
      emit this->colorChanged();
   });
}
void ColorPickerButton::setColor(QColor c) {
   this->_color = c;
   emit this->colorChanged();
}
void ColorPickerButton::setHasAlpha(bool s) {
   this->_hasAlpha = s;
}
void ColorPickerButton::_updateColor() {
   constexpr int text_cutoff = 125;
   //
   QColor text = QColor{ 0, 0, 0, 255 };
   if (_perceived_brightness(this->_color) < text_cutoff)
      text = QColor{ 255, 255, 255, 255 };
   //
   auto code = this->_color.name();
   this->setStyleSheet(_style.arg(code).arg(text.name()));
   this->setText(code);
}