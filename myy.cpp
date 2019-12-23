#include <myy/myy.h>
#include <myy/current/opengl.h>
#include <myy/helpers/opengl/loaders.h>
#include <myy/helpers/opengl/shaders_pack.h>
#include <myy/helpers/fonts/packed_fonts_parser.h>
#include <myy/helpers/fonts/packed_fonts_display.h>
#include <myy/helpers/matrices.h>

#include <myy/helpers/opengl/buffers.h>

#include <myy/helpers/position.h>
#include <myy/helpers/dimensions.h>

#include <menu_parts.h>

#include <string.h>

#include "shaders.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include <src/widgets/simple_forms.h>
#include <src/widgets/menu_forms.h>
#include <src/widgets/text_buffer.h>
#include <src/widgets/stencil.h>
#include <src/widgets/text_area.h>
#include <src/widgets/common_types.h>
#include <src/widgets/lateral_menu.h>

#include <myy/helpers/myy_vector.hpp>
#include <src/widgets/list.hpp>

#include <src/widgets/menu.hpp>

/**
 * List widgets :
 * - myy_text_area : Text input widget
 *   * Editing is done with an editor module.
 *     The whole idea is that the module implementation
 *     differs from platform to platform.
 *     (e.g. Android text module editor is completely
 *      different from the X11 one).
 * - 
 */

#include <myy_data.h>

struct myy_shaders_db myy_programs;
struct menu_parts_handler menu_handler;
struct myy_text_area area;



static struct myy::user_state state;

#include <myy/helpers/position.h>

using namespace myy;

static void point_test1(Triangle<Point2D<uint16_t>> a)
{
	a.log(std::cerr);
	std::cerr << "Size of triangle : " << sizeof(a) << "\n";
}

static void point_test2(Point2D<uint16_t> const * __restrict const b)
{
	b->log(std::cerr);
}

extern "C" {
	int myy_init(
		myy_states * __restrict const states,
		int argc,
		char **argv,
		struct myy_window_parameters * __restrict const window)
	{
		myy::MenuForms a = myy::MenuForms();
		states->user_state = &state;
		window->height = 720;
		window->width  = 1280;
		window->title  = "Meow";

		point_test1({{1,2},{3,4},{5,6}});
		Point2D<uint16_t> b {3,4};
		point_test2(&b);
		return 0;
	};
}

#include <iostream>

static void dump_myy_programs()
{
	std::cerr <<
		"GLuint standard_id                    = " << myy_programs.standard_id                    << "\n" <<
		"GLint  standard_unif_projection       = " << myy_programs.standard_unif_projection       << "\n" <<
		"GLint  standard_unif_tex_projection   = " << myy_programs.standard_unif_tex_projection   << "\n" <<
		"GLint  standard_unif_tex              = " << myy_programs.standard_unif_tex              << "\n" <<
		"GLuint text_id                        = " << myy_programs.text_id                        << "\n" <<
		"GLint  text_unif_projection           = " << myy_programs.text_unif_projection           << "\n" <<
		"GLint  text_unif_texture_projection   = " << myy_programs.text_unif_texture_projection   << "\n" <<
		"GLint  text_unif_text_offset          = " << myy_programs.text_unif_text_offset          << "\n" <<
		"GLint  text_unif_global_offset        = " << myy_programs.text_unif_global_offset        << "\n" <<
		"GLint  text_unif_fonts_texture        = " << myy_programs.text_unif_fonts_texture        << "\n" <<
		"GLint  text_unif_rgb                  = " << myy_programs.text_unif_rgb                  << "\n" <<
		"GLuint simple_stencil_id              = " << myy_programs.simple_stencil_id              << "\n" <<
		"GLint  simple_stencil_unif_projection = " << myy_programs.simple_stencil_unif_projection << "\n" <<
		"GLuint menu_forms_id                  = " << myy_programs.menu_forms_id                  << "\n" <<
		"GLint  menu_forms_unif_projection     = " << myy_programs.menu_forms_unif_projection     << "\n" <<
		"GLint  menu_forms_unif_global_offset  = " << myy_programs.menu_forms_unif_global_offset  << "\n" <<
		"GLuint lines_id                       = " << myy_programs.lines_id                       << "\n" <<
		"GLint  lines_unif_projection          = " << myy_programs.lines_unif_projection          << "\n" <<
		"GLint  lines_unif_global_offset       = " << myy_programs.lines_unif_global_offset       << "\n"; 
}

#include <lib/QuickAssembler/assembler.h>

