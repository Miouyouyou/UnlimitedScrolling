#include "src/widgets/menu/widgets.h"
#include "myy/helpers/random.h"

#define MARGIN (6) /* px */

static void myy_button_widget_handle_click(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict states,
	position_S const absolute_pos,
	enum myy_input_events const event_type,
	union myy_input_event_data const * __restrict const event_data)
{
	struct myy_widget_button * __restrict const button =
		&widget->button;

	LOG("[MYY_BUTTON_WIDGET_HANDLE_CLICK] Event type : %u, absolute_pos : {%d,%d}\n",
		event_type, absolute_pos.x, absolute_pos.y);
	button->action(button->user_arg, states, button);
}

static void myy_button_widget_draw_opaque(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states,
	uint32_t const i)
{
	struct myy_widget_button * __restrict const button =
		&widget->button;

	simple_forms_set_draw_offset(&button->gl_forms, button->draw_offset);
	simple_forms_draw(&button->gl_forms);
}

static void myy_button_widget_draw_transparent(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states,
	uint32_t const i)
{
	struct myy_widget_button * __restrict const button =
		&widget->button;

	text_buffer_set_draw_offset(&button->gl_text, button->draw_offset);
	text_buffer_draw(&button->gl_text);
}

static void myy_button_widget_set_position(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states,
	position_S_4D const abs_position)
{
	struct myy_widget_button * __restrict const button =
		&widget->button;
	
	/* FIXME Do we need to store this ?
	 * Can't we just setup the sub-widgets in one-pass if that's
	 * setup and be done with it ?
	 * Only the dimensions interest us anyway.
	 */
	button->draw_offset = abs_position;
}

static dimensions_S myy_button_widget_prepare(
	union myy_menu_widget * __restrict const widget,
	struct myy_states_s * __restrict const states)
{
	struct myy_widget_button * __restrict const button =
		(struct myy_widget_button *__restrict) widget;

	/* Copy lateral menu "add button" code */
	simple_forms_reset(&button->gl_forms);
	text_buffer_reset(&button->gl_text);

	position_S_3D text_draw_position =
		position_S_3D_struct(MARGIN,0,-2);
	/* FIXME text_buffer HAVE TO RETURN THE TEXT BOUNDING BOX.
	 * Inferring it from the current pen position will only
	 * lead to issues with multiline text.
	 */
	text_buffer_add_string_colored(
		&button->gl_text,
		(uint8_t const * __restrict) button->text,
		&text_draw_position,
		rgba8_white_opaque());

	text_buffer_store_to_gpu(&button->gl_text);

	dimensions_S const button_new_dimensions = dimensions_S_struct(
		text_draw_position.x + MARGIN,
		text_draw_position.y + MARGIN);

	simple_forms_add_rectangle_3D_upper_left(
		&button->gl_forms,
		position_S_3D_struct(0,0,-1),
		button_new_dimensions,
		rgba8_color(50,myy_random8(),25,255));

	simple_forms_store_to_gpu(&button->gl_forms);

	button->dimensions = button_new_dimensions;
	return button_new_dimensions;
}

static void myy_button_widget_cleanup(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states)
{
	struct myy_widget_button * __restrict const button =
		&widget->button;

	simple_forms_cleanup(&button->gl_forms, states);
	text_buffer_cleanup(&button->gl_text, states);
}

static struct myy_menu_widget_ops myy_button_widget_ops =
	myy_menu_widget_ops_for(myy_button_widget);

static void myy_button_widget_default_handler(
	void * __restrict const user_arg,
	myy_states * __restrict const states,
	struct myy_widget_button * __restrict const widget) 
{
	LOG("[myy_button_widget_default_handler] Button clicked\n");
}

/* TODO Separare add and init */
bool myy_widget_button_add(union myy_menu_widget*const widget, myy_states*const states, myy_widget_button_add_args_t*const args)
{
	struct myy_widget_button * __restrict const button =
		&widget->button;

	LOG("[MYY_BUTTON_WIDGET_INIT] INIT !\n");
	LOG("Widget space : %p\n", widget);
	
	button->ops         = &myy_button_widget_ops;
	button->draw_offset = position_S_4D_struct(0,0,0,0);
	button->dimensions  = dimensions_S_struct(0,0);
	LOG("???\n");
	simple_forms_init(&button->gl_forms);
	LOG("♪ Is it the text ? Is it the crash ? ♪\n");
	text_buffer_init(&button->gl_text,
		gl_text_atlas_addr_from(states));
	button->text        = 
		(args->text ? args->text : "No text provided");
	button->action      = 
		(args->action ?
		 args->action : myy_button_widget_default_handler);
	button->user_arg    = args->action_arg;
		
	LOG("END INIT !\n");
	return true;
}

