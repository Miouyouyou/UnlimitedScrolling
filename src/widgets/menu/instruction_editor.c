#include "src/widgets/menu/instruction_editor.h"
#include "src/widgets/lateral_menu.h"


#include <stdbool.h>

/**
 * L'éditeur d'instructions englobe sa présentation complète.
 * C'est à dire que si celui-ci doit s'afficher comme un menu,
 * il intègre la création, l'affichage et le remplissage du menu
 * et la destruction du menu.
 * 
 * Cette approche casse la flexibilité.
 * Cependant, elle reste la plus simple à mettre en place
 * dans un temps court.
 * 
 * TODO Ça casse complètement l'idée d'un MVC en général, ça
 * force à tout gérer de manière très spécialisée étant donné
 * que l'on a aucun moyen d'accéder à un widget rajouté dans
 * le menu facilement.
 * Sans compter que l'on initialise des widgets inutilement
 * dans les champs, puisqu'ils sont censés être rajoutés dans
 * l'arbre de widgets du menu, et non pas dans un conteneur
 * quelconque, à moins que ce conteneur agisse en tant que
 * widget lui même...
 * 
 * Solution potentielle :
 * - Renvoyer l'adresse de chaque widget au menu rajouté via
 *   sidepane_right_menu_add
 * - Stocker les adresses des widgets dans
 *   struct instruction_editor_field
 * 
 * Seconde solution :
 * - Créer un widget "Vertical layout" (Agencement vertical)
 * - Rajouter les widgets au layout.
 * - Rajouter le layout au menu.
 * - Aliasser le layout vers un type "instruction field"
 * - Bâtir l'interface d'instruction autour du layout, avec
 *   accès direct aux widgets.
 * + CLEAN
 * 
 */

static void click_handler() {
	myy_vector_menu_widgets * __restrict const widgets =
		myy_widget_vertical_layout_widgets(layout);

	myy_widget_text_edit_t * __restrict const te_name =
		(myy_widget_text_edit_t *)
		myy_vector_menu_widgets_at_ptr(widgets, 1);
	char const * __restrict name =
		myy_widget_text_edit_input_value(te_name);

	myy_widget_text_edit_t * __restrict const te_from =
		(myy_widget_text_edit_t *)
		myy_vector_menu_widgets_at_ptr(widgets, 3);
	char const * __restrict from =
		myy_widget_text_edit_input_value(te_from);

	myy_widget_text_edit_t * __restrict const te_to =
		(myy_widget_text_edit_t *)
		myy_vector_menu_widgets_at_ptr(widgets, 5);
	char const * __restrict to =
		myy_widget_text_edit_input_value(te_to);

}


static myy_instruction_edit_init_args_t default_init_args = {
	.menu = (sidepane_menu_t *) 0
};

struct sidepane_right_menu_new_args {
	char const * __restrict const title;
	char const * __restrict const subtitle;
	uint32_t n_widgets;
	myy_menu_widget_t widgets[];
};
typedef struct sidepane_right_menu_new_args
	sidepane_right_menu_new_args_t;


char const * myy_instruction_editor_current_instruction_title(
	struct myy_instruction_editor * __restrict const editor,
	myy_states * __restrict const states)
{
	strings_get(
		myy_states_assembler_state_addr(states),
		editor->edited_instruction->name_id);
}

static void show_instruction_add_field_interface(
	void * __restrict const user_arg,
	myy_states * __restrict const states)
{
	struct myy_instruction_editor * __restrict const editor =
		(struct myy_instruction_editor * __restrict) user_arg;

	sidepane_right_menu_new_args_t args = {
		.title = myy_instruction_editor_current_instruction_title(
			editor, states),
		.subtitle = "Add field",
		.widgets = {
			
		}
	};
	/*sidepane_right_menu_new_with_widgets(
		editor, states, */
}