MainBoard board;

static void init_assembler(
	myy_states * __restrict states)
{
	auto assembler_state =
		&user_state::from(states)->assembler_state;
	bool const valid_collections = 
		global_state_setup(assembler_state, 1);

	/*if (!valid_collections)
		goto invalid_collections;*/

	instructions_collection_t * __restrict const collection =
		instructions_collections_add(
			&assembler_state->collections, assembler_state, "ARM");

	/*if (collection == NULL)
		goto invalid_collection;*/

	myy_vector_instruction * __restrict const instructions =
		&collection->instructions;
	instruction_t * __restrict const mov = instructions_add(
		instructions,
		assembler_state,
		"MOV",
		0);
	/*if (mov == NULL)
		goto invalid_instruction;*/

	/*instruction_field_t * __restrict const dst =*/
	instruction_add_field(mov, assembler_state, "Register DST", 13, 5);
	/*instruction_field_t * __restrict const src =*/
	instruction_add_field(mov, assembler_state, "Register SRC", 4, 5);
	/*instruction_field_t * __restrict const shift =*/
	instruction_add_field(mov, assembler_state, "Shift", 0, 2);

	instruction_t * __restrict const adc = instructions_add(
		instructions,
		assembler_state,
		"ADC 64 bits",
		0b1001101000000000000000000000);

	instruction_add_field(adc, assembler_state, "Register DST", 0, 5);
	instruction_add_field(adc, assembler_state, "Register OP1", 5, 10);
	instruction_add_field(adc, assembler_state, "Register OP2", 16, 21);

	uint64_t field_values[] = {27, 31, 3};
	/*myy_vector_uint64 arguments = myy_vector_uint64_init(4);
	myy_vector_uint64_add(
		&arguments,
		sizeof(field_values)/sizeof(uint64_t),
		field_values);
	uint64_t assembled_instruction =
		instruction_assemble(mov, &arguments);*/

	/*string_data_t name = strings_get(assembler_state, mov->name_id);*/

	/*instruction_dump_infos_with_values(
		mov,
		assembler_state,
		&arguments);*/

	uint64_t second_values[] = {1,24,31};

	board.init(states, {0,0,3,0});
	auto functionnode = InstructionNode(1, "Ä国をÔ");
	functionnode.add(mov, dynarray<uint64_t>(field_values, 3));
	functionnode.add(adc, dynarray<uint64_t>(second_values, 3));
	board.show(states, &functionnode, {0,0,2,0});

	field_values[0] = 12;
	field_values[2] = 5;
	auto functionnode2 = InstructionNode(2, "俺のターン！");
	functionnode2.add(adc, dynarray<uint64_t>(field_values, 3));
	board.show(states, &functionnode2, {300,300,3,0});

	return;
/*
invalid_instruction:
	myy_vector_instruction_free_content(collection->instructions);
invalid_collection:
	// FIXME Create a delete function for major types.
	// Let the delete functions handle the internals...
	myy_vector_instructions_collection_free_content(state.collections);
	myy_vector_utf8_free_content(state.strings.memory_space);
invalid_collections:
	return;
	*/
}

static void init_text_atlas(
	myy_states * __restrict states,
	uintreg_t surface_width,
	uintreg_t surface_height)
{
	myy_shaders_pack_load_all_programs_from_file(
		"data/shaders.pack",
		(uint8_t * __restrict) &myy_programs);
	LOG("myy_programs location : %p\n", &myy_programs);


	struct gl_text_infos * __restrict const loaded_infos =
		&user_state::from(states)->gl_text_atlas;
	struct myy_sampler_properties properties =
		myy_sampler_properties_default();

	glActiveTexture(GL_TEXTURE4);
	myy_packed_fonts_load(
		"data/font_pack_meta.dat", loaded_infos, NULL, &properties);

	glClearColor(GLOBAL_BACKGROUND_COLOR);

	float inv_tex_width  = 1.0f/loaded_infos->tex_width;
	float inv_tex_height = 1.0f/loaded_infos->tex_height;

	glUseProgram(myy_programs.text_id);
	glUniform1i(
		myy_programs.text_unif_fonts_texture,
		4);
	glUniform2f(
		myy_programs.text_unif_texture_projection,
		inv_tex_width,
		inv_tex_height);

	glEnableVertexAttribArray(text_xy);
	glEnableVertexAttribArray(text_in_st);
	
}

