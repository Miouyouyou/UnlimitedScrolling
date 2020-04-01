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
#include <stdarg.h>

#include <sys/mman.h>

struct myy_shaders_db myy_programs;
struct menu_parts_handler menu_handler;
struct myy_text_area area;



static struct myy::user_state state;

#include <myy/helpers/position.h>

using namespace myy;

extern "C" {
	int myy_init(
		myy_states * __restrict const states,
		int argc,
		char **argv,
		struct myy_window_parameters * __restrict const window)
	{
		myy::MenuForms a = myy::MenuForms();
		states->user_state = &state;
		window->height = 900;
		window->width  = 1600;
		window->title  = "Meow";

		return 0;
	};
}

#include <lib/QuickAssembler/assembler.h>

MainBoard board;

static instructions_collection_t * __restrict test_collection =
	(instructions_collection_t *) 0;

InstructionNode sauvage = InstructionNode(3, "TestCode");
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
	test_collection = collection;

	/*if (collection == NULL)
		goto invalid_collection;*/

	myy_vector_instruction * __restrict const instructions =
		&collection->instructions;
	instruction_t * __restrict const adr = instructions_add(
		instructions,
		assembler_state,
		"ADR",
		0b00010000000000000000000000000000);
	/*if (mov == NULL)
		goto invalid_instruction;*/

	instruction_add_field(adr, assembler_state, "Dst Register", 0, 5-0);
	instruction_add_field(adr, assembler_state, "immhi", 5, 23-5);
	instruction_add_field(adr, assembler_state, "immlo", 29, 31-29);


	instruction_t * __restrict const adrp = instructions_add(
		instructions,
		assembler_state,
		"ADRP",
		0b10010000000000000000000000000000);

	instruction_add_field(adr, assembler_state, "Dst Register", 0, 5-0);
	instruction_add_field(adr, assembler_state, "immhi", 5, 23-5);
	instruction_add_field(adr, assembler_state, "immlo", 29, 31-29);

	instruction_t * __restrict const blr = instructions_add(
		instructions,
		assembler_state,
		"BLR",
		0b11010110001111110000000000000000);

	instruction_add_field(blr, assembler_state, "Branch register", 5, 10-5);

	instruction_t * __restrict const movwi = instructions_add(
		instructions,
		assembler_state,
		"MOVWI",
		0b01010010100000000000000000000000);
	instruction_add_field(
		movwi, assembler_state, "Dst Register", 0,   5-0);
	instruction_add_field(
		movwi, assembler_state, "IMM16",        5,  21-5);
	instruction_add_field(
		movwi, assembler_state, "n*16b Shift", 21, 23-21);
	instruction_add_field(
		movwi, assembler_state, "32/64",       31, 32-31);

	instruction_t * __restrict const movr = instructions_add(
		instructions,
		assembler_state,
		"MOVR",
		0b00101010000000000000001111100000);
	instruction_add_field(
		movr, assembler_state, "Dst Register",  0,   5-0);
	instruction_add_field(
		movr, assembler_state, "Src Register", 16, 21-16);
	instruction_add_field(
		movr, assembler_state, "32/64",        31, 32-31);

	instruction_t * __restrict const svc = instructions_add(
		instructions,
		assembler_state,
		"SVC",
		0b11010100000000000000000000000001);

	instruction_add_field(
		svc, assembler_state, "Call number", 5, 21-5);

	instruction_t * __restrict const ret = instructions_add(
		instructions,
		assembler_state,
		"RET",
		0b11010110010111110000000000000000);

	instruction_add_field(ret, assembler_state, "LR Reg", 5, 10-5);

	instruction_t * __restrict const adc = instructions_add(
		instructions,
		assembler_state,
		"ADC",
		0b10011010000000000000000000000000);

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
	functionnode.add(movr, dynarray<uint64_t>(field_values, 3));
	functionnode.add(adc, dynarray<uint64_t>(second_values, 3));
	board.show(states, &functionnode, {0,0,2,0});

	field_values[0] = 12;
	field_values[2] = 5;
	auto functionnode2 = InstructionNode(2, "俺のターン！");
	functionnode2.add(adc, dynarray<uint64_t>(field_values, 3));
	board.show(states, &functionnode2, {300,300,3,0});

	/* [x0: str, x1: print_func]
	 * blr x1
	 * ret
	 */
	/*sauvage.add(blr, {{2, 1, 1}});
	sauvage.add(movr, {{1, 0, 1}});
	sauvage.add(movwi, {{0, 1, 0, 1}});
	sauvage.add(movwi, {{8, 64, 0, 1}});
	sauvage.add(svc, {std::initializer_list<uint64_t>{0}});
	sauvage.add(movwi, {{0, 144, 0, 1}});
	sauvage.add(ret, {std::initializer_list<uint64_t>{30}});*/
	sauvage.add(movr, {28, 30, 1});
	sauvage.add(blr, {std::initializer_list<uint64_t>{1}});
	sauvage.add(movr, {30, 28, 1});
	sauvage.add(ret, {std::initializer_list<uint64_t>{30}});

	board.show(states, &sauvage, {600,600,4,0});
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
	simple_forms_set_projection(&matrix);
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

