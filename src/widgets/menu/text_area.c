#include "src/widgets/menu/widgets.h"

#include "myy/helpers/temp_string.h"
#include <stdbool.h>

#define MARGIN_TOP     (20)
#define MARGIN_LEFT    (8)
#define MARGIN_BOTTOM  (28)
#define MARGIN_RIGHT   (8)


static char const * myy_widget_text_area_value(
	struct myy_widget_text_area * __restrict const text_area)
{
	return temp_string_text(&text_area->text);
}

static void myy_widget_text_area_value_set(
	struct myy_widget_text_area * __restrict const text_area,
	char const * __restrict const value)
{
	temp_string_text_set(&text_area->text, value);
}
static void myy_widget_text_area_draw_opaque(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states,
	uint32_t i)
{
	struct myy_widget_text_area * __restrict const
		text_area = &widget->text_area;

}

static void myy_widget_text_area_draw_transparent(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states,
	uint32_t i)
{
	struct myy_widget_text_area * __restrict const
		text_area = &widget->text_area;

	text_buffer_set_draw_offset(
		&text_area->gl_text,
		text_area->draw_offset);
	text_buffer_draw(&text_area->gl_text);
}

static void myy_widget_text_area_set_draw_offset(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states,
	position_S_4D const position)
{
	struct myy_widget_text_area * __restrict const
		text_area = &widget->text_area;

	text_area->draw_offset = position;
}

static dimensions_S myy_widget_text_area_prepare(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states)
{
	struct myy_widget_text_area * __restrict const
	text_area = &widget->text_area;
	position_S_3D text_draw_pos =
	position_S_3D_struct(MARGIN_LEFT,MARGIN_TOP,0);

	text_buffer_reset(&text_area->gl_text);
	LOG("Text : %s\n", myy_widget_text_area_value(text_area));
	text_buffer_add_string_colored(
		&text_area->gl_text,
		(uint8_t const *)
		myy_widget_text_area_value(text_area),
		&text_draw_pos,
		rgba8_white_opaque());
	text_buffer_store_to_gpu(&text_area->gl_text);
	dimensions_S const dimensions = {
		text_draw_pos.x + MARGIN_LEFT,
		text_draw_pos.y + MARGIN_BOTTOM
	};
	LOG("dimensions : %d, %d\n", dimensions.width, dimensions.height);

	text_area->dimensions = dimensions;
	LOG("[MYY_WIDGET_TEXT_AREA_PREPARE] I AM PREPARED !\n");
	return dimensions;
}

static void myy_widget_text_area_handle_click(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states,
	position_S const absolute_pos,
	enum myy_input_events const event_type,
	union myy_input_event_data const * __restrict const event_data)
{
	struct myy_widget_text_area * __restrict const
	text_area = &widget->text_area;
}

static void myy_widget_text_area_cleanup(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states)
{
	struct myy_widget_text_area * __restrict const
	text_area = &widget->text_area;
	
	text_buffer_cleanup(&text_area->gl_text, states);
}

static struct myy_menu_widget_ops ops = {
	.draw_opaque      = myy_widget_text_area_draw_opaque,
	.draw_transparent = myy_widget_text_area_draw_transparent,
	.handle_click     = myy_widget_text_area_handle_click,
	.set_position     = myy_widget_text_area_set_draw_offset,
	.prepare          = myy_widget_text_area_prepare,
	.cleanup          = myy_widget_text_area_cleanup
};

static bool myy_widget_text_area_init(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states)
{
	struct myy_widget_text_area * __restrict const
	text_area = &widget->text_area;

	text_area->ops         = &ops;
	text_area->draw_offset = position_S_4D_struct(0,0,0,0);
	text_area->dimensions  = dimensions_S_struct(0,0);
	text_buffer_init(&text_area->gl_text,
		gl_text_atlas_addr_from(states));

	text_area->text        = temp_string_create("");
	if (!temp_string_valid(&text_area->text))
		goto could_not_init_text_value_space;

	return true;
could_not_init_text_value_space:
	return false;
}

bool myy_widget_text_area_add(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states,
	myy_widget_text_area_add_args_t * __restrict const args)
{
	struct myy_widget_text_area * __restrict const
		text_area = &widget->text_area;

	myy_widget_text_area_init(widget, states);

	myy_widget_text_area_value_set(text_area, args->text);
	return true;
}


