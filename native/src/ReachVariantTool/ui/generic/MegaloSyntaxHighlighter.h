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
#include <QSyntaxHighlighter>
#include <QTextCharFormat>

//
// Syntax highlighter for Megalo. Based on a highlighter I wrote for Lua, so 
// there's some functionality I've left in (e.g. block comments and strings) 
// even though as of this writing, our Megalo compiler doesn't support it. 
// (It'd be nice to have!)
//
class MegaloSyntaxHighlighter : public QSyntaxHighlighter {
    Q_OBJECT
   public:
      struct Keyword {
         QString  word;
         bool     is_subkeyword = false;
         bool     requires_next = false; // may not actually be usable in QSyntaxHighlighter design
         QVector<Keyword*> possible;

         Keyword* make_phrase(const QVector<QString>&);

         Keyword* make_subkeyword(const QString&, bool requires_next = false);
      };

   public:
       MegaloSyntaxHighlighter(QTextDocument* parent = nullptr);

       enum class TokenType {
          None,
          Comment_Line,
          Comment_Block,
          String_Simple,
          String_Block,
       };

       // Simple block state, capable ofb eing stored in a single int.
       class BlockState {
          protected:
             int value = 0;

             static constexpr const int code_bitcount = 8;
             static constexpr const int code_mask     = (1 << code_bitcount) - 1;
             static_assert(code_bitcount < sizeof(int) * 8);

          public:
            BlockState() {}
            BlockState(int i) : value(i) {}
            BlockState(TokenType t, int param = 0) : value((int)t | (param << code_bitcount)) {}

            static constexpr const int param_bits          = sizeof(int) * 8 - code_bitcount;
            static constexpr const int max_supported_param = (1 << param_bits) - 1;

            inline TokenType code() const noexcept { return (TokenType)(this->value & code_mask); }
            inline int param() const noexcept { return this->value >> code_bitcount; }

            inline bool is_none() const noexcept { return (this->value == -1) || code() == TokenType::None; }

            inline void clear() noexcept {
               this->value = 0;
            }

            inline int to_int() const noexcept { return this->value; }
       };

       class ComplexBlockState : public QTextBlockUserData {
          public:
            const Keyword* keyword = nullptr;
       };

   protected:
       void highlightBlock(const QString &text) override;

   public:
      //
      // Configuration values. After changing any of these, you must call rehighlight() 
      // on this syntax highlighter so that your changes take effect on any already-
      // highlighted text.
      //
      struct {
         bool block_comments = true;
         bool block_strings  = false; // Megalo compiler doesn't support these
      } features;
      struct {
         struct {
            QTextCharFormat line;
            QTextCharFormat block;
         } comment;
         QTextCharFormat keyword;
         QTextCharFormat subkeyword;
         QTextCharFormat number;
         QTextCharFormat op;
         struct {
            QTextCharFormat simple;
            QTextCharFormat block;
         } string;
      } formats;

      void reloadFormattingFromINI();
};