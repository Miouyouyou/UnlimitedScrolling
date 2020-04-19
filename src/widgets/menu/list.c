#include "src/widgets/menu/widgets.h"

#define PADDING_LEFT   (0)
#define PADDING_RIGHT  (0)
#define PADDING_TOP    (0)
#define PADDING_BOTTOM (0)

static myy_vector_menu_widgets * myy_widget_list_get_children(
	struct myy_widget_list * __restrict const list)
{
	return (myy_vector_menu_widgets *) (&list->children_widgets);
}

static void myy_widget_list_draw_opaque(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states,
	uint32_t i)
{
	struct myy_widget_list * __restrict const list =
		&widget->list;
	myy_vector_menu_widgets * const children =
		myy_widget_list_get_children(list);
	myy_vector_for_each_ptr(
		union myy_menu_widget, child, in, children,
		{
			child->generic.ops->draw_opaque(child, states, i);
		}
	);
	simple_forms_set_draw_offset(&list->gl_forms, list->draw_offset);
	simple_forms_draw(&list->gl_forms);
}

static void  myy_widget_list_draw_transparent(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states,
	uint32_t i)
{
	struct myy_widget_list * __restrict const list =
		&widget->list;
	// The list currently don't have any transparent parts
	// itself. The children widgets might, though.
	myy_vector_menu_widgets * const children =
		myy_widget_list_get_children(list);
	myy_vector_for_each_ptr(
		union myy_menu_widget, child, in, children,
		{
			child->generic.ops->draw_transparent(child, states, i);
		}
	);
}

static void myy_widget_list_handle_click(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states,
	position_S const absolute_pos,
	enum myy_input_events const event_type,
	union myy_input_event_data const * __restrict const event_data)
{
	struct myy_widget_list * __restrict const
		list         = &widget->list;
	myy_vector_widget_areas * __restrict const
		areas_vector = &list->children_widgets_areas;
	myy_vector_menu_widgets * __restrict const
		widgets      = myy_widget_list_get_children(list);
	/* To factorize with lateral_menu.c */
	uint32_t const
		n_widgets    = myy_vector_widget_areas_length(areas_vector);
	widget_area_t const * __restrict const
		areas        = myy_vector_widget_areas_data(areas_vector);

	LOG("Mooh\n");
	for (uint32_t i = 0; i < n_widgets; i++) {
		widget_area_t const area = areas[i];
		if (widget_area_contains(area, absolute_pos)) {
			union myy_menu_widget * __restrict const
				widget = myy_vector_menu_widgets_at_ptr(widgets, i);
			/*widget->button.ops->handle_click(
				widget, states, absolute_pos, event_type, event_data);*/
			list->handler(list, states, i, widget, list->user_arg);
			widget->button.ops->handle_click(
				widget, states, absolute_pos, event_type, event_data);
			break;
		}
	}
}
static void  myy_widget_list_set_position(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states,
	position_S_4D const position)
{
	struct myy_widget_list * __restrict const list =
		&widget->list;
	list->draw_offset = position;
}
static dimensions_S myy_widget_list_prepare(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states)
{
	struct myy_widget_list * __restrict const list =
		&widget->list;

	/* TODO We should require a maximal dimension...
	 * Try to pass the maximum acceptable dimensions to
	 * the "prepare" phase function.
	 */
	position_S_4D position_actuelle =
		list->draw_offset;
	/* Our reset position */
	int16_t const marge_gauche = position_actuelle.x;
	/* Cache the widgets and their areas */
	myy_vector_menu_widgets * __restrict const
		children = myy_widget_list_get_children(list);
	myy_vector_widget_areas * __restrict const
		children_areas = &list->children_widgets_areas;
	/* Reset the click areas. We'll recreate them in the loop */
	myy_vector_widget_areas_reset(children_areas);
	/* FIXME See about using 32 bits values for dimensions...
	 * Compute the max_width... 32K can actually be too little
	 * this time...
	 */
	int16_t max_width = 0;
	myy_vector_for_each_ptr(
		union myy_menu_widget, child, in, children,
		{
			struct myy_menu_widget_ops * __restrict const ops =
				child->generic.ops;
			/* FIXME Take the position for both arguments */
			/* Remember the upper_left for the click area */
			position_S const haut_gauche = {
				marge_gauche, position_actuelle.y};
			/* Add the paddings and pass the current position
			 * to the child widget, so that it can prepare
			 * itself
			 */
			position_actuelle.x  = marge_gauche + PADDING_LEFT;
			position_actuelle.y += PADDING_TOP;
			ops->set_position(child, states, position_actuelle);
			dimensions_S const
				child_dimensions = ops->prepare(child, states);
			/* Compute the maximal width for scrolling and/or
			 * enclosure purposes.
			 * Determine the actual click area of this widget.
			 * WARNING Don't limit this to the maximal width
			 * of the list. Just remember the dimensions of
			 * the list and the local scrolling offset when
			 * computing click coordinates in the click
			 * handler.
			 */
			position_actuelle.y +=
				child_dimensions.height + PADDING_BOTTOM;
			int16_t const
				current_width = 
					PADDING_LEFT
					+ child_dimensions.width
					+ PADDING_RIGHT;
			max_width = 
				(max_width > current_width) ?
				max_width : current_width;
			position_S const bas_droite = {
				(int16_t)
				(marge_gauche+current_width), position_actuelle.y
			};
			widget_area_t const
				click_area = widget_area_struct_ulbr_S(haut_gauche, bas_droite);
			myy_vector_widget_areas_add(
				children_areas, 1, &click_area);
		}
	);

	/* The list background */
	dimensions_S const
		list_dimensions = {
			(uint16_t) max_width,
			(uint16_t) (position_actuelle.y-list->draw_offset.y)};
	simple_forms_reset(&list->gl_forms);
	simple_forms_add_rectangle_3D_upper_left(
		&list->gl_forms,
		position_S_3D_struct(0, 0, 0),
		list_dimensions,
		rgba8_black_opaque());
	simple_forms_store_to_gpu(&list->gl_forms);
	return list_dimensions;
}

