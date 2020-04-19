#ifndef MYY_BUTTON_WIDGET_H
#define MYY_BUTTON_WIDGET_H 1

#include "myy/helpers/dimensions.h"
#include "src/widgets/menu_forms.h"

struct myy_widget_button;

typedef void (*myy_widget_button_action_ptr)(
	void * __restrict const user_arg,
	myy_states * __restrict const states,
	struct myy_widget_button * __restrict const widget);

struct myy_widget_button {
	struct myy_menu_widget_ops * ops;
	position_S_4D draw_offset;
	dimensions_S dimensions;
	struct simple_forms gl_forms;
	struct text_buffer gl_text;
	char const * __restrict text;
	myy_widget_button_action_ptr action;
	void * user_arg;
};

typedef struct myy_widget_button myy_widget_button_t;

struct myy_widget_button_add_arguments {
	char const * __restrict text;
	myy_widget_button_action_ptr action;
	void * __restrict const action_arg;
};

typedef struct myy_widget_button_add_arguments myy_widget_button_add_args_t;

bool myy_widget_button_add(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states,
	myy_widget_button_add_args_t * __restrict const args);

/*struct myy_text_area_with_label_widget {
	struct myy_menu_widget_ops * ops;
	struct draw_area_S draw_area;
	struct simple_forms gl_forms;
	struct text_buffer gl_text;
	char const * __restrict label_text;
	char value[16];
};*/

#endif
