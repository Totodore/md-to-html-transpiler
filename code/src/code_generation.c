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

string add_indentation(string str, unsigned int indent_lvl)
{
	for (int t = 0; t < indent_lvl; t++)
	{
		APPEND_ARR(str, "  ");
	}
	return str;
}
#define ADD_INDENTATION(str, indent_lvl) str = add_indentation(str, indent_lvl)

/**
 * @brief Append an integer to a string buffer.
 */
string append_int(string buffer, int value)
{
	int len = value != 0 ? log10(abs(value)) + 1 : 1;
	char *buf = malloc(len + 1);
	sprintf(buf, "%d", value);
	APPEND_ARR(buffer, buf);
	return buffer;
}
#define APPEND_INT(buffer, value) buffer = append_int(buffer, value)

/**
 * @brief Append a coordinate to a buffer separated by a delimiter.
 */
string append_coord(string buffer, SvgCoord *coord, const char *delim)
{
	APPEND_INT(buffer, coord->x);
	APPEND_ARR(buffer, delim);
	APPEND_INT(buffer, coord->y);
}
#define APPEND_COORD(buffer, coord, delim) buffer = append_coord(buffer, coord, delim)

/**
 * @brief Append a list of coordinates to a buffer separated by a delimiter.
 */
string append_coord_list(string buffer, SvgCoordList *coord_list, const char *delim, const char *coord_delim)
{
	while (coord_list != NULL)
	{
		APPEND_COORD(buffer, coord_list->coord, coord_delim);

		coord_list = coord_list->next;
		if (coord_list != NULL)
			APPEND_ARR(buffer, delim);
	}
	return buffer;
}
#define APPEND_COORD_LIST(buffer, coord_list, delim, coord_delim) buffer = append_coord_list(buffer, coord_list, delim, coord_delim)

/**
 * @brief Append a property in the form ` prop="value"` to a buffer.
 */
string append_prop(string buffer, const char *prop, char *value)
{
	APPEND_ARR(buffer, " ");
	APPEND_ARR(buffer, prop);
	APPEND_ARR(buffer, "=\"");
	APPEND_ARR(buffer, value);
	APPEND_ARR(buffer, "\"");
	return buffer;
}
#define APPEND_PROP(buffer, prop, value) buffer = append_prop(buffer, prop, value)

/**
 * @brief Append a property in the form ` prop="value"` to a buffer.
 */