static void init_projections(
	myy_states * __restrict state,
	uintreg_t surface_width,
	uintreg_t surface_height)
{

	union myy_4x4_matrix matrix;
	myy_matrix_4x4_ortho_layered_window_coords(
		&matrix, surface_width, surface_height, 64);

	glUseProgram(myy_programs.text_id);
	glUniformMatrix4fv(
		myy_programs.text_unif_projection,
		1,
		GL_FALSE,
		matrix.raw_data);
	glUseProgram(myy_programs.lines_id);
	glUniformMatrix4fv(
		myy_programs.lines_unif_projection,
		1,
		GL_FALSE,
		matrix.raw_data);
	menu_forms_set_projection(&matrix);
	glUseProgram(0);
}

myy_vector_template(int16, int16_t)

static GLuint lines_buffer;
static uint32_t n_lines;
static int32_t offset_x = 0, offset_y = 0;
static int32_t offset_current_x = 0, offset_current_y = 0;
static struct sidepane_menu right_menu;


static void lines_draw()
{
	glUseProgram(myy_programs.lines_id);
	glBindBuffer(GL_ARRAY_BUFFER, lines_buffer);
	glUniform2f(myy_programs.lines_unif_global_offset,
		(float) ((offset_x + offset_current_x) & 31),
		(float) ((offset_y + offset_current_y) & 31));
	glVertexAttribPointer(
		lines_xy,
		2,
		GL_SHORT,
		GL_FALSE,
		2*sizeof(int16_t),
		(void *) (0x0));
	glDrawArrays(GL_LINES, 0, n_lines);
	glUseProgram(0);
}

static void lines_init()
{
	glUseProgram(myy_programs.lines_id);
	glEnableVertexAttribArray(lines_xy);
	glUseProgram(0);
}

static void sidemenu_prepare(
	char const * __restrict const title)
{
	sidepane_right_menu_reset(&right_menu);
	sidepane_right_menu_prepare(&right_menu, title);
	sidepane_right_menu_store_to_gpu(&right_menu);
}

static void sidemenu_show(
	myy_states * __restrict const state)
{
	/* Not sure it's the best way to handle this...
	 * Should the widget know it's being added
	 * on the screen or is it the UI manager
	 * responsibility to know the state of the UI
	 * objects shown on the screen... Hmm...
	 * 
	 * An alternative would be to check whether the
	 * element itself is being added to the draw
	 * list on each add...
	 */
	if (!right_menu.showing) {
		user_state::from(state)->draw_functions_list.add(
			sidepane_right_menu_draw_cb,
			&right_menu);
		right_menu.showing = true;
	}
}

static void sidemenu_hide(
	myy_states * __restrict const state)
{
	if (right_menu.showing) {
		user_state::from(state)->draw_functions_list.remove(
			sidepane_right_menu_draw_cb);
		right_menu.showing = false;
	}
}

void myy_init_drawing(
	myy_states * __restrict state,
	uintreg_t const surface_width,
	uintreg_t const surface_height)
{
	init_text_atlas(state, surface_width, surface_height);
	init_projections(state, surface_width, surface_height);

	std::cerr <<
		"surface_width  : " << surface_width << "\n" <<
		"surface_height : " << surface_height << "\n";


	LOG("width : %lu, height : %lu\n", surface_width, surface_height);
	myy_vector_int16 coords = myy_vector_int16_init(512);
	{
		int16_t line_left[2]  = {-128, 0};
		int16_t line_right[2] = {(int16_t) (surface_width+128), 0};
		int16_t const limit = surface_height+128;
		for (int16_t i = -128; i < limit; i += 32)
		{
			line_left[1]  = i; // { -128, i }
			line_right[1] = i; // { surface_width+128, i }
			myy_vector_int16_add(&coords, 2, line_left);
			myy_vector_int16_add(&coords, 2, line_right);
		}
		LOG("n_elements after lines : %zu\n", myy_vector_int16_length(&coords));
	}
	{
		int16_t column_up[2]   = {0, -128};
		int16_t column_down[2] = {0, (int16_t) (surface_height+128)};
		int16_t const limit = surface_width+128;
		for (int16_t i = -128; i < limit; i += 32)
		{
			column_up[0]   = i;
			column_down[0] = i;
			myy_vector_int16_add(&coords, 2, column_up);
			myy_vector_int16_add(&coords, 2, column_down);
		}
		LOG("n_elements after lines : %zu\n", myy_vector_int16_length(&coords));
	}

	/* Infinite lines */


	LOG("n_elements : %zu (%zu lines)\n",
		myy_vector_int16_length(&coords),
		myy_vector_int16_length(&coords)/2);
	glGenBuffers(1, &lines_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, lines_buffer);
	glBufferData(GL_ARRAY_BUFFER,
		myy_vector_int16_allocated_used(&coords),
		myy_vector_int16_data(&coords),
		GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	lines_init();
	n_lines = myy_vector_int16_length(&coords) / 2;

	myy_vector_int16_free_content(coords);
	glEnable(GL_DEPTH_TEST);

	auto user_state = user_state::from(state);
	sidepane_right_menu_init(
		&right_menu, &user_state->gl_text_atlas,
		surface_width, surface_height, 0);

	if (!user_state->draw_functions_list.init()) {
		LOG(
			"\n\n"
			"▲ WARNING ▲\n\n"
			"Problems while initialising the draw functions list array\n\n"
			"▲ WARNING ▲\n\n");
	}
	sidemenu_prepare("Meow");

	init_assembler(state);
	user_state->draw_functions_list.add(MainBoard::draw, &board);
	
}

void myy_draw(
	myy_states * __restrict state, 
	uintreg_t i,
	uint64_t last_frame_delta_ns)
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

	board.set_camera(offset_x + offset_current_x, offset_y + offset_current_y);
	user_state::from(state)->draw_functions_list.draw(state);
	lines_draw();
}


