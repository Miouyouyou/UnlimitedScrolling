// -*- C++ -*-
#pragma once
#ifdef __cplusplus

#include "simple_forms.h"
#include <lib/QuickAssembler/assembler.h>

#include <cstdint>

#include <myy/helpers/myy_vector.hpp>

#include <myy/current/opengl.h>

#include <src/widgets/simple_forms.h>
#include <src/widgets/menu_forms.h>

#include <myy.h>

namespace myy {
	enum node_type {
		invalid,
		instructions,
		data
	};

	instruction_t bad_instruction = {
		.id = 0,
		.name_id = 0,
		.base_value = 0,
		.fields = {0,0,0}
	};
	struct InstructionNodeElement {
		instruction_t * __restrict instruction;
		dynarray<uint64_t> values;
		InstructionNodeElement() :
			instruction(&bad_instruction),
			values()
		{
			instruction = &bad_instruction;
		}
		InstructionNodeElement(
			instruction_t * __restrict oinst,
			dynarray<uint64_t> const ovalues)
			: instruction(oinst), values(ovalues)
		{
			LOG("[InstructionNodeElement] instruction : %p\n", instruction);
		}
	};

	struct InstructionNode {
		uint64_t const id;
		node_type const type = node_type::instructions;
		char const * __restrict title;
		dynarray<InstructionNodeElement> instructions;
		stringdb_memory memory;
		InstructionNode() :
			id(0),
			type(node_type::invalid),
			title("Mooh"),
			instructions(),
			memory()
		{}
		InstructionNode(
			uint64_t const oid,
			char const * __restrict const provided_title) : 
			id(oid),
			type(node_type::instructions),
			title(provided_title),
			instructions(),
			memory()
		{
			init();
		}

		void init() {
			if (type != node_type::invalid) {
				instructions.init();
				stringdb_memory_setup(&memory);
			}
		}

		void add(
			instruction_t * __restrict const instruction,
			dynarray<uint64_t> const values)
		{
			instructions.append_memcpy(
				InstructionNodeElement(instruction,values));
			LOG("[InstructionNode#add] Added instruction : %p\n", instructions.last_ptr()->instruction);
		}
	};

	struct DisplayedNode {
		InstructionNode * __restrict node;
		Point4D<int16_t> position;

		DisplayedNode() :
			node(nullptr /* FIXME Point to a dummy default node instead */),
			position({0,0,0,0})
		{}

		void init(
			Point4D<int16_t> const pos,
			InstructionNode * __restrict const linked_node)
		{
			position = pos;
			node = linked_node;
		}

		DisplayedNode(
			Point4D<int16_t> const at,
			InstructionNode * __restrict linked_node) :
			node(linked_node),
			position(at)
		{ }

		void move(Point4D<int16_t> const to)
		{
			position = to;
		}

		char const * instruction_to_string(
			instruction_t const * __restrict const instruction,
			myy_states const * __restrict const states)
		{
			auto assembler_state =
				&user_state::from(states)->assembler_state;
			return strings_get(assembler_state, instruction->name_id).text;
		}
		/* The generate coords function */
		void store_to_gpu(
			myy_states const * __restrict const states,
			menu_forms  * __restrict forms,
			text_buffer * __restrict text)
		{
			constexpr int16_t margin = 6 /* px */;
			int16_t width = 0 /* px */;
			int16_t const line_height = text->text_display_atlas->default_max_line_height()-6;
			int16_t const line_start = margin + position.x;
			position_S_3D start_pos = position_S_3D_struct(
				position.x+margin,
				position.y+line_height,
				position.z);
			text_buffer_add_string_colored(
				text, (uint8_t const * __restrict) (node->title),
				&start_pos, rgba8_color(255,255,255,255));
			width = start_pos.x - line_start;
			start_pos.x  = line_start;
			/* Use lambdas, get weird shit errors about the compiler
			 * not being able to capture some local variables correctly.
			 * ...
			 * WTF ?
			 * The worst thing is that I don't give two shits about
			 * closures. I just want to call a function on each element
			 * of my array !
			 * Iterators in C++ being fucking stupid to write, here
			 * I go with some dumb C traversal... which works...
			 */
			auto instructions = node->instructions;
			auto element_address = instructions.data();
			auto const last_address = instructions.tail_cptr();
			
			while(element_address < last_address) {
				/* 'const' isn't only about "You're not authorized to
				 * write into it" in C++, it's also about ensuring that
				 * the element isn't volatile, meaning that its content
				 * won't change during the function call...
				 * The second concept has its uses with multithreading,
				 * and most importantly, multicore multithreading where
				 * data access races are real.
				 * However, I only care about the first concept here,
				 * not the second !
				 * Now, since element->instruction isn't marked as
				 * constant, the compiler flips a shit if I pass it to
				 * a function expecting a const... That's not a problem
				 * in C, since in C const is only about "readonly in the
				 * definition scope". And that's exactly the concept I
				 * want to enforce here. I'm just telling the compiler
				 * "This function won't modify the content of the provided
				 * argument !"
				 * This whole "const" story in C++ is the main reason I
				 * tend to avoid this stupid language.
				 * They should have use another keyword like "readonly"
				 * to enforce the first concept, or "unmutable" to
				 * enforce the second concept along the first.
				 * Dear C++ norm writers, don't take an already existing
				 * keyword and associate concepts that were not associated
				 * to it initially !
				 * Or just remove the 'C' in 'C++' !
				 */
				
				instruction_t const * __restrict const instruction_const =
					(instruction_t const * __restrict)
					element_address->instruction;
				auto string = instruction_to_string(instruction_const, states);
				start_pos.y += margin+line_height;
				text_buffer_add_string_colored(
					text, (uint8_t const * __restrict) string,
					&start_pos, rgba8_color(255,255,255,255));
				int16_t const current_line_width =
					(int16_t) (start_pos.x - line_start + margin);
				width = (width > current_line_width ? width : current_line_width);
				start_pos.x  = line_start;
				start_pos.y += margin;
				element_address++;
			}
			int16_t const total_width  = width + margin;
			int16_t const total_height = start_pos.y - position.y;
			LOG("Final width : %d\n", width);
			menu_forms_add_rectangle(
				forms,
				position_S_struct(position.x, start_pos.y),
				dimensions_S_struct(total_width, total_height),
				rgba8_color(0,0,0,255));
				//rgba8_color(128,128,128,255));
				
			menu_forms_store_to_gpu(forms);
			text_buffer_store_to_gpu(text);
		}
	};

