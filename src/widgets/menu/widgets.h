#ifndef MYY_WIDGETS_MENU_WIDGETS_H
#define MYY_WIDGETS_MENU_WIDGETS_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <globals.h>
#include <stdint.h>

#include "myy/helpers/position.h"
#include "myy/helpers/dimensions.h"
#include "myy/helpers/vector.h"

#include "src/widgets/menu_forms.h"
#include "src/widgets/text_buffer.h"


/* TODO Add a real text area
 * TODO Menu navigation
 * TODO Add instruction
 * TODO -> Edit new instruction "INST"
 * TODO  -> Button : Add field
 * TODO   -> Edit new field "FLD"
 * TODO    -> Other actions...
 * TODO     -> Delete field
 * TODO    -> Instruction field editor
 * TODO  -> Button : Other actions...
 * TODO    -> New menu - Button : Delete instruction
 * TODO     -> On click - Reset navigation and open instructions list
 */
union myy_menu_widget;

struct draw_area_S {
	position_S_4D upper_left;
	dimensions_S dimensions;
};

struct myy_menu_widget_ops {
	void (*draw_opaque)(
		union myy_menu_widget * __restrict const widget,
		myy_states * __restrict const states,
		uint32_t i);
	void (*draw_transparent)(
		union myy_menu_widget * __restrict const widget,
		myy_states * __restrict const states,
		uint32_t i);
	void (*handle_click)(
		union myy_menu_widget * __restrict const widget,
		myy_states * __restrict const states,
		position_S const absolute_pos,
		enum myy_input_events const event_type,
		union myy_input_event_data const * __restrict const event_data);
	void (*set_position)(
		union myy_menu_widget * __restrict const widget,
		myy_states * __restrict const states,
		position_S_4D const position);
	dimensions_S (*prepare)(
		union myy_menu_widget * __restrict const widget,
		myy_states * __restrict const states);
	void (*cleanup)(
		union myy_menu_widget * __restrict const widget,
		myy_states * __restrict const states);
};
typedef struct myy_menu_widget_ops myy_menu_widget_ops_t;

struct myy_generic_widget {
	myy_menu_widget_ops_t * ops;
	position_S_4D draw_offset;
	dimensions_S dimensions;
};

#define myy_menu_widget_ops_for(prefix) {\
	.draw_opaque      =  prefix## _draw_opaque, \
	.draw_transparent =  prefix## _draw_transparent,\
	.handle_click     =  prefix## _handle_click,\
	.set_position     =  prefix## _set_position,\
	.prepare          =  prefix## _prepare,\
	.cleanup          =  prefix## _cleanup,\
}

#include "src/widgets/menu/button.h"
#include "src/widgets/menu/list.h"
#include "src/widgets/menu/text_area.h"
#include "src/widgets/menu/text_edit.h"
#include "src/widgets/menu/vertical_layout.h"

enum myy_menu_widget_type {
	myy_menu_widget_type_invalid,
	myy_menu_widget_type_button,
	myy_menu_widget_type_text_area,
	myy_menu_widget_type_text_edit,
	myy_menu_widget_type_list,
	myy_menu_widget_type_list_strings,
	myy_menu_widget_type_vertical_layout,
	myy_menu_widget_type_count
};
typedef enum myy_menu_widget_type myy_menu_widget_type_t;

__attribute__((unused))
static inline bool myy_menu_widget_type_is_valid(
	myy_menu_widget_type_t const type)
{
	return
		(myy_menu_widget_type_invalid < type)
		& (type < myy_menu_widget_type_count);
}

union myy_menu_widget_add_args {
	myy_widget_button_add_args_t button;
	myy_widget_text_area_add_args_t text_area;
	myy_widget_text_edit_add_args_t text_edit;
	myy_widget_list_add_arguments_t list;
	myy_widget_list_generic_strings_add_args_t list_strings;
	myy_widget_vertical_layout_add_args_t vertical_layout;
};
typedef union myy_menu_widget_add_args myy_menu_widget_add_args_t;

union myy_menu_widget {
	struct myy_generic_widget generic;
	struct myy_widget_button button;
	struct myy_widget_text_area text_area;
	struct myy_widget_text_edit text_edit;
	struct myy_widget_list list;
	struct myy_widget_list list_strings;
	struct myy_widget_vertical_layout vertical_layout;
};
typedef union myy_menu_widget myy_menu_widget_t;

myy_vector_template(menu_widgets, union myy_menu_widget)

__attribute__((unused))
static inline void
myy_vector_menu_widgets_draw_opaque(
	myy_vector_menu_widgets * __restrict const widgets,
	myy_states * __restrict const states,
	uint32_t i)
{
	myy_vector_for_each_ptr(
		myy_menu_widget_t, widget, in, widgets,
		{
			widget->generic.ops->draw_opaque(widget, states, i);
		}
	);
}

