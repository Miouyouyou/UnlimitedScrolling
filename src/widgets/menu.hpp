#pragma once

#include <globals.h>

#include <src/widgets/helper_area.hpp>
#include <src/widgets/helpers_screenactions.hpp>

namespace myy {

	enum InteractionTypes {
		interaction_type_invalid,
		interaction_type_click,
		interaction_type_move,
		interaction_type_touch,
		interaction_type_count
	};
	struct MenuForms {
		typedef ColoredPoint< Point4D<int16_t>,RGBA<uint8_t> > MenuColoredPoint;

		// Mainly used to cache cpu_buffer.length()
		// Remember that draw functions will be called at
		// least 60 times per second (up to 200 times).
		GLuint n_points;
		dynarray<MenuColoredPoint> cpu_buffer;
		GLuint gpu_buffer;
		Point4D<int16_t> offset;

		MenuForms() :
			n_points(0),
			cpu_buffer(),
			gpu_buffer(0),
			offset(0,0,0,0)
		{
		}
		~MenuForms() {
		}
		void init() {
			n_points = 0;
			cpu_buffer.init(256);
			glGenBuffers(1, &gpu_buffer);
			offset.set(0,0,0,0);
		}
		void set_offset(Point4D<int16_t> ooffset)
		{
			offset = ooffset;
		}

		void set_offset(
			int16_t const x, int16_t const y)
		{
			offset.set(x, y);
		}

		void free_content() {
			cpu_buffer.free_content();
			glDeleteBuffers(1, &gpu_buffer);
		}
		MenuForms(MenuForms const & oelements) :
			n_points(oelements.n_points),
			cpu_buffer(oelements.cpu_buffer),
			gpu_buffer(oelements.gpu_buffer),
			offset(oelements.offset)
		{}

		void set_projection(
			myy_4x4_matrix const * __restrict const projection)
		{
			glUseProgram(myy_programs.menu_forms_id);
			glUniformMatrix4fv(
				myy_programs.menu_forms_unif_projection,
				1,
				GL_FALSE,
				projection->raw_data);
			glUseProgram(0);
		}

		void draw()
		{
			glUseProgram(myy_programs.menu_forms_id);
			glUniform4f(
				myy_programs.menu_forms_unif_global_offset,
				offset.x, offset.y, offset.z, offset.w);
			glBindBuffer(GL_ARRAY_BUFFER, gpu_buffer);
			glVertexAttribPointer(
				menu_forms_xyz,
				4, GL_SHORT, GL_FALSE,
				cpu_buffer.mem_used(),
				(gpu_buffer_offset_t)
				(offsetof(MenuColoredPoint, point)));
			glVertexAttribPointer(
				menu_forms_in_color,
				4, GL_UNSIGNED_BYTE, GL_TRUE,
				cpu_buffer.mem_used(),
				(gpu_buffer_offset_t)
				(offsetof(MenuColoredPoint, color)));
			glDrawArrays(GL_TRIANGLES, 0, n_points);
			glUseProgram(0);
		}

		void add_triangle(
			Triangle<MenuColoredPoint> triangle)
		{
			cpu_buffer.add_memcpy(
				triangle.points_const(),
				triangle.n_points());
			n_points = cpu_buffer.length();
		}

		void add_rectangle(
			Area<MenuColoredPoint> const area)
		{
			add_triangle({
				area.upper_left(),
				area.bottom_left(),
				area.bottom_right()});
			add_triangle({
				area.upper_right(),
				area.upper_left(),
				area.bottom_right()});
		}

		void add_arrow_left(
			Area<MenuColoredPoint> * __restrict const area)
		{
			add_triangle({
				area->middle_left(),
				area->bottom_right(),
				area->upper_right(),
			});
		}

		void add_arrow_right(
			Area<MenuColoredPoint> * __restrict const area)
		{
			add_triangle({
				area->middle_right(),
				area->upper_left(),
				area->bottom_left()
			});
		}
	};
}
