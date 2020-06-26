#pragma once
#include <cstdint>
#include <string>
#include "helpers/xml.h"

void handle_base_tag(std::string& html, int depth);
void handle_page_title_tag(std::string& html, std::string title_to_display);
void minimize_indent(std::string& text);
size_t extract_html_from_xml(uint32_t token_index, cobb::xml::document& doc, std::string& out, std::string stem);