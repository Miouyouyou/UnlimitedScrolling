// -*- C++ -*-
#ifdef __cplusplus
#include "simple_forms.h"
#include <lib/QuickAssembler/assembler.h>

#include <cstdint>

#include <myy/helpers/myy_vector.hpp>

#include <myy/current/opengl.h>

#include <src/widgets/simple_forms.h>
#include <src/widgets/menu_forms.h>

namespace myy {
	enum node_type {
		invalid,
		instructions,
		data
	};

	struct InstructionListElement {
		uint64_t instruction_id;
		dynarray<uint64_t> values;
	};

	struct InstructionNode {
		uint64_t const id;
		node_type const type = node_type::instructions;
		dynarray<InstructionListElement> instructions;
		stringdb_memory memory;
		InstructionNode() :
			id(0),
			type(node_type::invalid),
			memory()
		{}
		InstructionNode(uint64_t const oid) :
			id(oid),
			type(node_type::instructions),
			instructions(),
			memory()
		{
			stringdb_memory_setup(&this->memory);
			instructions.init();
		}
	};

	
	struct List {
		menu_forms  forms;
		text_buffer text;
		List() :
			forms(),
			text()
		{}
		void init(
			Point4D<uint16_t> const position,
			struct gl_text_infos * __restrict const gl_text_atlas)
		{
			menu_forms_init(&forms);
			text_buffer_init(&text, gl_text_atlas);
		}
	};
}
#endif
