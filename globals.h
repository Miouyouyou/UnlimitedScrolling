#ifndef MYY_GLOBALS_H
#define MYY_GLOBALS_H 1

/* A list of global variables 'extern' definitions.
 * These variables should be accessible through
 * another way in the final versions.
 * As ugly as they are, global variables make it
 * really easy to focus on the main problems and
 * avoid overthinking the implementation at
 * prototype stage.
 */

#include <shaders.h>
#include <myy.h>

#include "myy/helpers/fonts/packed_fonts_parser.h"
#include "myy/helpers/fonts/packed_fonts_display.h"
#include "lib/QuickAssembler/assembler.h"
#include "src/widgets/text_edit_module.h"

#include "myy_user_state.h"


extern struct myy_shaders_db myy_programs;


#include <src/widgets/text_area.h>


#ifdef __cplusplus

#include "myy/helpers/myy_vector.hpp"

namespace myy {
	struct draw_functions {
		
		struct draw_function {
			typedef void (*draw_func_ptr)(
				void * __restrict const arg,
				myy_states * __restrict const states);

			draw_func_ptr func;
			void * __restrict const arg;
			static void dummy_func(
				void * __restrict const arg,
				myy_states * __restrict const states)
			{
				LOG("I'm a dummy draw function that should not have been called !\n");
			}
			draw_function() :
				func(dummy_func),
				arg(nullptr)
			{}
			draw_function(
				draw_func_ptr ofunc,
				void * __restrict const oarg) :
				func(ofunc), arg(oarg)
			{ }
		};
		dynarray<draw_function> functions;
		draw_functions() :
			functions(dynarray<draw_function>())
		{}
		inline bool init() {
			this->functions.init(4096);
			this->functions.inspect();
			return this->functions.is_valid();
		}
		inline void add(
			draw_function::draw_func_ptr func,
			void * __restrict const arg)
		{
			functions.append_memcpy(draw_function(func,arg));
		}
		inline void remove(draw_function::draw_func_ptr func)
		{
			functions.remove_if(
				[func](draw_function const * __restrict const df) -> bool {
					return df->func == func;
				}
			);
		}
		inline void draw(myy_states * __restrict const states) {
			functions.for_each(
				[states](draw_function const * __restrict const df) {
					df->func(df->arg, states);
				}
			);
		}
	};

	/* TODO Find a proper way...
	 * But until then, let's advance... */

	/* Methodology
	 * The javascript way :
	 * - Pass a function and an argument to the async method.
	 * - When it finishes, it calls the function with the arg.
	 * The function with the argument will then be used to
	 * determine how to deal with things.
	 */
	struct user_state {
		static user_state * from(
			myy_states * __restrict const states)
		{
			return (user_state *) (states->user_state);
		}
		static user_state * from(
			myy_states const * __restrict const states)
		{
			return (user_state *) (states->user_state);
		}

		draw_functions draw_functions_list;
		struct {
			void * callback_arg;
			myy_user_state_edited_callback done_callback;
		} edited;
		global_state_t assembler_state;
		gl_text_infos gl_text_atlas;
		bool editing;
	};
}
#endif

#endif