static void sidepane_menu_button_test(
	myy_widget_button_t * __restrict const button,
	myy_states * __restrict const states,
	void * __restrict const arg)
{
	LOG("MEOW! UL : {%d,%d}, BR : {%d,%d}\n",
		button->draw_offset.x, button->draw_offset.y,
		button->draw_offset.x+button->dimensions.width,
		button->draw_offset.y+button->dimensions.height);
}

static void test_handler_list(
	myy_widget_list_t * __restrict const widget,
	myy_states * __restrict const states,
	int const index,
	void * click_widget,
	void * user_arg)
{
	LOG("[test_handler_list] %d\n", index);
}

myy_list_element_click_handler handler = test_handler_list;


static char const * string_id_to_string(
	global_state_t * __restrict const state,
	string_id_t const * __restrict const id_ptr)
{
	char const * __restrict const text =
		strings_get(state, *id_ptr).text;
	LOG("String : %s\n", text);
	LOG("[STRING_ID_TO_STRING] %p (Global state : %p)\n", text, state);
	return text;
}

typedef bool (*generic_add_handler)(
	union myy_menu_widget *,
	myy_states *,
	void *);

static void test_text_edit_ae_handler(
	void * user_arg,
	myy_states * __restrict const states,
	struct myy_widget_text_edit * __restrict const widget,
	char const * __restrict const current_value)
{
	LOG("[text_text_edit_ae_handler] Miou : %s\n", current_value);
}

static uint64_t handle_print(
	char const * __restrict const message)
{
	LOG("%s", message);
	LOG("Returning to base...");
	return 7487;
}
static void editor_button_add_onclick(
	void * __restrict const user_arg,
	myy_states * __restrict const states,
	struct myy_widget_button * __restrict const widget)
{
	LOG("On click !\n");
	auto a = dynarray<uint32_t>();
	a.init(8);
	a.inspect();
	sauvage.assemble_into(&a, myy_states_assembler_state_addr(states));
	LOG("Assembled %lu instructions\n", a.length());
	LOG("[");
	a.for_each([](auto assembled_inst){
		LOG("\n0x%08x,", *assembled_inst);
	});
	LOG("\n]\n");
	uint64_t (*compiled_code_entry)(
		char const * __restrict const string,
		uint64_t (*)(char const *)) =
		(uint64_t (*)(char const * __restrict, uint64_t (*)(char const *))) mmap(
			0, 4096,
			PROT_READ|PROT_WRITE,
			MAP_PRIVATE|MAP_ANONYMOUS, 0, 0);
	if (compiled_code_entry == MAP_FAILED) {
		LOG("MAP FAILED");
		return;
	}
	else {
		LOG("MMAP : %p\n", compiled_code_entry);
	}
	memcpy((void *) compiled_code_entry, a.data(), a.mem_used());
	uint32_t * __restrict const instructions = 
		(uint32_t *) compiled_code_entry;
	for (uint32_t i = 0; i < a.length(); i++) {
		LOG("[MMAP] 0x%08x\n", instructions[i]);
	}
	char meow[] = "Cochon d'indonax !";
	LOG("mprotect = %d\n", 
		mprotect((void *) compiled_code_entry, 4096, PROT_READ|PROT_EXEC));
	
