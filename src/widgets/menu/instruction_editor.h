#ifndef MYY_SRC_WIDGETS_MENU_INSTRUCTION_EDITOR_H
#define MYY_SRC_WIDGETS_MENU_INSTRUCTION_EDITOR_H 1

#include "myy/helpers/vector.h"

#include "lib/QuickAssembler/assembler.h"
#include "src/widgets/menu_forms.h"
#include "src/widgets/text_buffer.h"

#include "src/widgets/menu/widgets.h"

#include "src/widgets/lateral_menu.h"

enum myy_instruction_editor_field_widgets_indices {
	myy_instruction_editor_field_index_name,
	myy_instruction_editor_field_index_label_from,
	myy_instruction_editor_field_index_from,
	myy_instruction_editor_field_index_label_to,
	myy_instruction_editor_field_index_to,
	myy_instruction_editor_field_count
};

struct myy_instruction_editor_field {
	union myy_menu_widget widgets[myy_instruction_editor_field_count];
};

myy_vector_template(instruction_editor_fields, struct myy_instruction_editor_field)

enum myy_widget_instruction_editor_widgets_indices {
	myy_widget_instruction_editor_index_name,
	myy_widget_instruction_editor_index_label_base_value,
	myy_widget_instruction_editor_index_edit_base_value,
};

struct myy_instruction_edit_init_args {
	sidepane_menu_t * __restrict const menu;
};

typedef struct myy_instruction_edit_init_args myy_instruction_edit_init_args_t;

struct myy_instruction_editor {
	instruction_t * __restrict const edited_instruction;
	sidepane_menu_t * __restrict menu;
	union myy_menu_widget widgets[2];
	myy_vector_instruction_editor_fields fields;
	bool menu_managed;
};


#endif
