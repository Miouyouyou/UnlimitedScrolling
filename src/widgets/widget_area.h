#ifndef MYY_SRC_WIDGETS_WIDGET_AREA_H
#define MYY_SRC_WIDGETS_WIDGET_AREA_H 1

#include "myy/helpers/position.h"
#include "myy/helpers/vector.h"
#include <stdbool.h>

struct widget_area {
	position_S upper_left;
	position_S bottom_right;
};

typedef struct widget_area widget_area_t;

__attribute__((unused))
static inline bool widget_area_contains(
	widget_area_t const area,
	position_S const pos)
{
	return
		(area.upper_left.x <= pos.x)
		& (pos.x <= area.bottom_right.x)
		& (area.upper_left.y <= pos.y)
		& (pos.y <= area.bottom_right.y);
}

myy_vector_template(widget_areas, widget_area_t)

__attribute__((unused))
static inline widget_area_t widget_area_struct_ulbr_S(
	position_S const upper_left,
	position_S const bottom_right)
{
	widget_area_t area = {
		.upper_left = upper_left,
		.bottom_right = bottom_right
	};
	return area;
}

#define widget_areas_for_each_containing(areas_name, area_name, posS, i_name, ...) {\
	widget_area_t * __restrict const areas_data = \
		myy_vector_widget_areas_data(areas_name);\
	uintmax_t const areas_count =\
		myy_vector_widget_areas_length(areas_name);\
	for (uintmax_t i_name = 0; i_name < areas_count; i++) {\
		widget_area_t area_name = areas_data[i]; \
		if (widget_area_contains(area_name, posS)) __VA_ARGS__\
	}\
}\

__attribute__((unused))
static inline void widget_areas_dump(
	myy_vector_widget_areas * __restrict const areas)
{
	myy_vector_for_each_ptr(
		widget_area_t, area, in, areas,
		{
			LOG("[AREA] upper_left (%d,%d) - (%d,%d) bottom_right\n",
				area->upper_left.x, area->upper_left.y,
				area->bottom_right.x, area->bottom_right.y);
		}
	);
}

#endif
