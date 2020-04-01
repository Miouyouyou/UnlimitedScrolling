#include "src/widgets/lateral_menu.h"

#include "src/widgets/simple_forms.h"
#include "src/widgets/menu_forms.h"
#include "src/widgets/text_buffer.h"
#include "myy/current/opengl.h"
#include "myy/helpers/random.h"

#include "globals.h"

#include <stdbool.h>

#define SIDEPANE_MENU_SIZE 500
#define MARGIN 24 /* px */

/*struct simple_forms forms;
	struct text_buffer text;
	position_S_4D position;
	unsigned int height;
	unsigned int width;
	unsigned int layer;
	bool showing;
	char const * __restrict title;
	char const * __restrict subtitle;
	myy_vector_widget_areas widget_areas;
	myy_vector_menu_widgets menu_widgets;*/

void sidepane_right_menu_cleanup(
	struct sidepane_menu * __restrict const menu,
	myy_states * __restrict const states)
{
	menu->showing = false;

	sidepane_right_menu_reset(menu, states);
	simple_forms_cleanup(&menu->forms, states);
	text_buffer_cleanup(&menu->text, states);
}

void sidepane_right_menu_init(
	struct sidepane_menu * __restrict const menu,
	struct gl_text_infos * __restrict const text_atlas_props,
	unsigned int const screen_width,
	unsigned int const screen_height,
	unsigned int layer)
{
	position_S_4D
		menu_position = position_S_4D_struct(
			screen_width-SIDEPANE_MENU_SIZE, 0, 15, 0);
	simple_forms_init(&menu->forms);
	simple_forms_set_draw_offset(&menu->forms, menu_position);
	text_buffer_init(&menu->text, text_atlas_props);
	text_buffer_set_draw_offset(&menu->text, menu_position);

	menu->position = menu_position;
	LOG("[SIDEPANE_RIGHT_MENU_INIT]\n"
		"\tMenu position : {.x = %d, .y = %d, .z = %d, .w = %d}\n",
		menu_position.x, menu_position.y,
		menu_position.z, menu_position.w);
	menu->height  = screen_height;
	menu->width   = SIDEPANE_MENU_SIZE;
	menu->layer   = layer;
	menu->showing = false;
	menu->title   = "";
	menu->subtitle = (char const *) 0;
}

void sidepane_right_menu_set_title(
	struct sidepane_menu * __restrict const menu,
	char const * __restrict const title)
{
	menu->title = title;
}

void sidepane_right_menu_set_subtitle(
	struct sidepane_menu * __restrict const menu,
	char const * __restrict const subtitle)
{
	menu->subtitle = subtitle;
}

void sidepane_right_menu_reset(
	struct sidepane_menu * __restrict const menu,
	myy_states * __restrict const states)
{
	myy_vector_widget_areas_free_content(menu->widget_areas);
	myy_vector_for_each_ptr(
		union myy_menu_widget, widget, in, &menu->menu_widgets,
		{
			widget->generic.ops->cleanup(widget, states);
		}
	);
	myy_vector_menu_widgets_free_content(menu->menu_widgets);
	simple_forms_reset(&menu->forms);
	text_buffer_reset(&menu->text);
}

void sidepane_right_menu_add_list(
	sidepane_menu_t * __restrict const menu,
	char const * __restrict const list_start,
	size_t n_elements)
{

	char const * __restrict cursor = list_start;
	while(n_elements--) {
		size_t const string_size = strlen(cursor);
		cursor += string_size+sizeof('\0');
	}
}

void sidepane_right_menu_refresh(
	struct sidepane_menu * __restrict const menu,
	myy_states * __restrict const states)
{
	/*menu_forms_add_rectangle_3D_upper_left(
		&menu->forms,
		position_S_3D_struct(0,0,0),
		dimensions_S_struct(menu->width, menu->height),
		rgba8_black_opaque());*/

	rgba8_t const white = rgba8_white_opaque();
	simple_forms_add_arrow_left(&menu->forms,
		position_S_3D_struct(0, 52, 1),
		white);
	simple_forms_add_arrow_left(&menu->forms,
		position_S_3D_struct(0, 52, 0),
		rgba8_color(128,128,128,255));

	position_S_4D pos =
		position_S_4D_struct(52, 16, menu->layer, 0);
	text_buffer_add_string_colored(
		&menu->text,
		(uint8_t const *) (menu->title),
		(position_S_3D *) &pos,
		white);
	if (menu->subtitle) {
		pos.x = 52;
		// TODO Make text_buffer display things from the upper left corner
		text_buffer_add_string_colored(
			&menu->text,
			(uint8_t const *) (menu->subtitle),
			(position_S_3D *) &pos,
			rgba8_color(0,128,128,255));
		pos.y += 16;
	}

	sidepane_right_menu_store_to_gpu(menu);

	position_S_4D draw_position_abs = menu->position;
	draw_position_abs.y += pos.y + MARGIN;

	myy_vector_for_each_ptr(
		union myy_menu_widget, widget, in, &menu->menu_widgets,
		{
			struct myy_menu_widget_ops * __restrict const widget_ops =
				widget->button.ops;
			dimensions_S const widget_dimensions =
				widget_ops->prepare(widget, states);
			/* TODO Do this in set_position, using every member
			 * dimensions... ? */
			widget_ops->set_position(widget, states, draw_position_abs);

			widget_area_t const area = {
				.upper_left = {draw_position_abs.x, draw_position_abs.y},
				.bottom_right = {
					draw_position_abs.x+widget_dimensions.width,
					draw_position_abs.y+widget_dimensions.height
				}
			};
			myy_vector_widget_areas_add(&menu->widget_areas, 1, &area);

			draw_position_abs.y += widget_dimensions.height;
		}
	);
}