	LOG("[Exec] %lu <-\n",compiled_code_entry(meow, handle_print));
	a.free_content();
}

static void sidemenu_prepare(
	myy_states * __restrict const states,
	char const * __restrict const title)
{
	myy_user_state_t * __restrict const ustate =
		myy_user_state_from(states);
	sidepane_right_menu_reset(&right_menu, states);
	/*sidepane_right_menu_add_button(&right_menu, states,
		"Pouip", sidepane_menu_button_test, &right_menu);
	sidepane_right_menu_add_button(&right_menu, states,
		"Fuck you", sidepane_menu_button_test, &right_menu);
	sidepane_right_menu_add_button(&right_menu, states,
		"Sa race en slip de bains", sidepane_menu_button_test, &right_menu);
	instruction_t const * __restrict const instructions =
		myy_vector_instruction_data(&test_collection->instructions);
	{
		myy_list_widget_generic_strings_args_t const args = {
			.n_elements =
				myy_vector_instruction_length(&test_collection->instructions),
			.start_address =
				(uint8_t *) (&instructions->name_id),
			.stride =
				sizeof(instruction_t),
			.to_string =
				(char const * (*)(void * arg, uint8_t *)) string_id_to_string,
			.to_string_arg =
				(void *) (&ustate->assembler_state),
			.list_click_handler =
				test_handler_list,
			.list_click_arg =
				&right_menu
		};
		sidepane_right_menu_add_widget(
			&right_menu,
			states,
			(generic_add_handler)
				myy_list_widget_add_generic_string_list,
			(void *) &args);
	}*/

	{
		myy_menu_widget_definition_t sub_widgets[] = {
			{
				.type = myy_menu_widget_type_button,
				.args = {
					.button = {
						.text = "Execute",
						.action = editor_button_add_onclick,
						.action_arg = (void *) 0
					}
				}
			},
		};
		myy_widget_vertical_layout_add_args_t const args = {
			.n_widgets =
				(sizeof(sub_widgets)/sizeof(typeof(sub_widgets[0]))),
			.widgets = sub_widgets
		};
		
		union myy_menu_widget * const added_widget =
			sidepane_right_menu_add_widget(
				&right_menu,
				states,
				myy_menu_widget_type_vertical_layout,
				(void*) &args);
		myy_widget_vertical_layout_t * __restrict const layout =
			&added_widget->vertical_layout;
		myy_vector_menu_widgets * __restrict const pouips =
			myy_widget_vertical_layout_widgets(layout);
		myy_widget_button_t * __restrict const button =
			&myy_vector_menu_widgets_at_ptr(pouips, 0)->button;

		button->action = editor_button_add_onclick;
		button->user_arg = layout;
		
	}
	sidepane_right_menu_set_title(&right_menu, title);
	sidepane_right_menu_set_subtitle(&right_menu, "pouip");
	sidepane_right_menu_refresh(&right_menu, states);
	//sidepane_right_menu_store_to_gpu(&right_menu);
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
    LOG("Init drawing");
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

	init_assembler(state);
	user_state->draw_functions_list.add(MainBoard::draw, &board);
	sidemenu_prepare(state, "Meow");

}

void myy_draw(
	myy_states * __restrict state,
	uintreg_t i,
	uint64_t last_frame_delta_ns)
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

	board.set_camera(
		offset_x + offset_current_x,
		offset_y + offset_current_y);
	myy_draw_functions_draw(
		&myy_user_state_from(state)->draw_functions,
		state,
		i);
	//user_state::from(state)->draw_functions_list.draw(state);
	lines_draw();
}