string append_int_prop(string buffer, const char *prop, int value)
{
	APPEND_ARR(buffer, " ");
	APPEND_ARR(buffer, prop);
	APPEND_ARR(buffer, "=\"");
	APPEND_INT(buffer, value);
	APPEND_ARR(buffer, "\"");
	return buffer;
}
#define APPEND_INT_PROP(buffer, prop, value) buffer = append_int_prop(buffer, prop, value)
string append_svg_element(string buffer, SvgInst *element)
{
	switch (element->kind)
	{
	case Line:
	{
		APPEND_ARR(buffer, "<line");
		SvgCoord *coord = element->coords->coord;

		APPEND_INT_PROP(buffer, "x1", coord->x);
		APPEND_INT_PROP(buffer, "y1", coord->y);

		coord = element->coords->next->coord;
		APPEND_INT_PROP(buffer, "x2", coord->x);
		APPEND_INT_PROP(buffer, "y2", coord->y);

		if (element->color_fill != NULL)
			APPEND_PROP(buffer, "stroke", element->color_fill);

		APPEND_ARR(buffer, "/>");
		APPEND_ARR(buffer, "\n");
		return buffer;
	}
	case Polyline:
	{
		APPEND_ARR(buffer, "<polyline points=\"");

		APPEND_COORD_LIST(buffer, element->coords, " ", ",");
		APPEND_ARR(buffer, "\"");

		if (element->color_fill != NULL)
			APPEND_PROP(buffer, "stroke", element->color_fill);

		APPEND_PROP(buffer, "fill", "none");

		APPEND_ARR(buffer, "/>");
		APPEND_ARR(buffer, "\n");
		return buffer;
	}
	case Polygon:
	{
		APPEND_ARR(buffer, "<polygon points=\"");
		APPEND_COORD_LIST(buffer, element->coords, " ", ",");
		APPEND_ARR(buffer, "\"");

		if (element->color_stroke != NULL)
			APPEND_PROP(buffer, "stroke", element->color_stroke);

		if (element->color_fill != NULL)
			APPEND_PROP(buffer, "fill", element->color_fill);

		APPEND_ARR(buffer, "/>");
		APPEND_ARR(buffer, "\n");
		return buffer;
	}
	case Circle:
	{
		APPEND_ARR(buffer, "<circle");
		SvgCoord *coord = element->coords->coord;
		APPEND_INT_PROP(buffer, "cx", coord->x);
		APPEND_INT_PROP(buffer, "cy", coord->y);
		APPEND_INT_PROP(buffer, "r", element->width);

		if (element->color_stroke != NULL)
			APPEND_PROP(buffer, "stroke", element->color_stroke);

		if (element->color_fill != NULL)
			APPEND_PROP(buffer, "fill", element->color_fill);

		APPEND_ARR(buffer, "/>");
		APPEND_ARR(buffer, "\n");
		return buffer;
	}
	case Ellipse:
	{
		APPEND_ARR(buffer, "<ellipse");
		SvgCoord *coord = element->coords->coord;
		APPEND_INT_PROP(buffer, "cx", coord->x);
		APPEND_INT_PROP(buffer, "cy", coord->y);
		APPEND_INT_PROP(buffer, "rx", element->width);
		APPEND_INT_PROP(buffer, "ry", element->height);

		if (element->color_stroke != NULL)
			APPEND_PROP(buffer, "stroke", element->color_stroke);

		if (element->color_fill != NULL)
			APPEND_PROP(buffer, "fill", element->color_fill);

		APPEND_ARR(buffer, "/>");
		APPEND_ARR(buffer, "\n");
		return buffer;
	}
	case Rect:
	{
		APPEND_ARR(buffer, "<rect");
		SvgCoord *coord = element->coords->coord;
		APPEND_INT_PROP(buffer, "x", coord->x);
		APPEND_INT_PROP(buffer, "y", coord->y);
		APPEND_INT_PROP(buffer, "width", element->width);
		APPEND_INT_PROP(buffer, "height", element->height);

		APPEND_ARR(buffer, "\"");

		if (element->color_stroke != NULL)
			APPEND_PROP(buffer, "stroke", element->color_stroke);

		if (element->color_fill != NULL)
			APPEND_PROP(buffer, "fill", element->color_stroke);

		APPEND_ARR(buffer, "/>");
		APPEND_ARR(buffer, "\n");
		return buffer;
	}
	case Text:
	{
		APPEND_ARR(buffer, "<text");
		SvgCoord *coord = element->coords->coord;
		APPEND_INT_PROP(buffer, "x", coord->x);
		APPEND_INT_PROP(buffer, "y", coord->y);

		if (element->anchor != NULL)
			APPEND_PROP(buffer, "text-anchor", element->anchor);

		if (element->color_stroke != NULL)
			APPEND_PROP(buffer, "stroke", element->color_stroke);

		if (element->color_fill != NULL)
			APPEND_PROP(buffer, "fill", element->color_fill);

		APPEND_ARR(buffer, ">");
		APPEND_ARR(buffer, element->text);
		APPEND_ARR(buffer, "</text>");
		APPEND_ARR(buffer, "\n");
		return buffer;
	}
	}
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
		ADD_INDENTATION(html, indent);

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
		ADD_INDENTATION(html, indent);

		APPEND_ARR(html, "<h1>");
		APPEND_ARR(html, dom->text);
		APPEND_ARR(html, "</h1>\n");

		return html;
	}
	case Header2:
	{
		string html = STR("");
		ADD_INDENTATION(html, indent);

		APPEND_ARR(html, "<h2>");
		APPEND_ARR(html, dom->text);
		APPEND_ARR(html, "</h2>\n");

		return html;
	}
	case Header3:
	{
		string html = STR("");
		ADD_INDENTATION(html, indent);

		APPEND_ARR(html, "<h3>");
		APPEND_ARR(html, dom->text);
		APPEND_ARR(html, "</h3>\n");

		return html;
	}
	case Header4:
	{
		string html = STR("");
		ADD_INDENTATION(html, indent);

		APPEND_ARR(html, "<h4>");
		APPEND_ARR(html, dom->text);
		APPEND_ARR(html, "</h4>\n");

		return html;
	}
	case Header5:
	{
		string html = STR("");
		ADD_INDENTATION(html, indent);

		APPEND_ARR(html, "<h5>");
		APPEND_ARR(html, dom->text);
		APPEND_ARR(html, "</h5>\n");

		return html;
	}
	case Header6:
	{
		string html = STR("");
		ADD_INDENTATION(html, indent);

		APPEND_ARR(html, "<h6>");
		APPEND_ARR(html, dom->text);
		APPEND_ARR(html, "</h6>\n");

		return html;
	}
	case Quote:
	{
		string html = STR("");
		ADD_INDENTATION(html, indent);

		APPEND_ARR(html, "<blockquote>");
		APPEND_ARR(html, dom->text);
		APPEND_ARR(html, "</blockquote>\n");

		return html;
	}
	case Paragraph:
	{
		string html = STR("");
		ADD_INDENTATION(html, indent);

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
				ADD_INDENTATION(html, indent + 1);
			}

			string content = code_generation_from_dom(child->dom, indent + 1); // Indentation not relevant here

			APPEND_STR(html, content);

			previous_child = child->dom;
			child = child->next;
		}

		APPEND_ARR(html, "\n");
		ADD_INDENTATION(html, indent);
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
		ADD_INDENTATION(html, indent);

		APPEND_ARR(html, "<code>");
		APPEND_ARR(html, dom->text);
		APPEND_ARR(html, "</code>\n");

		return html;
	}
	case BlockCode:
	{
		string html = STR("");
		ADD_INDENTATION(html, indent);

		APPEND_ARR(html, "<pre>");
		APPEND_ARR(html, dom->text);
		APPEND_ARR(html, "</pre>\n");

		return html;
	}
	case Link:
	{
		string html = STR("");
		ADD_INDENTATION(html, indent);

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
		ADD_INDENTATION(html, indent);

		APPEND_ARR(html, "<img");
		APPEND_PROP(html, "src", dom->url);
		APPEND_PROP(html, "alt", dom->text);
		APPEND_ARR(html, "\">\n");
		return html;
	}
	case SVG:
	{
		string html = STR("");
		if (dom->svg_coords == NULL || dom->svg_coords->next == NULL)
			return html;

		ADD_INDENTATION(html, indent);
		APPEND_ARR(html, "<svg viewBox=\"");
		// Add viewbox coordinates
		APPEND_COORD_LIST(html, dom->svg_coords, " ", " ");

		APPEND_ARR(html, "\">\n");

		SvgList *child = dom->svg_children;
		while (child != NULL)
		{
			ADD_INDENTATION(html, indent + 1);
			html = append_svg_element(html, child->svg);
			child = child->next;
		}

		ADD_INDENTATION(html, indent);
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