static void myy_widget_list_cleanup(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states)
{
	struct myy_widget_list * __restrict const list =
		&widget->list;
	myy_vector_widget_areas_free_content(list->children_widgets_areas);
	simple_forms_cleanup(&list->gl_forms, states);
	myy_vector_free_content(list->children_widgets);
}

static struct myy_menu_widget_ops ops = {
	.draw_opaque      = myy_widget_list_draw_opaque,
	.draw_transparent = myy_widget_list_draw_transparent,
	.handle_click     = myy_widget_list_handle_click,
	.set_position     = myy_widget_list_set_position,
	.prepare          = myy_widget_list_prepare,
	.cleanup          = myy_widget_list_cleanup,
};

void myy_widget_list_add(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states,
	struct myy_widget_list_add_arguments const * __restrict const arguments)
{
	struct myy_widget_list * __restrict const list =
		&widget->list;

	list->ops         = &ops;
	list->draw_offset = position_S_4D_struct(0,0,0,0);
	list->dimensions  = dimensions_S_struct(0,0);

	simple_forms_init(&list->gl_forms);

	myy_vector_menu_widgets * __restrict const
		children_list = myy_widget_list_get_children(list);
	*children_list    = myy_vector_menu_widgets_init(16);

	list->children_widgets_areas = myy_vector_widget_areas_init(16);

	list->handler     = arguments->handler;
	list->user_arg    = arguments->user_arg;
}

static void string_list_widget_click_handler(
	void * __restrict const user_arg,
	myy_states * __restrict const states,
	struct myy_widget_button * __restrict const widget)
{
	LOG("[string_list_widget_click_handler] Dummy handler.\n");
}



/* Create a list of clickable strings, using an OpenGL way
 * for dealing with the list.
 * You'll have to pass :
 * - the address where to store the list widget;
 * - the amount of strings to parse;
 * - the start address (which is basically
 *   the address containing your character string + offset needed
 *   to access it, if needed);
 * - the 'stride', which is the amount of octets between the
 *   current address and the next element address.
 *   This can be negative if you want to sweep an array in reverse;
 * - the address of the function able to convert the elements
 *   at the provided address into C strings;
 * - the address of the first argument passed to that function;
 * - the address of the function dealing with clicks on the
 *   list, which will receive the index of the element clicked.
 */
bool myy_widget_list_add_generic_string_list(
	union myy_menu_widget * __restrict const widget,
	myy_states * __restrict const states,
	myy_widget_list_generic_strings_add_args_t * args)
{
	struct myy_widget_list_add_arguments const arguments = 
	{
		.handler  = args->list_click_handler,
		.user_arg = args->list_click_arg
	};
	myy_widget_list_add(widget, states, &arguments);

	size_t const
		n_elements = args->n_elements;
	intptr_t const
		stride     = args->stride;
	char const * (*to_string)(void * to_string_arg, uint8_t * element) =
		args->to_string;
	void * const to_string_arg = args->to_string_arg;
	struct myy_widget_list * __restrict const
		list = &widget->list;
	myy_vector_menu_widgets * __restrict const
		children = myy_widget_list_get_children(list);
	union myy_menu_widget * __restrict widget_cursor =
		myy_vector_menu_widgets_tail_ptr(children);
	myy_vector_menu_widgets_inspect(children);

	if (!myy_vector_menu_widgets_add_empty(children, n_elements)) {
		myy_widget_list_cleanup(widget, states);
		return false;
	}

	myy_widget_button_add_args_t list_element_struct_args = {
		.text = NULL,
		.action = string_list_widget_click_handler,
		.action_arg = list
	};

	uint8_t * __restrict
		current_element = args->start_address;

	for (size_t i = 0; i < n_elements; i++) {
		list_element_struct_args.text =
			to_string(to_string_arg, current_element);
		myy_widget_button_add(
			widget_cursor, states, &list_element_struct_args);
		widget_cursor++;
		current_element += stride;
	}

	return true;
}
