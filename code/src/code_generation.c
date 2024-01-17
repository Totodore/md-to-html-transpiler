#include "ast.h"

#define SIMPLE_STRINGS_IMPL
#include "simple_strings.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

const char HTML_HEADER[] = "<!DOCTYPE html>\n"
						   "<html>\n"
						   "  <head>\n"
						   "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
						   "    <meta charset=\"utf-8\">\n"
						   "    <title>Markdown to HTML</title>\n"
						   "    <link rel=\"stylesheet\" href=\"github-markdown.css\">\n"
						   "  </head>\n"
						   "  <body>\n"
						   "  <article class=\"markdown-body\">\n";

const char HTML_FOOTER[] = "  </article>\n"
						   "  </body>\n"
						   "</html>";

void add_indentation(string str, unsigned int indent_lvl)
{
	for (int t = 0; t < indent_lvl; t++)
	{
		APPEND_ARR(str, "  ");
	}
}

/**
 * @brief Append an integer to a string buffer.
 */
void append_int(string buffer, int value)
{
	int len = value != 0 ? log10(abs(value)) + 1 : 1;
	char *buf = malloc(len + 1);
	sprintf(buf, "%d", value);
	APPEND_ARR(buffer, buf);
	free(buf);
}

/**
 * @brief Append a coordinate to a buffer separated by a delimiter.
 */
void append_coord(string buffer, SvgCoord *coord, const char *delim)
{
	append_int(buffer, coord->x);
	APPEND_ARR(buffer, delim);
	append_int(buffer, coord->y);
}
/**
 * @brief Append a list of coordinates to a buffer separated by a delimiter.
 */
void append_coord_list(string buffer, SvgCoordList *coord_list, const char *delim, const char *coord_delim)
{
	while (coord_list != NULL)
	{
		append_coord(buffer, coord_list->coord, coord_delim);

		coord_list = coord_list->next;
		if (coord_list != NULL)
			APPEND_ARR(buffer, delim);
	}
}
/**
 * @brief Append a property in the form ` prop="value"` to a buffer.
 */
void append_prop(string buffer, const char *prop, char *value)
{
	APPEND_ARR(buffer, " ");
	APPEND_ARR(buffer, prop);
	APPEND_ARR(buffer, "=\"");
	APPEND_ARR(buffer, value);
	APPEND_ARR(buffer, "\"");
}

/**
 * @brief Append a property in the form ` prop="value"` to a buffer.
 */
