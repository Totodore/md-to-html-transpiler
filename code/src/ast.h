#ifndef __H_AST
#define __H_AST

/*
YOU DO NOT HAVE TO CHANGE THIS FILE.
YOU MAY IF YOU WANT TO ADD ANY IMPROVEMENT.
*/

typedef enum dom_element {
    Document,

    TextElement,
    Paragraph,
    Bold,
    Italic,
    Underline,
    Strikethrough,

    Header1,
    Header2,
    Header3,
    Header4,
    Header5,
    Header6,

    Quote,
    Link,
    Image,
    InlineCode,
    BlockCode,
    HRule,

    SVG,
} DomElement;

typedef enum svg_inst_kind {
    Line,
    Polyline,
    Polygon,
    Circle,
    Ellipse,
    Rect,
    Text,
} SvgInstKind;

typedef struct svg_coord {
    int x; // The X coordinate.
    int y; // The Y coordinate.
} SvgCoord;

typedef struct svg_coord_list {
    SvgCoord* coord; // An object containing the coordinates.
    struct svg_coord_list* next; // A reference to the next element of the list or NULL if it is the last element.
} SvgCoordList;

typedef struct svg_inst {
    SvgInstKind kind; // The type of instruction (Line, Circle, etc...)
    SvgCoordList* coords; // A reference to the first element of a list of coordinates. NULL if no coordinates needed.
    int width; // The width parameter of the command. -1 by default.
    int height; // The height parameter of the command. -1 by default.
    char* text; // The text used in the TEXT svg command. NULL for the other commands.
    char* anchor; // The anchor parameter in the TEXT svg command. NULL for the other commands.
    char* color_stroke; // The color stroke paremeter. NULL if not defined.
    char* color_fill; // The color fill parameter. NULL if not defined.
} SvgInst;
typedef struct svg_list {
    SvgInst* svg; // An object containing an svg instruction.
    struct svg_list* next; // A reference to the next element of the list or NULL if it is the last element.
} SvgList;

typedef struct dom {
    DomElement dom_el; // The type of dom element (Paragraph, Header1, Quote, etc...)
    struct dom_list* children; // A reference to the first element of a list of dom elements. NULL if no children.
    SvgList* svg_children; // A reference to the first element of a list of svg instructions. NULL if no children.
    char* text; // A text parameter that can be used for TextElement, Header, Link and Image elements. NULL if not needed.
    char* url; // A url parameter that can be used for Link and Image elements. NULL if not needed.
} DOM;

typedef struct dom_list {
    DOM* dom; // An object containing a dom element.
    struct dom_list* next; // A reference to the next element of the list or NULL if it is the last element.
} DomList;


SvgCoord* new_svg_coord(int x, int y);
SvgCoordList* new_svg_coord_list(SvgCoord* svg_coord);
SvgInst* new_svg_inst(SvgInstKind kind, SvgCoordList* coords);
SvgList* new_svg_list(SvgInst* svg);
DOM* new_dom(DomElement dom_el, DomList* children);
DomList* new_dom_list(DOM* dom);

void free_svg_coord_list(SvgCoordList* svg_coord_list);
void free_svg_inst(SvgInst* svg_inst);
void free_svg_list(SvgList* svg_list);
void free_dom(DOM* dom);
void free_dom_list(DomList* dom_list);

#endif
