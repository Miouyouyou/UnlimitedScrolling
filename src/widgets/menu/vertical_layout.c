#include "src/widgets/menu/widgets.h"

myy_vector_menu_widgets * myy_widget_vertical_layout_widgets(
	myy_widget_vertical_layout_t * __restrict const layout)
{
	return (myy_vector_menu_widgets *) (&layout->widgets);
}

static void myy_widget_vertical_layout_draw_opaque(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states,
	uint32_t i)
{
	myy_widget_vertical_layout_t * __restrict const
		layout = &widget->vertical_layout;

	myy_vector_menu_widgets * __restrict const
		sub_widgets = myy_widget_vertical_layout_widgets(layout);
	size_t const n_widgets =
		myy_vector_menu_widgets_length(sub_widgets);

	myy_vector_menu_widgets_draw_opaque(sub_widgets, states, i);
}

static void myy_widget_vertical_layout_draw_transparent(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states,
	uint32_t i)
{
	myy_widget_vertical_layout_t * __restrict const
		layout = &widget->vertical_layout;

	myy_vector_menu_widgets * __restrict const
		widgets = myy_widget_vertical_layout_widgets(layout);
	myy_vector_menu_widgets_draw_transparent(widgets, states, i);
}

static void myy_widget_vertical_layout_handle_click(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states,
	position_S const absolute_pos,
	enum myy_input_events const event_type,
	union myy_input_event_data const * __restrict const event_data)
{
	LOG("[myy_widget_vertical_layout_handle_click] Clac !\n");
	LOG("\tScanning for %d,%d in :\n", absolute_pos.x, absolute_pos.y);

	/* TODO Factorize this instead of only the inner loop. */
	myy_widget_vertical_layout_t * __restrict const layout =
		&widget->vertical_layout;
	myy_vector_widget_areas * __restrict const areas_vector =
		&layout->widget_areas;
	myy_vector_menu_widgets * __restrict const widgets_vector =
		myy_widget_vertical_layout_widgets(layout);
	myy_menu_widget_t * __restrict const widgets =
		myy_vector_menu_widgets_data(widgets_vector);
	widget_areas_dump(areas_vector);
	myy_vector_widget_areas_inspect(areas_vector);
	widget_areas_for_each_containing(areas_vector, area, absolute_pos, i,
		{
			myy_menu_widget_t * __restrict const sub_widget =
				widgets+i;
			sub_widget->generic.ops->handle_click(
				sub_widget,
				states,
				absolute_pos,
				event_type,
				event_data);
			break;
		}
	);
}

static dimensions_S myy_widget_vertical_layout_prepare(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states)
{
	myy_widget_vertical_layout_t * __restrict const layout =
		&widget->vertical_layout;
	myy_vector_menu_widgets * __restrict const widgets_vector =
		myy_widget_vertical_layout_widgets(layout);

	dimensions_S dimensions = dimensions_S_struct(0,0);

	myy_vector_for_each_ptr(
		myy_menu_widget_t, sub_widget, in, widgets_vector,
		{
			myy_menu_widget_ops_t * __restrict const sw_ops =
				sub_widget->generic.ops;
			/* TODO Compute the maximal width used by each
			 * widget here.
			 * Generate scrolling mechanisms using this
			 * information, if the maximal width is superior
			 * to the allocated width on screen.
			 */
			dimensions_S const sub_widget_dims =
				sw_ops->prepare(sub_widget, states);
			dimensions.height += sub_widget_dims.height;
			dimensions.width = 
				(dimensions.width > sub_widget_dims.width ?
				dimensions.width : sub_widget_dims.width);
		}
	);

	layout->dimensions = dimensions;
	return dimensions;
}

static void myy_widget_vertical_layout_set_position(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states,
	position_S_4D position)
{
	myy_widget_vertical_layout_t * __restrict const layout =
		&widget->vertical_layout;
	layout->draw_offset = position;
	myy_vector_menu_widgets * __restrict const widgets =
		myy_widget_vertical_layout_widgets(layout);
	myy_vector_widget_areas * __restrict const areas =
		&layout->widget_areas;
	myy_vector_widget_areas_reset(areas);
	/* FIXME Without that, the sub widgets would not be positioned
	 * correctly...
	 * As said in the TODO, it might be better to pass the
	 * position directly during the draw calls.
	 * Remove this if the position is passed during draw
	 * calls...
	 */
	/* TODO See about passing the position during the
	 * draw phase. That should make scrolling smoother
	 * by only using the updated position when drawing,
	 * and avoid calling "set_position" every ms
	 * when doing animated scrolling.
	 * This can also avoid calling set_position on
	 * widgets that would not be drawn. This is a
	 * particulary useful optimization for very long
	 * list of elements.
	 */
	/* The code split between prepare and set_position
	 * requires "prepare" to be called before "set_position".
	 * This is... weird... and should be documented.
	 */
	myy_vector_for_each_ptr(
		myy_menu_widget_t, sub_widget, in, widgets,
		{
			sub_widget->generic.ops->set_position(
				sub_widget, states, position);
			dimensions_S const dimensions =
				sub_widget->generic.dimensions;

			widget_area_t const widget_area = {
				.upper_left = {position.x, position.y},
				.bottom_right = {
					position.x+dimensions.width,
					position.y+dimensions.height}
			};
			myy_vector_widget_areas_add(areas, 1, &widget_area);
			
			position.y += dimensions.height;
		}
	);
}