void append_int_prop(string buffer, const char *prop, int value)
{
	APPEND_ARR(buffer, " ");
	APPEND_ARR(buffer, prop);
	APPEND_ARR(buffer, "=\"");
	append_int(buffer, value);
	APPEND_ARR(buffer, "\"");
}
void append_svg_element(string buffer, SvgInst *element)
{
	switch (element->kind)
	{
	case Line:
	{
		APPEND_ARR(buffer, "<line");
		SvgCoord *coord = element->coords->coord;

		append_int_prop(buffer, "x1", coord->x);
		append_int_prop(buffer, "y1", coord->y);

		coord = element->coords->next->coord;
		append_int_prop(buffer, "x2", coord->x);
		append_int_prop(buffer, "y2", coord->y);

		if (element->color_fill != NULL)
			append_prop(buffer, "stroke", element->color_fill);

		APPEND_ARR(buffer, "/>");
		break;
	}
	case Polyline:
	{
		APPEND_ARR(buffer, "<polyline points=\"");

		append_coord_list(buffer, element->coords, " ", ",");
		APPEND_ARR(buffer, "\"");

		if (element->color_fill != NULL)
			append_prop(buffer, "stroke", element->color_fill);

		append_prop(buffer, "fill", "none");

		APPEND_ARR(buffer, "/>");
		break;
	}
	case Polygon:
	{
		APPEND_ARR(buffer, "<polygon points=\"");
		append_coord_list(buffer, element->coords, " ", ",");
		APPEND_ARR(buffer, "\"");

		if (element->color_stroke != NULL)
			append_prop(buffer, "stroke", element->color_stroke);

		if (element->color_fill != NULL)
			append_prop(buffer, "fill", element->color_fill);

		APPEND_ARR(buffer, "/>");
		break;
	}
	case Circle:
	{
		APPEND_ARR(buffer, "<circle");
		SvgCoord *coord = element->coords->coord;
		append_int_prop(buffer, "cx", coord->x);
		append_int_prop(buffer, "cy", coord->y);
		append_int_prop(buffer, "r", element->width);

		if (element->color_stroke != NULL)
			append_prop(buffer, "stroke", element->color_stroke);

		if (element->color_fill != NULL)
			append_prop(buffer, "fill", element->color_fill);

		APPEND_ARR(buffer, "/>");
		break;
	}
	case Ellipse:
	{
		APPEND_ARR(buffer, "<ellipse");
		SvgCoord *coord = element->coords->coord;
		append_int_prop(buffer, "cx", coord->x);
		append_int_prop(buffer, "cy", coord->y);
		append_int_prop(buffer, "rx", element->width);
		append_int_prop(buffer, "ry", element->height);

		if (element->color_stroke != NULL)
			append_prop(buffer, "stroke", element->color_stroke);

		if (element->color_fill != NULL)
			append_prop(buffer, "fill", element->color_fill);

		APPEND_ARR(buffer, "/>");
		break;
	}
	case Rect:
	{
		APPEND_ARR(buffer, "<rect");
		SvgCoord *coord = element->coords->coord;
		append_int_prop(buffer, "x", coord->x);
		append_int_prop(buffer, "y", coord->y);
		append_int_prop(buffer, "width", element->width);
		append_int_prop(buffer, "height", element->height);

		APPEND_ARR(buffer, "\"");

		if (element->color_stroke != NULL)
			append_prop(buffer, "stroke", element->color_stroke);

		if (element->color_fill != NULL)
			append_prop(buffer, "fill", element->color_stroke);

		APPEND_ARR(buffer, "/>");
		break;
	}
	case Text:
	{
		APPEND_ARR(buffer, "<text");
		SvgCoord *coord = element->coords->coord;
		append_int_prop(buffer, "x", coord->x);
		append_int_prop(buffer, "y", coord->y);

		if (element->anchor != NULL)
			append_prop(buffer, "text-anchor", element->anchor);

		if (element->color_stroke != NULL)
			append_prop(buffer, "stroke", element->color_stroke);

		if (element->color_fill != NULL)
			append_prop(buffer, "fill", element->color_fill);

		APPEND_ARR(buffer, ">");
		APPEND_ARR(buffer, element->text);
		APPEND_ARR(buffer, "</text>");
		break;
	}
	}

	APPEND_ARR(buffer, "\n");
}

