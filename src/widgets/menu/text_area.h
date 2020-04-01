#ifndef MYY_SRC_WIDGETS_TEXT_AREA_H
#define MYY_SRC_WIDGETS_TEXT_AREA_H 1

#include "myy/helpers/dimensions.h"
#include "myy/helpers/position.h"
#include "src/widgets/text_buffer.h"
#include "myy/helpers/temp_string.h"


struct myy_widget_text_area {
	struct myy_menu_widget_ops * ops;
	position_S_4D draw_offset;
	dimensions_S dimensions;
	struct text_buffer gl_text;
	/* TODO Check for a REAL use of this.
	 * This was implemented to avoid having to
	 * add it in the last minute when a
	 * requirement pops up...
	 * However, if none "pops up", maybe this
	 * should be removed
	 */
	temp_string_t text;
};

typedef struct myy_widget_text_area myy_widget_text_area_t;

struct myy_widget_text_area_add_args {
	char const * __restrict const text;
};

typedef struct myy_widget_text_area_add_args
	myy_widget_text_area_add_args_t;

bool myy_widget_text_area_add(
	union myy_menu_widget * __restrict const memory_space,
	myy_states * __restrict const states,
	myy_widget_text_area_add_args_t * __restrict const args);

#endif
