#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

#define SAFE_FREE(ptr) if ((ptr) != NULL) free((ptr));


SvgCoord* new_svg_coord(int x, int y) {
    SvgCoord* svg_coord = malloc(sizeof(SvgCoord));

    svg_coord->x = x;
    svg_coord->y = y;

    return svg_coord;
}

SvgCoordList* new_svg_coord_list(SvgCoord* svg_coord) {
    SvgCoordList* svg_coord_list = malloc(sizeof(SvgCoordList));

    svg_coord_list->coord = svg_coord;
    svg_coord_list->next = NULL;

    return svg_coord_list;
}

SvgInst* new_svg_inst(SvgInstKind kind, SvgCoordList* coords) {
    SvgInst* svg_inst = malloc(sizeof(SvgInst));

    svg_inst->kind = kind;
    svg_inst->coords = coords;
    svg_inst->width = -1;
    svg_inst->height = -1;
    svg_inst->text = NULL;
    svg_inst->anchor = NULL;
    svg_inst->color_stroke = NULL;
    svg_inst->color_fill = NULL;

    return svg_inst;
}

SvgList* new_svg_list(SvgInst* svg) {
    SvgList* svg_list = malloc(sizeof(SvgList));

    svg_list->svg = svg;
    svg_list->next = NULL;

    return svg_list;
}

DOM* new_dom(DomElement dom_el, DomList* children) {
    DOM* dom = malloc(sizeof(DOM));

    dom->dom_el = dom_el;
    dom->children = children;
    dom->svg_children = NULL;
    dom->text = NULL;
    dom->url = NULL;

    return dom;
}

DomList* new_dom_list(DOM* dom) {
    DomList* dom_list = malloc(sizeof(DomList));

    dom_list->dom = dom;
    dom_list->next = NULL;

    return dom_list;
}

/*
END OF THE COMPLETION.
----------------------
YOU DO NOT HAVE TO CHANGE THE FUNCTIONS BELOW.
*/

void free_svg_coord_list(SvgCoordList* svg_coord_list) {
    SvgCoordList* curr = svg_coord_list;

    while (curr != NULL) {
        if (curr->coord != NULL) {
            free(curr->coord);
        }
        SvgCoordList* prev = curr;
        curr = curr->next;
        free(prev);
    }
}

void free_svg_inst(SvgInst* svg_inst) {
    if (svg_inst == NULL) return;
    if (svg_inst->coords) free_svg_coord_list(svg_inst->coords);

    SAFE_FREE(svg_inst->text);
    SAFE_FREE(svg_inst->anchor);
    SAFE_FREE(svg_inst->color_stroke);
    SAFE_FREE(svg_inst->color_fill);

    free(svg_inst);
}

void free_svg_list(SvgList* svg_list) {
    SvgList* curr = svg_list;

    while (curr != NULL) {
        if (curr->svg != NULL) free_svg_inst(curr->svg);
        SvgList* prev = curr;
        curr = curr->next;
        free(prev);
    }
}

void free_dom(DOM* dom) {
    if (dom == NULL) return;

    free_dom_list(dom->children);
    free_svg_list(dom->svg_children);

    SAFE_FREE(dom->text);
    SAFE_FREE(dom->url);

    free(dom);
}

void free_dom_list(DomList* dom_list) {
    DomList* curr = dom_list;

    while (curr != NULL) {
        if (curr->dom != NULL) free_dom(curr->dom);
        DomList* prev = curr;
        curr = curr->next;
        free(prev);
    }
}
