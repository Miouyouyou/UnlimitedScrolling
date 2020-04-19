#include "src/widgets/menu/widgets.h"

#include "myy/helpers/temp_string.h"
#include <stdbool.h>

#define MARGIN_TOP     (20)
#define MARGIN_LEFT    (8)
#define MARGIN_BOTTOM  (28)
#define MARGIN_RIGHT   (8)

static void myy_widget_text_edit_draw_opaque(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states,
	uint32_t i)
{
	struct myy_widget_text_edit * __restrict const
		text_edit = &widget->text_edit;

	simple_forms_set_draw_offset(&text_edit->gl_forms,
		text_edit->draw_offset);
	simple_forms_draw(&text_edit->gl_forms);
}

static void myy_widget_text_edit_draw_transparent(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states,
	uint32_t i)
{
	struct myy_widget_text_edit * __restrict const
		text_edit = &widget->text_edit;

	text_buffer_set_draw_offset(
		&text_edit->gl_text,
		text_edit->draw_offset);
	text_buffer_draw(&text_edit->gl_text);
}
static void myy_widget_text_edit_set_draw_offset(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states,
	position_S_4D const position)
{
	struct myy_widget_text_edit * __restrict const
		text_edit = &widget->text_edit;

	text_edit->draw_offset = position;
}

/* TODO Export ? */
char const * __restrict const myy_widget_text_edit_input_value(
	struct myy_widget_text_edit * __restrict const text_edit)
{
	return temp_string_text(&text_edit->input);
}

static dimensions_S myy_widget_text_edit_prepare(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states)
{
	struct myy_widget_text_edit * __restrict const
		text_edit = &widget->text_edit;
	position_S_3D text_draw_pos =
		position_S_3D_struct(MARGIN_LEFT,MARGIN_TOP,0);

	text_buffer_reset(&text_edit->gl_text);
	LOG("Text : %s\n", myy_widget_text_edit_input_value(text_edit));
	text_buffer_add_string_colored(
		&text_edit->gl_text,
		(uint8_t const *)
			myy_widget_text_edit_input_value(text_edit),
		&text_draw_pos,
		rgba8_white_opaque());
	text_buffer_store_to_gpu(&text_edit->gl_text);
	dimensions_S const dimensions = {
		(uint16_t) (text_draw_pos.x + MARGIN_LEFT),
		(uint16_t) (text_draw_pos.y + MARGIN_BOTTOM)
	};
	LOG("dimensions : %d, %d\n", dimensions.width, dimensions.height);

	simple_forms_reset(&text_edit->gl_forms);
	simple_forms_add_rectangle_3D_upper_left(
		&text_edit->gl_forms,
		position_S_3D_struct(0,0,0),
		dimensions,
		rgba8_black_opaque());
	simple_forms_store_to_gpu(&text_edit->gl_forms);

	text_edit->dimensions = dimensions;
	LOG("[MYY_WIDGET_TEXT_AREA_PREPARE] I AM PREPARED !\n");
	return dimensions;
}

/* TODO Uniformiser */
static void myy_widget_text_edit_input_value_set(
	struct myy_widget_text_edit * __restrict const text_edit,
	myy_states * __restrict const states,
	char const * __restrict const new_value)
{
	temp_string_text_set(&text_edit->input, new_value);
	
	myy_widget_text_edit_prepare(
		(union myy_menu_widget *) text_edit, states);
}

static void myy_text_input_done_cb(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states,
	char const * __restrict const text)
{
	struct myy_widget_text_edit * __restrict const
		text_edit = &widget->text_edit;
	myy_widget_text_edit_input_value_set(
		text_edit, states, text);
	text_edit->after_edit(
		text_edit->after_edit_arg,
		states,
		text_edit,
		text);
}

static void myy_widget_text_edit_handle_click(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states,
	position_S const absolute_pos,
	enum myy_input_events const event_type,
	union myy_input_event_data const * __restrict const event_data)
{
	struct myy_widget_text_edit * __restrict const
		text_edit = &widget->text_edit;
	position_S_4D const draw_offset = text_edit->draw_offset;
	dimensions_S const dims = text_edit->dimensions;

	myy_user_state_t * __restrict const ustate =
		myy_user_state_from(states);

	ustate->edited.callback_arg = text_edit;
	ustate->edited.done_callback = 
		(myy_user_state_edited_callback) myy_text_input_done_cb;
	myy_text_input_start(states);

}

static void myy_widget_text_edit_input_destroy(
	struct myy_widget_text_edit * __restrict const text_edit,
	myy_states * __restrict const states)
{
	temp_string_destroy(&text_edit->input);
}


static void myy_widget_text_edit_cleanup(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states)
{
	struct myy_widget_text_edit * __restrict const
		text_edit = &widget->text_edit;
	simple_forms_cleanup(&text_edit->gl_forms, states);
	text_buffer_cleanup(&text_edit->gl_text, states);
	myy_widget_text_edit_input_destroy(text_edit, states);
}

static struct myy_menu_widget_ops ops = {
	.draw_opaque      = myy_widget_text_edit_draw_opaque,
	.draw_transparent = myy_widget_text_edit_draw_transparent,
	.handle_click     = myy_widget_text_edit_handle_click,
	.set_position     = myy_widget_text_edit_set_draw_offset,
	.prepare          = myy_widget_text_edit_prepare,
	.cleanup          = myy_widget_text_edit_cleanup
};

static void myy_widget_text_edit_init(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states)
{
	struct myy_widget_text_edit * __restrict const
		text_edit = &widget->text_edit;

	text_edit->ops         = &ops;
	text_edit->draw_offset = position_S_4D_struct(0,0,0,0);
	text_edit->dimensions  = dimensions_S_struct(0,0);
	simple_forms_init(&text_edit->gl_forms);
	text_buffer_init(&text_edit->gl_text,
		gl_text_atlas_addr_from(states));
	text_edit->input       = temp_string_create("");
}

static void default_after_edit_handler(
	void * user_arg,
	myy_states * __restrict const states,
	struct myy_widget_text_edit * __restrict const widget,
	char const * __restrict const current_value)
{
	LOG("[default_after_edit_handler] Meow : %s\n", current_value);
}

bool myy_widget_text_edit_add(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states,
	myy_widget_text_edit_add_args_t * __restrict const args)
{
	struct myy_widget_text_edit * __restrict const
		text_edit = &widget->text_edit;

	myy_widget_text_edit_init(widget, states);

	LOG("[myy_widget_text_edit_add] Double frites patates !\n");
	if (args->default_text)
	{
		myy_widget_text_edit_input_value_set(
			text_edit, states, args->default_text);
	}

	myy_widget_text_edit_after_edit_handler after_edit_handler =
		args->after_edit_handler ?
		args->after_edit_handler : default_after_edit_handler;
	text_edit->after_edit = after_edit_handler;
	text_edit->after_edit_arg = args->after_edit_arg;

	return true;
}
 
 
