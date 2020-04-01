#ifndef MYY_VERTICAL_LAYOUT_H
#define MYY_VERTICAL_LAYOUT_H 1

#include "myy/helpers/position.h"
#include "myy/helpers/dimensions.h"
#include "myy/helpers/vector.h"

#include "myy.h"
#include "myy_user_state.h"

#include "src/widgets/widget_area.h"

#include <stdbool.h>

struct myy_menu_widget_definition;


/* TODO Sweep the areas in the click handler.
 * TODO Factorize the sweep function.
 * TODO Finish the vertical layout
 * TODO Test it on a menu
 * TODO Use it in the instruction editor
 */
struct myy_widget_vertical_layout {
	struct myy_menu_widget_ops * ops;
	position_S_4D draw_offset;
	dimensions_S dimensions;
	myy_vector_widget_areas widget_areas;
	struct myy_vector widgets;
};
typedef struct myy_widget_vertical_layout myy_widget_vertical_layout_t;

struct myy_widget_vertical_layout_add_args {
	uint64_t const n_widgets;
	struct myy_menu_widget_definition * __restrict const widgets;
};
typedef struct myy_widget_vertical_layout_add_args
	myy_widget_vertical_layout_add_args_t;

/* TODO
 * Think and implement an interface allowing the addition
 * of a preinitialized widget
 * - Is there a use of that ?
 */
/* Use to add a vertical layout to an existing widget tree */
bool myy_widget_vertical_layout_add(
	struct myy_widget_vertical_layout * __restrict const widget,
	myy_states * __restrict const states,
	myy_widget_vertical_layout_add_args_t * __restrict const args);

/* Used to initialize a vertical layout */
bool myy_widget_vertical_layout_init(
	struct myy_widget_vertical_layout * __restrict const widget,
	myy_states * __restrict const states);

bool myy_widget_vertical_layout_append_widgets_defs_array(
	myy_widget_vertical_layout_t * __restrict const layout,
	myy_states * __restrict const states,
	struct myy_menu_widget_definition const * __restrict definition,
	size_t n_widgets);

struct myy_vector_menu_widgets_st  * myy_widget_vertical_layout_widgets(
	myy_widget_vertical_layout_t * __restrict const layout);

#endif