__attribute__((unused))
static inline void
myy_vector_menu_widgets_draw_transparent(
	myy_vector_menu_widgets * __restrict const widgets,
	myy_states * __restrict const states,
	uint32_t i)
{
	myy_vector_for_each_ptr(
		myy_menu_widget_t, widget, in, widgets,
		{
			widget->generic.ops->draw_transparent(widget, states, i);
		}
	);
}

__attribute__((unused))
static inline void
myy_vector_menu_widgets_cleanup(
	myy_vector_menu_widgets * __restrict const widgets,
	myy_states * __restrict const states)
{
	myy_vector_for_each_ptr(
		myy_menu_widget_t, widget, in, widgets,
		{
			widget->generic.ops->cleanup(widget, states);
		}
	);
}

struct myy_menu_widget_definition {
	myy_menu_widget_type_t type;
	myy_menu_widget_add_args_t args;
};
typedef struct myy_menu_widget_definition myy_menu_widget_definition_t;

#define WIDGET_BUTTON_DEF(button_text, func_ptr, func_ptr_arg) {\
	.type = myy_menu_widget_type_button,\
	.args = {\
		.button = {\
			.text = button_text,\
			.action = func_ptr,\
			.action_arg = func_ptr_arg\
		}\
	}\
}

#define WIDGET_TEXT_AREA_DEF(textarea_text) {\
	.type = myy_menu_widget_type_text_area,\
	.args = {\
		.text_area = {\
			.text = textarea_text,\
		}\
	}\
}

#define WIDGET_TEXT_EDIT_DEF(textarea_text, ae_func_ptr, func_ptr_arg) {\
	.type = myy_menu_widget_type_text_edit, \
	.args = {                               \
		.text_edit = {                      \
			.text = textarea_text,          \
			.after_edit = ae_func_ptr,      \
			.after_edit_arg = func_ptr_arg  \
		}                                   \
	}                                       \
}

#define WIDGET_LIST_STRINGS_DEF()

#define WIDGET_VERTICAL_LAYOUT_DEF(sub_widgets_def_arr) {\
	.type = myy_menu_widget_type_vertical_layout,      \
	.args = {                                          \
		.vertical_layout = {                           \
			.n_widgets =                               \
				sizeof(sub_widgets_def_arr)            \
				/sizeof(myy_menu_widget_definition_t), \
			.widgets   = sub_widgets_def_arr,          \
		}                                              \
	}                                                  \
}


typedef bool (*myy_menu_widget_generic_add_handler)(
	union myy_menu_widget * __restrict const widgets,
	myy_states * __restrict const states,
	void * const args);

__attribute__((unused))
static inline bool myy_menu_widget_invalid_add_handler(
	union myy_menu_widget * __restrict const widgets,
	myy_states * __restrict const states,
	void * const args)
{
	LOG("[myy_menu_widget_invalid_add_handler]\n"
		"CALLED AN INVALID HANDLER !\n");
	return false;
}

__attribute__((unused))
__attribute__((pure))
static inline
myy_menu_widget_generic_add_handler
myy_menu_widget_add_handler_from_type(
	myy_menu_widget_type_t const type)
{
	myy_menu_widget_generic_add_handler handler;
	switch (type) {
		case myy_menu_widget_type_invalid:
			handler =
				(myy_menu_widget_generic_add_handler)
				myy_menu_widget_invalid_add_handler;
			break;
		case myy_menu_widget_type_button:
			handler =
				(myy_menu_widget_generic_add_handler)
				myy_widget_button_add;
			break;
		case myy_menu_widget_type_text_area:
			handler =
				(myy_menu_widget_generic_add_handler)
				myy_widget_text_area_add;
			break;
		case myy_menu_widget_type_text_edit:
			handler =
				(myy_menu_widget_generic_add_handler)
				myy_widget_text_edit_add;
			break;
		case myy_menu_widget_type_list:
			handler =
				(myy_menu_widget_generic_add_handler)
				myy_widget_list_add;
			break;
		case myy_menu_widget_type_list_strings:
			handler =
				(myy_menu_widget_generic_add_handler)
				myy_widget_list_add_generic_string_list;
			break;
		case myy_menu_widget_type_vertical_layout:
			handler =
				(myy_menu_widget_generic_add_handler)
				myy_widget_vertical_layout_add;
			break;
		case myy_menu_widget_type_count:
		default:
			handler =
				(myy_menu_widget_generic_add_handler)
				myy_menu_widget_invalid_add_handler;
			break;
	}
	return handler;
}

#ifdef __cplusplus
}
#endif

#endif
