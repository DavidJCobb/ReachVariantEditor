#pragma once
#include <functional>
#include <QtXml>
#include <QString>

namespace util {
   namespace impl {
      struct megalo_keyword {
         using phrase_t = QVector<QString>;

         QString initial;
         QVector<phrase_t> phrases; // should not include initial word

         // If a phrase contains a word consisting only of "*", then that's a 
         // wildcard and can be matched by any word.
         int longest_phrase_matching(const phrase_t& words) const noexcept;
      };
   }

   //
   // Stateful syntax highlighter. You could e.g. scan over a node tree and pass 
   // each text node into this.
   //
   class megalo_syntax_highlighter {
      public:
         struct parse_dom_options {
            bool adjust_indent              = true;
            bool include_containing_element = false;
            std::function<void(QString&)> url_tweak;
            //
            bool _is_recursing   = false;
            int  _minimum_indent = -1;
         };
      public:
         struct {
            struct {
               impl::megalo_keyword* main = nullptr;
               int phrase     = -1;
               int subkeyword = -1; // number of words completed (not started) for the current phrase
            } keyword;
            bool  in_comment    = false;
            bool  in_xml_entity = false;
            QChar in_string     = '\0';
            uint  indent_count  = 0;
            bool  past_indent   = false; // have we moved past indentation on the current line?
            //
            QChar previous_char = '\0'; // only updated at the end of parsing one string; only meaningful at the start of parsing the next
         } state;
         struct {
            bool escape_xml_entities = false; // should be (true) if you're passing in text that is not yet XML-escaped
            bool skip_xml_entities   = true;  // should be (true) if you're passing in text that is already XML-escaped
            uint reduce_indent_by    = 0;
         } options;

      protected:
         void _exit_keyword();

         static QString _enter_markup(const QString& type);
         static QString _exit_markup(const QString& type);

         QString _escape_character(QChar c) const noexcept;

      public:
         QString parse_more(const QString& fragment, bool is_end = false);
         QString end();

         QString before_markup_break() const noexcept;
         QString after_markup_break() const noexcept;

         QString parse_dom(QDomElement root, parse_dom_options);
   };

   // Receives plain text, with characters escaped as XML entities already, and 
   // returns XML with SPAN tags for different syntax components.
   extern QString megalo_syntax_highlight(const QString& script);
}