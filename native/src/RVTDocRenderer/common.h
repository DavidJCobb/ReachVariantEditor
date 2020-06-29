#pragma once
#include <cstdint>
#include <string>
#include "helpers/xml.h"

void ensure_paragraph(std::string&);
void handle_base_tag(std::string& html, int depth); // change the HREF of a BASE tag to a string of the format "../../" using the given depth
void handle_page_title_tag(std::string& html, std::string title_to_display); // change the content of a TITLE tag
void minimize_indent(std::string& text);
void syntax_highlight_in_html(const std::string& in, std::string& out);
void syntax_highlight_in_html(std::string& out);
size_t extract_html_from_xml(uint32_t token_index, cobb::xml::document& doc, std::string& out, std::string stem);