static uint32_t moving = 0;
static uint32_t start_x = 0, start_y = 0;
extern "C" {
	void myy_input(
		myy_states * __restrict state,
		enum myy_input_events const event_type,
		union myy_input_event_data * __restrict const event)
	{
		switch(event_type) {
			case myy_input_event_invalid:
				break;
			case myy_input_event_mouse_moved_absolute:
				if (moving) {
					offset_current_x = (event->mouse_move_absolute.x - start_x);
					offset_current_y = (event->mouse_move_absolute.y - start_y);
				}
				break;
			case myy_input_event_mouse_moved_relative:
				break;
			case myy_input_event_mouse_button_pressed:
				if (event->mouse_button.button_number == 1) {
					start_x = event->mouse_button.x;
					start_y = event->mouse_button.y;
					moving = 1;
				}
				break;
			case myy_input_event_mouse_button_released:
				if (event->mouse_button.button_number == 1) {
					moving = 0;
					offset_x += offset_current_x;
					offset_y += offset_current_y;
					offset_current_x = 0;
					offset_current_y = 0;
				}
				LOG("Mouse pressed : x: %d y: %d (%d - %d)\n",
					event->mouse_button.x,
					event->mouse_button.y,
					event->mouse_button.button_number,
					event_type);
				break;
			case myy_input_event_touch_pressed:
				start_x = event->touch.x;
				start_y = event->touch.y;
				moving = 1;
				break;
			case myy_input_event_touch_move:
				if (moving) {
					offset_current_x = (event->touch.x - start_x);
					offset_current_y = (event->touch.y - start_y);
				}
				break;
			case myy_input_event_touch_released:
				moving = 0;
				offset_x += (event->touch.x - start_x);
				offset_y += (event->touch.y - start_y);
				offset_current_x = 0;
				offset_current_y = 0;
				break;
			case myy_input_event_keyboard_key_released:
				if (event->key.raw_code == 57) {
					if (!right_menu.showing)
						sidemenu_show(state);
					else
						sidemenu_hide(state);
				}
				break;
			case myy_input_event_keyboard_key_pressed:
				LOG("KEY: %d\n", event->key.raw_code);
				if (event->key.raw_code == 1) { myy_user_quit(state); }
				break;
			case myy_input_event_text_received:
				LOG("TEXT: %s\n", event->text.data);
				break;
			case myy_input_event_surface_size_changed:
				myy_display_initialised(
					state, event->surface.width, event->surface.height);
				break;
			case myy_input_event_window_destroyed:
				myy_user_quit(state);
				break;
			default:
				break;
		}
	}



	/* Question : How does that work, from the first interaction ? */
	void myy_editor_finished(
		myy_states * __restrict const states,
		uint8_t const * __restrict const string,
		size_t const string_size)
	{
		/* Still ugly, IMHO */
		auto * __restrict const user_state = user_state::from(states);
		struct myy_text_area * __restrict const edited_text_area =
			user_state->edited.text_area;
		myy_text_area_set_text_utf8_characters(
			edited_text_area,
			string_size, string);
		user_state->edited.done_callback(states);
	}
}