static void myy_instruction_editor_add_field_onclick(
	void * __restrict const user_arg,
	myy_states * __restrict const states,
	struct myy_widget_button * __restrict const widget)
{
	struct myy_instruction_editor * __restrict const editor =
		(struct myy_instruction_editor * __restrict) user_arg;

	show_instruction_add_field_interface(editor, states);
}

/* Init with menu pointer ? */
bool myy_instruction_edit_init(
	struct myy_instruction_editor * __restrict const editor,
	myy_states * __restrict const states,
	myy_instruction_edit_init_args_t * __restrict const provided_args)
{
	myy_instruction_edit_init_args_t * const args =
		(provided_args ? provided_args : &default_init_args);

	struct myy_instruction_editor local_editor;
	/* FIXME Can't we just use the current menu,
	 * display it and fill it with the right widgets,
	 * instead of creating another one ?
	 */
	local_editor.menu_managed =
		(args->menu == (sidepane_menu_t *) 0);
	if (local_editor.menu_managed) {
		local_editor.menu = 
			(typeof(local_editor.menu))
			malloc(sizeof(*local_editor.menu));
		if (!local_editor.menu)
			goto cannot_allocate_memory_for_menu;

		sidepane_right_menu_init(local_editor.menu,
			myy_states_gl_text_atlas_addr(states),
			states->surface_width,
			states->surface_height,
			1);
	}
	else {
		local_editor.menu = args->menu;
	}

	{
		myy_widget_button_add_args_t args = {
			.text = "Add field",
			.action = myy_instruction_editor_add_field_onclick,
			.action_arg = (void *) editor
		};
		sidepane_right_menu_add_widget(
			local_editor.menu,
			states,
			myy_menu_widget_type_button,
			&args);
			
	}
	return true;
cannot_allocate_memory_for_menu:
	return false;
}

void myy_instruction_editor_field_init(
	struct myy_instruction_editor_field * __restrict const fields,
	myy_states * __restrict const states)
{
	{
		myy_widget_text_edit_add_args_t args = {
			.default_text = "Field name",
			.after_edit_handler =
				(myy_widget_text_edit_after_edit_handler) 0,
			.after_edit_arg = 0
		};
		myy_widget_text_edit_add(
			fields->widgets+myy_instruction_editor_field_index_name,
			states,
			&args);
		args.default_text = "0";
		myy_widget_text_edit_add(
			fields->widgets+myy_instruction_editor_field_index_from,
			states,
			&args);
		args.default_text = "5";
		myy_widget_text_edit_add(
			fields->widgets+myy_instruction_editor_field_index_to,
			states,
			&args);
	}
	{
		struct myy_widget_button_add_arguments args = {
			.text = "From",
			.action = (myy_button_widget_action_ptr) 0,
			.action_arg = (void *) 0
		};
		myy_widget_button_add(
			fields->widgets+myy_instruction_editor_field_index_label_from,
			states,
			&args);
		args.text = "To";
		myy_widget_button_add(
			fields->widgets+myy_instruction_editor_field_index_label_to,
			states,
			&args);
	}
}

bool myy_instruction_editor_field_add(
	struct myy_instruction_editor * __restrict const editor,
	myy_states * __restrict const states)
{
	myy_vector_instruction_editor_fields * __restrict const
		fields = &editor->fields;
	bool const
		added = myy_vector_instruction_editor_fields_add_empty(fields, 1);
	
	if (added) {
		struct myy_instruction_editor_field * __restrict const
			added_field = myy_vector_instruction_editor_fields_tail_ptr(fields);
		myy_instruction_editor_field_init(added_field, states);
	}
}

void myy_instruction_edit_cleanup(
	struct myy_instruction_editor * __restrict const editor,
	myy_states * __restrict const states)
{

	sidepane_right_menu_reset(editor->menu, states);
	/* FIXME Only useful if we handle the creation of the
	 * menu ourselve, instead of borrowing it.
	 */
	if (editor->menu_managed)
		sidepane_right_menu_cleanup(editor->menu, states);
}
