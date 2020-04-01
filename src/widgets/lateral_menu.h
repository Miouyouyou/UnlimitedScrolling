#ifndef MYY_WIDGETS_LATERAL_MENU_H
#define MYY_WIDGETS_LATERAL_MENU_H 1

#include "src/widgets/simple_forms.h"
#include "src/widgets/menu_forms.h"
#include "src/widgets/text_buffer.h"

#include "myy.h"

#include "globals.h"

#include "src/widgets/widget_area.h"
#include "src/widgets/menu/widgets.h"

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif


struct sidepane_menu {
	struct simple_forms forms;
	struct text_buffer text;
	position_S_4D position;
	unsigned int height;
	unsigned int width;
	unsigned int layer;
	bool showing;
	char const * __restrict title;
	char const * __restrict subtitle;
	myy_vector_widget_areas widget_areas;
	myy_vector_menu_widgets menu_widgets;
};

typedef struct sidepane_menu sidepane_menu_t;

enum myy_widget_input_type {
	myy_widget_input_type_invalid,
	myy_widget_input_type_text,
	myy_widget_input_type_numeric
};

__attribute__((unused))
static inline int layer_next_front(unsigned int layer) {
	return layer - 1;
}

void sidepane_right_menu_init(
	struct sidepane_menu * __restrict const menu,
	struct gl_text_infos * __restrict const text_atlas_props,
	unsigned int const screen_width,
	unsigned int const screen_height,
	unsigned int layer);
void sidepane_right_menu_cleanup(
	struct sidepane_menu * __restrict const menu,
	myy_states * __restrict const states);
void sidepane_right_menu_reset(
	struct sidepane_menu * __restrict const menu,
	myy_states * __restrict const states);
void sidepane_right_menu_set_title(
	struct sidepane_menu * __restrict const menu,
	char const * __restrict const title);
void sidepane_right_menu_set_subtitle(
	struct sidepane_menu * __restrict const menu,
	char const * __restrict const subtitle);
void sidepane_right_menu_refresh(
	struct sidepane_menu * __restrict const menu,
	myy_states * __restrict const states);
void sidepane_right_menu_store_to_gpu(
	struct sidepane_menu * __restrict const menu);
void sidepane_right_menu_draw_opaque(
	struct sidepane_menu * __restrict const menu,
	myy_states * __restrict const state,
	uint32_t const i);
void sidepane_right_menu_draw_transparent(
	struct sidepane_menu * __restrict const menu,
	myy_states * __restrict const state,
	uint32_t const i);
void sidepane_right_menu_add_button(
	struct sidepane_menu * __restrict const menu,
	myy_states * __restrict const states,
	char const * __restrict const title,
	myy_widget_button_action_ptr action,
	void * const action_arg);
void sidepane_right_menu_add_input(
	struct sidepane_menu * __restrict const menu,
	char const * __restrict const label);
static inline void sidepane_right_menu_draw_cb(
	void * __restrict const menu,
	myy_states * __restrict const program_states)
{
	sidepane_right_menu_draw_opaque(
		(struct sidepane_menu * __restrict) menu,
		program_states, 0);
	sidepane_right_menu_draw_transparent(
		(struct sidepane_menu * __restrict) menu,
		program_states, 0);
}
void sidepane_right_menu_handle_click(
	struct sidepane_menu * __restrict const menu,
	myy_states * __restrict states,
	position_S const absolute_pos,
	enum myy_input_events event_type,
	union myy_input_event_data const * __restrict const event_data);
myy_menu_widget_t * sidepane_right_menu_add_widget(
	sidepane_menu_t * __restrict const menu,
	myy_states * __restrict const states,
	myy_menu_widget_type_t const widget_type,
	void * add_args);

#ifdef __cplusplus
}
#endif

#endif