static void myy_widget_vertical_layout_cleanup(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states)
{
	myy_widget_vertical_layout_t * __restrict const layout =
		&widget->vertical_layout;
	myy_vector_menu_widgets * __restrict const widgets_vector =
		myy_widget_vertical_layout_widgets(layout);
	/* TODO Factorize - myy_widgets_cleanup */
	myy_vector_for_each_ptr(
		myy_menu_widget_t, sub_widget, in, widgets_vector,
		{
			sub_widget->generic.ops->cleanup(sub_widget, states);
		}
	);
}

static myy_menu_widget_ops_t myy_widget_vertical_layout_ops = 
	myy_menu_widget_ops_for(myy_widget_vertical_layout);

bool myy_widget_vertical_layout_init(
	struct myy_widget_vertical_layout * __restrict const layout,
	myy_states * __restrict const states)
{
	layout->ops = &myy_widget_vertical_layout_ops;
	layout->dimensions = dimensions_S_struct(0,0);
	layout->draw_offset = position_S_4D_struct(0,0,0,0);
	myy_vector_menu_widgets * __restrict const sub_widgets =
		myy_widget_vertical_layout_widgets(layout);

	*sub_widgets = myy_vector_menu_widgets_init(8);
	if (!myy_vector_menu_widgets_is_valid(sub_widgets))
		goto could_not_allocate_memory_for_sub_widgets;

	layout->widget_areas = myy_vector_widget_areas_init(8);
	if (!myy_vector_widget_areas_is_valid(&layout->widget_areas))
		goto could_not_allocate_memory_for_widget_areas;

	return true;

	/* Unreachable 
	myy_vector_widget_areas_free_content(layout->widget_areas);*/
could_not_allocate_memory_for_widget_areas:
	myy_vector_menu_widgets_free_content(*sub_widgets);
could_not_allocate_memory_for_sub_widgets:
	return false;
}



bool myy_widget_vertical_layout_append_widgets_defs_array(
	myy_widget_vertical_layout_t * __restrict const layout,
	myy_states * __restrict const states,
	myy_menu_widget_definition_t const * __restrict definition,
	size_t n_widgets)
{
	myy_vector_menu_widgets * __restrict const sub_widgets =
		myy_widget_vertical_layout_widgets(layout);

	bool const got_enough_space =
		myy_vector_menu_widgets_ensure_enough_space_for(
			sub_widgets, n_widgets);
	if (!got_enough_space)
		goto not_enough_space_for_sub_widgets;

	myy_menu_widget_t * __restrict sub_widget_space =
		myy_vector_menu_widgets_tail_ptr(sub_widgets);
	while(n_widgets--) {

		myy_menu_widget_generic_add_handler add_func =
			myy_menu_widget_add_handler_from_type(definition->type);

		LOG("[myy_widget_vertical_layout_append_widgets] Adding a %d\n",
			definition->type);

		bool const added = add_func(
			sub_widget_space, states, (void *) &definition->args);
		if (!added)
			goto some_widgets_could_not_be_added;

		myy_vector_menu_widgets_add_empty(sub_widgets, 1);
		sub_widget_space++;
		definition++;
	}

	return true;

some_widgets_could_not_be_added:
	LOG("Could not add a subwidget :C\n");
	/* Remove added ones and bail out */
	myy_vector_menu_widgets_cleanup(sub_widgets, states);
not_enough_space_for_sub_widgets:
	return false;
}

bool myy_widget_vertical_layout_add(
	struct myy_widget_vertical_layout * __restrict const layout,
	myy_states * __restrict const states,
	myy_widget_vertical_layout_add_args_t * __restrict const args)
{
	LOG("------------------------\n");

	bool const initialized =
		myy_widget_vertical_layout_init(layout, states);
	if (!initialized)
		goto could_not_initialize_vertical_layout;

	myy_menu_widget_definition_t const * __restrict definition =
		args->widgets;
	uintmax_t n_widgets = args->n_widgets;

	bool const sub_widgets_added =
		myy_widget_vertical_layout_append_widgets_defs_array(
			layout, states, definition, n_widgets);
	if (!sub_widgets_added)
		goto could_not_add_sub_widgets;

	return true;

could_not_add_sub_widgets:
	LOG("ショボーン\n");
	/* TODO
	 * Should we nuke everything because some widgets could not
	 * be added ? 
	 */
	myy_widget_vertical_layout_cleanup(
		(myy_menu_widget_t * __restrict) layout, states);
could_not_initialize_vertical_layout:
	return false;
}

