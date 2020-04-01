#ifndef MYY_SRC_WIDGETS_MENU_TEXT_EDIT_H
#define MYY_SRC_WIDGETS_MENU_TEXT_EDIT_H 1

#include "myy/helpers/vector.h"
#include "myy/helpers/dimensions.h"
#include "myy/helpers/position.h"
#include "myy/helpers/temp_string.h"

#include "src/widgets/menu_forms.h"
#include "src/widgets/text_buffer.h"
#include "src/widgets/widget_area.h"

#include <stdbool.h>

struct myy_widget_text_edit;

/* FIXME Handlers can be uniformized like this :
 * - user_arg
 * - states
 * - widget
 * - additional_infos
 */
typedef void (*myy_widget_text_edit_after_edit_handler)(
	void * user_arg,
	myy_states * __restrict const states,
	struct myy_widget_text_edit * __restrict const widget,
	char const * __restrict const current_value);

struct myy_widget_text_edit {
	struct myy_menu_widget_ops * ops;
	position_S_4D draw_offset;
	dimensions_S dimensions;
	struct simple_forms gl_forms;
	struct text_buffer gl_text;
	temp_string_t input;
	myy_widget_text_edit_after_edit_handler after_edit;
	void * after_edit_arg;
};

typedef struct myy_widget_text_edit myy_widget_text_edit_t;

struct myy_widget_text_edit_add_args {
	char const * __restrict default_text;
	myy_widget_text_edit_after_edit_handler after_edit_handler;
	void * after_edit_arg;
};

typedef struct myy_widget_text_edit_add_args
	myy_widget_text_edit_add_args_t;

bool myy_widget_text_edit_add(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states,
	myy_widget_text_edit_add_args_t * __restrict const args);

char const * __restrict const myy_widget_text_edit_input_value(
	struct myy_widget_text_edit * __restrict const text_edit);

#endif