/* TODO
 * Double offset au niveau des shaders pour permettre de les afficher
 * dans une vue défilée.
 * Ou bien passer le coin supérieur gauche durant l'affichage de chaque
 * élément.
 * Chaque élément peut ensuite passer ça aux éléments enfants durant
 * l'affichage.
 */


/* TODO Pour l'affichage de listes de chaînes de caractères
 * Parcourir les tableaux de chaînes de caractères avec :
 * - L'adresse du premier élément
 * - Un pointeur de fonction pour obtenir une chaîne de caractères
 *   depuis l'adresse fournie.
 * - Une longueur de saut pour aller à l'élément suivant.
 * - Le nombre d'éléments à récupérer depuis la liste
 */

static void menu_edition_liste_instructions_bouton_ajout_demarrer(
	void * __restrict const menu,
	myy_states * __restrict const states,
	myy_widget_button_t * __restrict const button)
{
	
}

static void menu_edition_liste_instructions_bouton_suppression_demarrer(
	void * __restrict const menu,
	myy_states * __restrict const states,
	myy_widget_button_t * __restrict const button)
{
	
}

static void menu_edition_liste_instructions(
	instructions_collection_t * __restrict const collection,
	myy_states_s * __restrict const states)
{
	/* TODO Global_state_t needs to be renamed assembler_state_t */
	global_state_t * __restrict const state =
		myy_states_assembler_state_addr(states);
	string_data_t const collection_name_infos =
		strings_get(state, collection->name_id);
	char const * __restrict const collection_name =
		(collection_name_infos.text != (char const *) 0)
		? collection_name_infos.text
		: "No name";
	sidepane_right_menu_reset(&right_menu, states);
	sidepane_right_menu_set_title(&right_menu, "Liste des instructions");
	sidepane_right_menu_set_subtitle(&right_menu, collection_name);
	sidepane_right_menu_add_button(&right_menu, states,
		"Add instruction",
		menu_edition_liste_instructions_bouton_ajout_demarrer,
		&right_menu);
	sidepane_right_menu_add_button(&right_menu, states,
		"Remove instructions...",
		menu_edition_liste_instructions_bouton_suppression_demarrer,
		&right_menu);
	sidepane_right_menu_refresh(&right_menu, states);
	
}

static uint32_t moving = 0;
static uint32_t start_x = 0, start_y = 0;

#ifdef __cplusplus
extern "C" {
#endif
/* Question : How does that work, from the first interaction ? */
void myy_editor_finished(
	myy_states * __restrict const states,
	uint8_t const * __restrict const string,
	size_t const string_size)
{
	/* Still ugly, IMHO */
	myy_user_state_t * __restrict const user_state =
		myy_user_state_from(states);
	myy_states_text_edit_module_deal_with(states, (char *) string);
	user_state->editing = false;
}

#ifdef __cplusplus
}
#endif

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
			sidepane_right_menu_handle_click(&right_menu, state,
				position_S_struct(event->mouse_button.x, event->mouse_button.y),
				event_type, event);
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
			sidepane_right_menu_handle_click(&right_menu, state,
				position_S_struct(event->mouse_button.x, event->mouse_button.y),
				event_type, event);
			if (event->touch.x < 150) {
				sidemenu_show(state);
			}
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
		/* TODO We need a myy_input_event_chars_received to differentiate
		 * an entire text block to set in a buffer
		 * from chars to append to a buffer.
		 */
		case myy_input_event_text_received:
			LOG("TEXT: %s\n", event->text.data);
			//myy_states_text_edit_module_deal_with(state, event->text.data);
			break;
		case myy_input_event_editor_finished:
			LOG("Editor finished with : %s !\n", event->text.data);
			myy_editor_finished(state,
				(uint8_t const *) event->text.data,
				event->text.length);
		case myy_input_event_surface_size_changed:
			myy_display_initialised(
				state,
				event->surface.width, event->surface.height);
			break;
		case myy_input_event_window_destroyed:
			myy_user_quit(state);
			break;
		default:
			break;
	}
}
