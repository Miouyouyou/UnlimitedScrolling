#ifndef MYY_WIDGETS_LATERAL_MENU_H
#define MYY_WIDGETS_LATERAL_MENU_H 1

#include "src/widgets/simple_forms.h"
#include "src/widgets/menu_forms.h"
#include "src/widgets/text_buffer.h"

#include "myy.h"

#include "globals.h"

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

struct sidepane_menu {
	struct menu_forms forms;
	struct text_buffer text;
	unsigned int height;
	unsigned int width;
	unsigned int layer;
	bool showing;
};

void sidepane_right_menu_init(
	struct sidepane_menu * __restrict const menu,
	struct gl_text_infos * __restrict const text_atlas_props,
	unsigned int const screen_width,
	unsigned int const screen_height,
	unsigned int layer);
void sidepane_right_menu_reset(
	struct sidepane_menu * __restrict const menu);
void sidepane_right_menu_prepare(
	struct sidepane_menu * __restrict const menu,
	char const * __restrict const title);
void sidepane_right_menu_store_to_gpu(
	struct sidepane_menu * __restrict const menu);
void sidepane_right_menu_draw(
	struct sidepane_menu * __restrict const menu,
	myy_states * __restrict const program_states);
static inline void sidepane_right_menu_draw_cb(
	void * __restrict const menu,
	myy_states * __restrict const program_states)
{
	sidepane_right_menu_draw(
		(struct sidepane_menu * __restrict) menu,
		program_states);
}

#ifdef __cplusplus
}
#endif

#endif
