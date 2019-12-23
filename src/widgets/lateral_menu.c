#include "src/widgets/lateral_menu.h"

#include "src/widgets/simple_forms.h"
#include "src/widgets/menu_forms.h"
#include "src/widgets/text_buffer.h"
#include "myy/current/opengl.h"

#define SIDEPANE_MENU_SIZE 500

void sidepane_right_menu_init(
	struct sidepane_menu * __restrict const menu,
	struct gl_text_infos * __restrict const text_atlas_props,
	unsigned int const screen_width,
	unsigned int const screen_height,
	unsigned int layer)
{
	position_S_4D menu_position =
		position_S_4D_struct(screen_width-SIDEPANE_MENU_SIZE, 0, 15, 0);
	menu_forms_init(&menu->forms);
	menu_forms_set_global_position(&menu->forms, menu_position);
	text_buffer_init(&menu->text, text_atlas_props);
	text_buffer_set_global_position(&menu->text, menu_position);
		
	menu->height = screen_height;
	menu->width = SIDEPANE_MENU_SIZE;
	menu->layer = layer;
	menu->showing = false;
}

void sidepane_right_menu_reset(
	struct sidepane_menu * __restrict const menu)
{
	menu_forms_reset(&menu->forms);
	text_buffer_reset(&menu->text);
}

void sidepane_right_menu_prepare(
	struct sidepane_menu * __restrict const menu,
	char const * __restrict const title)
{
	menu_forms_add_rectangle(&menu->forms,
		position_S_struct(0, menu->height),
		dimensions_S_struct(menu->width, menu->height),
		rgba8_color(0,0,0,255));
	struct rgba8 const white = rgba8_color(255,255,255,255);
	menu_forms_add_arrow_left(&menu->forms,
		position_S_3D_struct(0, 52, 0),
		white);
	menu_forms_add_arrow_left(&menu->forms,
		position_S_3D_struct(0, 52, 0),
		rgba8_color(128,128,128,255));

	position_S_3D pos =
		position_S_3D_struct(52, 52, menu->layer);
	text_buffer_add_string_colored(
		&menu->text,
		(uint8_t const * __restrict) title,
		&pos,
		white);
}

void sidepane_right_menu_store_to_gpu(
	struct sidepane_menu * __restrict const menu)
{
	menu_forms_store_to_gpu(&menu->forms);
	text_buffer_store_to_gpu(&menu->text);
}

void sidepane_right_menu_draw(
	struct sidepane_menu * __restrict const menu,
	myy_states * __restrict const state)
{
	menu_forms_draw(&menu->forms);
	text_buffer_draw(&menu->text);
}