	struct MainBoard {
		menu_forms forms;
		text_buffer text;
		dynarray<DisplayedNode> nodes;
		Point4D<int16_t> camera_center;

		MainBoard() :
			forms(),
			text(),
			nodes(),
			camera_center(0,0,0,0)
		{
		}

		MainBoard(
			Point4D<int16_t> const position,
			myy_states * __restrict const states) :
			forms(),
			text(),
			nodes(),
			camera_center(position)
		{
			init(states);
		}

		void set_camera(Point4D<int16_t> const to) {
			camera_center = to;
			menu_forms_set_global_position(&forms, to.to_pos_S_4D());
			text_buffer_set_global_position(&text, to.to_pos_S_4D());
		}

		void set_camera(Point2D<int16_t> const to) {
			set_camera({to.x,to.y,camera_center.z,0});
		}

		void set_camera(int16_t const x, int16_t const y) {
			set_camera(Point4D<int16_t>(x, y, camera_center.z, 0));
		}
		void init(myy_states const * __restrict const states)
		{
			menu_forms_init(&forms);
			text_buffer_init(&text, &user_state::from(states)->gl_text_atlas);
			nodes.init(16);
			set_camera(camera_center);
		}

		void init(
			myy_states const * __restrict const states,
			Point4D<int16_t> to)
		{
			init(states);
			set_camera(to);
		}

		void show(
			myy_states * __restrict const state,
			InstructionNode * __restrict const node,
			Point4D<int16_t> const at)
		{
			nodes.inspect("Nodes");
			nodes.append_memcpy(DisplayedNode(at, node));
			redraw(state);
		}

		void hide(
			myy_states * __restrict const state,
			DisplayedNode * __restrict const node)
		{
			nodes.remove_if([node](auto const current_node) -> bool {
				return node == current_node;
			});
			redraw(state);
		}

		void hide(
			myy_states * __restrict const state,
			InstructionNode * __restrict const node)
		{
			nodes.remove_if([node](auto const displayer) -> bool {
				return displayer->node == node;
			});
			redraw(state);
		}

		void clear(myy_states * __restrict const state) {
			menu_forms_reset(&forms);
			text_buffer_reset(&text);
		}

		void redraw(myy_states * __restrict const state) {
			clear(state);
			store_to_gpu(state);
		}

		void store_to_gpu(myy_states * __restrict const state) {
			auto current_node = nodes.data();
			auto const end = nodes.tail_cptr();
			while(current_node < end) {
				current_node->store_to_gpu(state, &forms, &text);
				current_node++;
			}
		}

		void draw(myy_states * __restrict const state) {
			menu_forms_draw(&forms);
			text_buffer_draw(&text);
		}

		static inline void draw(
			void * __restrict const vboard,
			myy_states * __restrict const states)
		{
			MainBoard * __restrict const board =
				(MainBoard * __restrict) vboard;
			board->draw(states);
		}
	};

}
#endif
