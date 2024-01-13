#include "ast.h"

#define SIMPLE_STRINGS_IMPL
#include "simple_strings.h"
#include <string.h>

const char HTML_HEADER[] = 	"<!DOCTYPE html>\n"
                            "<html>\n"
                            "  <head>\n"
                            "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
                            "    <meta charset=\"utf-8\">\n"
                            "    <title>Markdown to HTML</title>\n"
                            "    <link rel=\"stylesheet\" href=\"github-markdown.css\">\n"
                            "  </head>\n"
                            "  <body>\n"
                            "  <article class=\"markdown-body\">\n";

const char HTML_FOOTER[] = 	"  </article>\n"
                            "  </body>\n"
                            "</html>";

void add_indentation(string str, unsigned int indent_lvl) {
    for (int t = 0; t < indent_lvl; t++) {
        APPEND_ARR(str, "  ");
    }
}

string code_generation_from_dom(DOM* dom, unsigned int indent) {
    /*
    YOU HAVE TO COMPLETE THIS FUNCTION TO PRODUCE THE HTML EQUIVALENT FOR EACH DOM ELEMENT
    */
    if (dom == NULL) return STR("");

    switch (dom->dom_el) {
        case Document: {
            string html = STR("");
            add_indentation(html, indent);

            DomList *child = dom->children;

            while (child != NULL) {
                string child_html = code_generation_from_dom(child->dom, indent + 1);

                APPEND_STR(html, child_html);

                child = child->next;
            }

            return html;
        }
        case Header1: {
            string html = STR("");
            add_indentation(html, indent);

            APPEND_ARR(html, "<h1>");
            APPEND_ARR(html, dom->text);
            APPEND_ARR(html, "</h1>\n");

            return html;
        }
        case Header2: {
            string html = STR("");
            add_indentation(html, indent);

            APPEND_ARR(html, "<h2>");
            APPEND_ARR(html, dom->text);
            APPEND_ARR(html, "</h2>\n");

            return html;
        }
        case Header3: {
            string html = STR("");
            add_indentation(html, indent);

            APPEND_ARR(html, "<h3>");
            APPEND_ARR(html, dom->text);
            APPEND_ARR(html, "</h3>\n");

            return html;
        }
        case Header4: {
            string html = STR("");
            add_indentation(html, indent);

            APPEND_ARR(html, "<h4>");
            APPEND_ARR(html, dom->text);
            APPEND_ARR(html, "</h4>\n");

            return html;
        }
        case Header5: {
            string html = STR("");
            add_indentation(html, indent);

            APPEND_ARR(html, "<h5>");
            APPEND_ARR(html, dom->text);
            APPEND_ARR(html, "</h5>\n");

            return html;
        }
        case Header6: {
            string html = STR("");
            add_indentation(html, indent);

            APPEND_ARR(html, "<h6>");
            APPEND_ARR(html, dom->text);
            APPEND_ARR(html, "</h6>\n");

            return html;
        }
        case Quote: {
            string html = STR("");
            add_indentation(html, indent);

            APPEND_ARR(html, "<blockquote>");
            APPEND_ARR(html, dom->text);
            APPEND_ARR(html, "</blockquote>\n");

            return html;
        }
        case Paragraph: {
            string html = STR("");
            add_indentation(html, indent);

            APPEND_ARR(html, "<p>\n");

            DomList *child = dom->children;
            DOM *previous_child = NULL;

            while (child != NULL) {
                // We check here if we have two TextElement besides, it should have a space between.
                if (previous_child != NULL) {
                    if (previous_child->dom_el == TextElement) {
                        APPEND_ARR(html, " ");
                    }
                } else {
                    add_indentation(html, indent + 1);
                }

                string content = code_generation_from_dom(child->dom, indent + 1); // Indentation not relevant here

                APPEND_STR(html, content);

                previous_child = child->dom;
                child = child->next;
            }

            APPEND_ARR(html, "\n");
            add_indentation(html, indent);
            APPEND_ARR(html, "</p>\n");

            return html;
        }
        case TextElement: {
            string html = STR(dom->text);

            return html;
        }
        case HRule: {
            string html = STR("<hr>");

            return html;
        }
        case Bold: {
            string html = STR("<b>");

            if (dom->children != NULL) {
                string content = code_generation_from_dom(dom->children->dom, indent + 1); // Identation not relevant here

                APPEND_STR(html, content);
            }

            APPEND_ARR(html, "</b>");

            return html;
        }
        case Italic: {
            string html = STR("<i>");

            if (dom->children != NULL) {
                string content = code_generation_from_dom(dom->children->dom, indent + 1); // Identation not relevant here

                APPEND_STR(html, content);
            }

            APPEND_ARR(html, "</i>");

            return html;
        }
        case Underline: {
            string html = STR("<u>");

            if (dom->children != NULL) {
                string content = code_generation_from_dom(dom->children->dom, indent + 1); // Identation not relevant here

                APPEND_STR(html, content);
            }

            APPEND_ARR(html, "</u>");

            return html;
        }
        case Strikethrough: {
            string html = STR("<s>");

            if (dom->children != NULL) {
                string content = code_generation_from_dom(dom->children->dom, indent + 1); // Identation not relevant here

                APPEND_STR(html, content);
            }

            APPEND_ARR(html, "</s>");

            return html;
        }
        default: {
            return STR("");
        }
    }
}

string code_generation(DOM* dom) {
    string final_code = STR(HTML_HEADER);
    string dom_code = code_generation_from_dom(dom, 2);

    APPEND_STR(final_code, dom_code);
    APPEND_ARR(final_code, HTML_FOOTER);

    spls_free_string(dom_code);

    return final_code;
}
