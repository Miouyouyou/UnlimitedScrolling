#pragma once

#include <globals.h>

#include <src/widgets/helpers_area.hpp>
#include <myy/helpers/points.hpp>

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
		GLuint previous_n_points;

		MenuForms() :
			n_points(0),
			cpu_buffer(),
			gpu_buffer(0),
			offset(0,0,0,0),
			previous_n_points(0)
		{
			LOG("Called Menuforms alone !\n");
		}
		~MenuForms() {
			LOG("Called Destroy menuforms !\n");
		}
		void init() {
			LOG("[C++] MenuForms#init() !\n");
			n_points = 0;
			cpu_buffer.init(256);
			glGenBuffers(1, &gpu_buffer);
			offset.set(0,0,0,0);
			previous_n_points = 0;
		}
		inline void reset() { cpu_buffer.reset(); }

		void free_content() {
			LOG("[C++] Free content !");
			cpu_buffer.free_content();
			glDeleteBuffers(1, &gpu_buffer);
		}

		MenuForms(MenuForms const & oelements) :
			n_points(oelements.n_points),
			cpu_buffer(oelements.cpu_buffer),
			gpu_buffer(oelements.gpu_buffer),
			offset(oelements.offset),
			previous_n_points(oelements.previous_n_points)
		{ LOG("Calling move constructor (?)\n"); }

		void set_offset(Point4D<int16_t> ooffset)
		{
			offset = ooffset;
		}

		void set_offset(
			int16_t const x, int16_t const y)
		{
			offset.set(x, y);
		}

		void set_projection(
			myy_4x4_matrix const * __restrict const projection)
		{
			glUseProgram(myy_programs.simple_forms_id);
			glUniformMatrix4fv(
				myy_programs.simple_forms_unif_projection,
				1,
				GL_FALSE,
				projection->raw_data);
			glUseProgram(0);
		}

		void store_to_gpu()
		{
			LOG("\n\n\n[C++] ------------ STORE TO GPU -------------\n\n\n");
			glBindBuffer(GL_ARRAY_BUFFER, gpu_buffer);
			LOG("glBindBuffer(GL_ARRAY_BUFFER, %d\n", gpu_buffer);
			glBufferData(GL_ARRAY_BUFFER,
				cpu_buffer.mem_used(),
				cpu_buffer.data(),
				GL_DYNAMIC_DRAW);
			LOG("glBufferData(Gl_ARRAY_BUFFER, %lu, %p, GL_DYNAMIC_DRAW);\n",
				cpu_buffer.mem_used(), cpu_buffer.data());

			n_points = cpu_buffer.length();
			previous_n_points = n_points;
			LOG("\n\n\n[C++] ++++++++++++ N POINTS : %d\n", n_points);
		}
		void draw()
		{
			
			glUseProgram(myy_programs.simple_forms_id);
			glUniform4f(
				myy_programs.simple_forms_unif_global_offset,
				offset.x, offset.y, offset.z, offset.w);
			glBindBuffer(GL_ARRAY_BUFFER, gpu_buffer);
			//LOG("[C++] gpu_buffer : %d\n", gpu_buffer);
			glVertexAttribPointer(
				simple_forms_xyz,
				4, GL_SHORT, GL_FALSE,
				sizeof(MenuColoredPoint),
				(gpu_buffer_offset_t)
				(offsetof(MenuColoredPoint, point)));

			glVertexAttribPointer(
				simple_forms_in_color,
				4, GL_UNSIGNED_BYTE, GL_TRUE,
				sizeof(MenuColoredPoint),
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
		}

		void add_rectangle(
			MenuColoredPoint const * __restrict const points)
		{
			/* Expects points to be in
			 * upleft, bottomleft, bottomright, upright order */
			cpu_buffer.add_memcpy(points, 3);
			cpu_buffer.add_memcpy(points+3, 1);
			cpu_buffer.add_memcpy(points+0, 1);
			cpu_buffer.add_memcpy(points+2, 1);
		}

		void add_rectangle(
			Point2D<int16_t> position,
			Point2D<int16_t> dimensions,
			int16_t layer,
			RGBA<uint8_t> color)
		{
			int16_t const left =
				(int16_t) (position.h());
			int16_t const top =
				(int16_t) (position.v());
			int16_t const bottom =
				(int16_t) (left + dimensions.y);
			int16_t const right =
				(int16_t) (top + dimensions.x);
			MenuColoredPoint const quad[] = {
				MenuColoredPoint({left, top, layer, 1}, color),
				MenuColoredPoint({left, bottom, layer, 1}, color),
				MenuColoredPoint({right, bottom, layer, 1}, color),
				MenuColoredPoint({right, top, layer, 1}, color)
			};
			add_rectangle(quad);
		}

		void add_rectangle(
			Point4D<int16_t> position,
			Point2D<int16_t> dimensions,
			RGBA<uint8_t> color)
		{
			int16_t const left =
				(int16_t) (position.h());
			int16_t const top =
				(int16_t) (position.v());
			int16_t const bottom =
				(int16_t) (position.v() + dimensions.v());
			int16_t const right =
				(int16_t) (position.h() + dimensions.h());
			int16_t const layer = position.p();
			int16_t const pw = 1;
			position.w = 1;
			MenuColoredPoint const quad[] = {
				MenuColoredPoint({left, top, layer, pw}, color),
				MenuColoredPoint({left, bottom, layer, pw}, color),
				MenuColoredPoint({right, bottom, layer, pw}, color),
				MenuColoredPoint({right, top, layer, pw}, color)
			};
			add_rectangle(quad);
		}

		void add_rectangle(
			Point3D<int16_t> position,
			Point2D<int16_t> dimensions,
			RGBA<uint8_t> color)
		{
			int16_t const bottom =
				(int16_t) (position.y - dimensions.y);
			int16_t const right =
				(int16_t) (position.x + dimensions.x);
			MenuColoredPoint const upper_left(
				{position.x, position.y, position.z, 1}, color);
			MenuColoredPoint const bottom_left(
				{position.x, bottom, position.z, 1}, color);
			MenuColoredPoint const bottom_right(
				{right, bottom, position.z, 1}, color);
			add_triangle({upper_left, bottom_left, bottom_right});
			MenuColoredPoint const upper_right(
				{right, position.y, position.z, 1}, color);
			add_triangle({upper_right, upper_left, bottom_right});
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
				area->upper_right()
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
