#ifndef MYY_WIDGETS_MENU_LIST_STRINGS_H
#define MYY_WIDGETS_MENU_LIST_STRINGS_H 1

#include "myy/helpers/position.h"
#include "myy/helpers/dimensions.h"

#include "src/widgets/menu_forms.h"
#include "src/widgets/text_buffer.h"

#include "src/widgets/widget_area.h"

struct myy_widget_list;

typedef void (*myy_list_element_click_handler)(
	struct myy_widget_list * __restrict const list,
	myy_states * __restrict const states,
	int index,
	void * clicked_widget,
	void * user_arg);

struct myy_widget_list {
	struct myy_menu_widget_ops * ops;
	position_S_4D draw_offset;
	dimensions_S dimensions;
	struct simple_forms gl_forms;
	/* FIXME ... This should be myy_vector_menu_widgets
	 * However, menu_widgets is an union of every widget
	 * definition, for convenience purposes.
	 * The vector definition is done AFTER the union
	 * definition, but the union definition requires
	 * every component to be defined BEFORE...
	 * Chicken and egg problem.
	 * I don't really know how to handle that one, so
	 * I'll just use the generic version of "myy_vector",
	 * which is called "myy_vector", and then cast this vector
	 * to myy_vector_menu_widgets when using it...
	 */
	struct myy_vector children_widgets;
	myy_vector_widget_areas children_widgets_areas;
	myy_list_element_click_handler handler;
	void * user_arg;
};
typedef struct myy_widget_list myy_widget_list_t;

struct myy_widget_list_add_arguments {
	myy_list_element_click_handler handler;
	void * user_arg;
};
typedef struct myy_widget_list_add_arguments myy_widget_list_add_arguments_t;

void myy_widget_list_add(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states,
	myy_widget_list_add_arguments_t
		const * __restrict const arguments);

struct myy_widget_list_generic_strings_args {
	size_t const n_elements;
	uint8_t * __restrict const start_address;
	intptr_t const stride;
	char const * (*to_string)(void * to_string_arg, uint8_t * element);
	void * const to_string_arg;
	myy_list_element_click_handler const list_click_handler;
	void * const list_click_arg;
};
typedef struct myy_widget_list_generic_strings_args myy_widget_list_generic_strings_add_args_t;


bool myy_widget_list_add_generic_string_list(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states,
	myy_widget_list_generic_strings_add_args_t * args);

#endif