void sidepane_right_menu_store_to_gpu(
	struct sidepane_menu * __restrict const menu)
{
	simple_forms_store_to_gpu(&menu->forms);
	text_buffer_store_to_gpu(&menu->text);
}

void sidepane_right_menu_draw_opaque(
	struct sidepane_menu * __restrict const menu,
	myy_states * __restrict const state,
	uint32_t const i)
{
	/* Draw front to back when drawing opaque forms */
	myy_vector_for_each_ptr(
		union myy_menu_widget, widget, in, &menu->menu_widgets,
		{
			widget->button.ops->draw_opaque(widget, state, 0);
		}
	);
	simple_forms_set_draw_offset(&menu->forms, menu->position);
	simple_forms_draw(&menu->forms);
}

void sidepane_right_menu_draw_transparent(
	struct sidepane_menu * __restrict const menu,
	myy_states * __restrict const state,
	uint32_t const i)
{
	text_buffer_set_draw_offset(&menu->text, menu->position);
	text_buffer_draw(&menu->text);
	myy_vector_for_each_ptr(
		union myy_menu_widget, widget, in, &menu->menu_widgets,
		{
			widget->button.ops->draw_transparent(widget, state, 0);
		}
	);
}

void sidepane_right_menu_add_button(
	struct sidepane_menu * __restrict const menu,
	myy_states * __restrict const states,
	char const * __restrict const title,
	myy_widget_button_action_ptr action,
	void * const action_arg)
{
	struct myy_widget_button_add_arguments args = {
		.text = title,
		.action = action,
		.action_arg = action_arg
	};

	myy_vector_menu_widgets_add_empty(&menu->menu_widgets, 1);

	union myy_menu_widget * __restrict const button_widget =
		myy_vector_menu_widgets_last(&menu->menu_widgets);

	myy_widget_button_add(button_widget, states, &args);
}

void sidepane_right_menu_handle_click(
	struct sidepane_menu * __restrict const menu,
	myy_states * __restrict states,
	position_S const absolute_pos,
	enum myy_input_events event_type,
	union myy_input_event_data const * __restrict const event_data)
{
	myy_vector_widget_areas * __restrict const areas_vector =
		&menu->widget_areas;
	uint32_t n_widgets = myy_vector_widget_areas_length(areas_vector);
	widget_area_t const * __restrict const areas =
		myy_vector_widget_areas_data(areas_vector);
	for (uint32_t i = 0; i < n_widgets; i++) {
		widget_area_t const area = areas[i];
		if (widget_area_contains(area, absolute_pos)) {
			union myy_menu_widget * __restrict const widget =
				myy_vector_menu_widgets_at_ptr(&menu->menu_widgets, i);
			widget->button.ops->handle_click(
				widget, states, absolute_pos, event_type, event_data);
		}
	}
}

struct text_buffer * sidepane_right_menu_text_buffer(
	sidepane_menu_t * __restrict const menu)
{
	return &menu->text;
}

myy_menu_widget_t * sidepane_right_menu_add_widget(
	sidepane_menu_t * __restrict const menu,
	myy_states * __restrict const states,
	myy_menu_widget_type_t const widget_type,
	void * add_args)
{
	LOG("PWOUIP\n");
	myy_menu_widget_generic_add_handler const handler =
		myy_menu_widget_add_handler_from_type(widget_type);
	myy_vector_menu_widgets_ensure_enough_space_for(
		&menu->menu_widgets, 1);
	union myy_menu_widget * __restrict const widget_space =
		myy_vector_menu_widgets_tail_ptr(&menu->menu_widgets);
	LOG("Widget_space : %p\n", widget_space);
	/* Use another name like "account for X more" instead of
	 * add empty
	 */
	bool const widget_initialized_correctly =
		handler(widget_space, states, add_args);
	if (widget_initialized_correctly)
		myy_vector_menu_widgets_add_empty(&menu->menu_widgets, 1);
	LOG("PWOP!\n");

	return
		(widget_initialized_correctly ?
		widget_space : (myy_menu_widget_t *) 0);
}