string code_generation_from_dom(DOM *dom, unsigned int indent)
{

	if (dom == NULL)
		return STR("");

	switch (dom->dom_el)
	{
	case Document:
	{
		string html = STR("");
		add_indentation(html, indent);

		DomList *child = dom->children;

		while (child != NULL)
		{
			string child_html = code_generation_from_dom(child->dom, indent + 1);

			APPEND_STR(html, child_html);

			child = child->next;
		}

		return html;
	}
	case Header1:
	{
		string html = STR("");
		add_indentation(html, indent);

		APPEND_ARR(html, "<h1>");
		APPEND_ARR(html, dom->text);
		APPEND_ARR(html, "</h1>\n");

		return html;
	}
	case Header2:
	{
		string html = STR("");
		add_indentation(html, indent);

		APPEND_ARR(html, "<h2>");
		APPEND_ARR(html, dom->text);
		APPEND_ARR(html, "</h2>\n");

		return html;
	}
	case Header3:
	{
		string html = STR("");
		add_indentation(html, indent);

		APPEND_ARR(html, "<h3>");
		APPEND_ARR(html, dom->text);
		APPEND_ARR(html, "</h3>\n");

		return html;
	}
	case Header4:
	{
		string html = STR("");
		add_indentation(html, indent);

		APPEND_ARR(html, "<h4>");
		APPEND_ARR(html, dom->text);
		APPEND_ARR(html, "</h4>\n");

		return html;
	}
	case Header5:
	{
		string html = STR("");
		add_indentation(html, indent);

		APPEND_ARR(html, "<h5>");
		APPEND_ARR(html, dom->text);
		APPEND_ARR(html, "</h5>\n");

		return html;
	}
	case Header6:
	{
		string html = STR("");
		add_indentation(html, indent);

		APPEND_ARR(html, "<h6>");
		APPEND_ARR(html, dom->text);
		APPEND_ARR(html, "</h6>\n");

		return html;
	}
	case Quote:
	{
		string html = STR("");
		add_indentation(html, indent);

		APPEND_ARR(html, "<blockquote>");
		APPEND_ARR(html, dom->text);
		APPEND_ARR(html, "</blockquote>\n");

		return html;
	}
	case Paragraph:
	{
		string html = STR("");
		add_indentation(html, indent);

		APPEND_ARR(html, "<p>\n");

		DomList *child = dom->children;
		DOM *previous_child = NULL;

		while (child != NULL)
		{
			// We check here if we have two TextElement besides, it should have a space between.
			if (previous_child != NULL)
			{
				if (previous_child->dom_el == TextElement)
				{
					APPEND_ARR(html, " ");
				}
			}
			else
			{
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
	case TextElement:
	{
		string html = STR(dom->text);

		return html;
	}
	case HRule:
	{
		string html = STR("<hr>");

		return html;
	}
	case Bold:
	{
		string html = STR("<b>");

		if (dom->children != NULL)
		{
			string content = code_generation_from_dom(dom->children->dom, indent + 1); // Identation not relevant here

			APPEND_STR(html, content);
		}

		APPEND_ARR(html, "</b>");

		return html;
	}
	case Italic:
	{
		string html = STR("<i>");

		if (dom->children != NULL)
		{
			string content = code_generation_from_dom(dom->children->dom, indent + 1); // Identation not relevant here

			APPEND_STR(html, content);
		}

		APPEND_ARR(html, "</i>");

		return html;
	}
	case Underline:
	{
		string html = STR("<u>");

		if (dom->children != NULL)
		{
			string content = code_generation_from_dom(dom->children->dom, indent + 1); // Identation not relevant here

			APPEND_STR(html, content);
		}

		APPEND_ARR(html, "</u>");

		return html;
	}
	case Strikethrough:
	{
		string html = STR("<s>");

		if (dom->children != NULL)
		{
			string content = code_generation_from_dom(dom->children->dom, indent + 1); // Identation not relevant here

			APPEND_STR(html, content);
		}

		APPEND_ARR(html, "</s>");

		return html;
	}
	case InlineCode:
	{
		string html = STR("");
		add_indentation(html, indent);

		APPEND_ARR(html, "<code>");
		APPEND_ARR(html, dom->text);
		APPEND_ARR(html, "</code>\n");

		return html;
	}
	case BlockCode:
	{
		string html = STR("");
		add_indentation(html, indent);

		APPEND_ARR(html, "<pre>");
		APPEND_ARR(html, dom->text);
		APPEND_ARR(html, "</pre>\n");

		return html;
	}
	case Link:
	{
		string html = STR("");
		add_indentation(html, indent);

		APPEND_ARR(html, "<a href=\"");
		APPEND_ARR(html, dom->url);
		APPEND_ARR(html, "\">");
		APPEND_ARR(html, dom->text);
		APPEND_ARR(html, "</a>\n");

		return html;
	}
	case Image:
	{
		string html = STR("");
		add_indentation(html, indent);

		APPEND_ARR(html, "<img src=\"");
		APPEND_ARR(html, dom->url);
		APPEND_ARR(html, "\" alt=\"");
		APPEND_ARR(html, dom->text);
		APPEND_ARR(html, "\">\n");

		return html;
	}
	case SVG:
	{
		string html = STR("");
		if (dom->svg_coords == NULL || dom->svg_coords->next == NULL)
			return html;

		add_indentation(html, indent);
		APPEND_ARR(html, "<svg viewBox=\"");
		// Add viewbox coordinates
		append_coord_list(html, dom->svg_coords, " ", " ");

		APPEND_ARR(html, "\">\n");

		SvgList *child = dom->svg_children;
		while (child != NULL)
		{
			add_indentation(html, indent + 1);
			append_svg_element(html, child->svg);
			child = child->next;
		}

		add_indentation(html, indent);
		APPEND_ARR(html, "</svg>\n");

		return html;
	}
	default:
	{
		return STR("");
	}
	}
}

string code_generation(DOM *dom)
{
	string final_code = STR(HTML_HEADER);
	string dom_code = code_generation_from_dom(dom, 2);

	APPEND_STR(final_code, dom_code);
	APPEND_ARR(final_code, HTML_FOOTER);

	spls_free_string(dom_code);

	return final_code;
}
