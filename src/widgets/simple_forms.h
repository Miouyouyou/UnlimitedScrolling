#ifndef MYY_WIDGETS_SIMPLE_FORMS
#define MYY_WIDGETS_SIMPLE_FORMS 1

#include <stdint.h>
#include <myy/helpers/position.h>
#include <myy/helpers/vector.h>
#include <myy/helpers/colors.h>

#ifdef __cplusplus

namespace myy {
	template <typename T>
	struct TexCoords {
		T s, t;
		TexCoords() :
			s((T)0), t((T)0)
		{ }
		TexCoords(T const cs, T const ct) :
			s(cs), t(ct)
		{ }
		TexCoords(T const * __restrict const coords) :
			s(coords[0]),
			t(coords[1])
		{ }
		TexCoords(TexCoords const & ot) :
			s(ot.s),
			t(ot.t)
		{ }
		TexCoords & operator=(TexCoords & ot)
		{ s = ot.s; t = ot.t; }
		TexCoords & operator=(TexCoords && ot)
		{ s = ot.s; t = ot.t; }
	};

	template <typename T>
	struct Point4D {
		typedef T ComponentType;
		T x, y, z, w;
		Point4D() :
			x((T)0),
			y((T)0),
			z((T)0),
			w((T)0)
		{}
		Point4D(T const ox, T const oy, T const oz, T const ow) :
			x(ox),
			y(oy),
			z(oz),
			w(ow)
		{}
		Point4D(T const * __restrict const coords) :
			x(coords[0]),
			y(coords[1]),
			z(coords[2]),
			w(coords[3])
		{}
		Point4D(Point4D const & op) :
			x(op.x),
			y(op.y),
			z(op.z),
			w(op.w)
		{}
		Point4D & operator=(Point4D const & op)
		{ 
			x = op.x; y = op.y; z = op.z; w = op.w;
			return *this;
		}
		Point4D & operator=(Point4D const && op)
		{ 
			x = op.x; y = op.y; z = op.z; w = op.w;
			return *this;
		}

		void set(
			T const ox,
			T const oy,
			T const oz,
			T const ow)
		{
			x = ox;
			y = oy;
			z = oz;
			w = ow;
		}

		void set(
			T const ox,
			T const oy)
		{
			x = ox;
			y = oy;
		}

		T h() const { return x; }
		T h(T new_h) { x = new_h; return x; }
		T v() const { return y; }
		T v(T new_v) { y = new_v; return y; }

		position_S_4D to_pos_S_4D() const {
			return position_S_4D_struct(
				(int16_t)x,
				(int16_t)y,
				(int16_t)z,
				(int16_t)w);
		}
	};

	template <typename T>
	struct RGBA {
		T r, g, b, a;
		RGBA() :
			r((T)0),
			g((T)0),
			b((T)0),
			a((T)0)
		{ }
		RGBA(T const cr, T const cg, T const cb, T const ca) :
			r(cr),
			g(cg),
			b(cb),
			a(ca)
		{ }
		RGBA(T const * __restrict const coords) :
			r(coords[0]),
			g(coords[1]),
			b(coords[2]),
			a(coords[3])
		{ }
		RGBA(RGBA const & oc) :
			r(oc.r),
			g(oc.g),
			b(oc.b),
			a(oc.a)
		{ }
		RGBA & operator=(RGBA const & oc)
		{ 
			r = oc.r;
			g = oc.g;
			b = oc.b;
			a = oc.a;
		}
		RGBA & operator=(RGBA const && oc)
		{ 
			r = oc.r;
			g = oc.g;
			b = oc.b;
			a = oc.a;
		}
	};

	template <typename Point_T, typename RGBA_T>
	struct ColoredPoint {
		Point_T point;
		RGBA_T color;
		ColoredPoint() :
			point(Point_T()),
			color(RGBA_T())
		{ }
		ColoredPoint(
			Point_T const opoint,
			RGBA_T const ocolor) :
			point(opoint),
			color(ocolor)
		{ }
		ColoredPoint(ColoredPoint const & op) :
			point(op.point),
			color(op.color)
		{ }
		ColoredPoint & operator=(ColoredPoint const & oc)
		{ 
			point = oc.point;
			color = oc.color;
		}
		ColoredPoint & operator=(ColoredPoint const && oc)
		{ 
			point = oc.point;
			color = oc.color;
		}
		typename Point_T::ComponentType h() { return point.h(); }
		typename Point_T::ComponentType h(typename Point_T::ComponentType new_h) {
			return point.h(new_h);
		}
		typename Point_T::ComponentType v() { return point.v(); }
		typename Point_T::ComponentType v(typename Point_T::ComponentType new_v) {
			return point.v(new_v);
		}
	};

};

extern "C" {
#endif
struct simple_rgb_point {
	position_S pos;
	int16_t z, w;
	struct rgba8 color;
};
myy_vector_template(rgb_points, struct simple_rgb_point)

__attribute__((unused))
static inline struct simple_rgb_point simple_rgb_point_struct(
	position_S const pos, int16_t depth,
	struct rgba8 const color)
{
	struct simple_rgb_point const point = {
		.pos   = pos,
		.z     = depth,
		.w     = 1,
		.color = color
	};

	return point;
}

__attribute__((unused))
static inline void simple_rgb_triangle(
	myy_vector_rgb_points * __restrict const points,
	int16_t depth,
	position_S const a, position_S const b, position_S const c,
	struct rgba8 const color)
{
	struct simple_rgb_point triangle_vertices[3] = {
		simple_rgb_point_struct(a, depth, color),
		simple_rgb_point_struct(b, depth, color),
		simple_rgb_point_struct(c, depth, color)
	};
	myy_vector_rgb_points_add(points, 3, triangle_vertices);
}

__attribute__((unused))
static inline void simple_rgb_quad(
	myy_vector_rgb_points * __restrict const points,
	int16_t const depth,
	position_S const down_left, position_S const up_right,
	struct rgba8 const color)
{
	position_S const up_left = {
		.x = down_left.x,
		.y = up_right.y
	};
	position_S const down_right = {
		.x = up_right.x,
		.y = down_left.y
	};
	struct simple_rgb_point two_triangles_quad_vertices[6] = {
		simple_rgb_point_struct(up_left, depth, color),
		simple_rgb_point_struct(down_left, depth, color),
		simple_rgb_point_struct(up_right, depth, color),

		simple_rgb_point_struct(down_right, depth, color),
		simple_rgb_point_struct(up_right, depth, color),
		simple_rgb_point_struct(down_left, depth, color)
	};

	myy_vector_rgb_points_add(points, 6, two_triangles_quad_vertices);
}

#ifdef __cplusplus
}
#endif
#endif
