#pragma once
#include <QSyntaxHighlighter>
#include <QTextCharFormat>

//
// Syntax highlighter for Megalo. Based on a highlighter I wrote for Lua, so 
// there's some functionality I've left in (e.g. block comments and strings) 
// even though as of this writing, our Megalo compiler doesn't support it. 
// (It'd be nice to have!)
//
namespace ui::megalo_syntax_highlighting {
   class highlighter : public QSyntaxHighlighter {
      Q_OBJECT;
      public:
          highlighter(QObject* parent = nullptr);
          highlighter(QTextDocument* parent);

      protected:
         void _init();
         virtual void highlightBlock(const QString &text) override;

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
}