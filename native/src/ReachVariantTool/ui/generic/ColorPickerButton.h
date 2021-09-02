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
#pragma once
#include <QPushButton>

class ColorPickerButton : public QPushButton {
   Q_OBJECT
   public:
      ColorPickerButton(QWidget* parent = nullptr);
      inline QColor color() const noexcept { return this->_color; }
      inline bool hasAlpha() const noexcept { return this->_hasAlpha; }
      void setColor(QColor);
      void setHasAlpha(bool);
      //
   signals:
      void colorChanged();
      //
   protected:
      bool   _hasAlpha = false;
      QColor _color;
      //
      void _updateColor();
};