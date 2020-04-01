#pragma once

namespace myy {
	template <typename PointType>
	struct Area {
		PointType upper_leftv, bottom_rightv;
		Area() :
			upper_leftv(),
			bottom_rightv()
		{}
		Area(
			PointType const oupper_left,
			PointType const obottom_right) :
			upper_leftv(oupper_left),
			bottom_rightv(obottom_right)
		{}
		Area(PointType const * __restrict const points) :
			upper_leftv(points[0]),
			bottom_rightv(points[1])
		{}
		Area(Area const & other_quad) :
			upper_leftv(other_quad.upper_left()),
			bottom_rightv(other_quad.bottom_right())
		{}
		Area(Area & other_quad) :
			upper_leftv(other_quad.upper_left()),
			bottom_rightv(other_quad.bottom_right())
		{ }
		inline PointType upper_left() const {
			return upper_leftv;
		}
		inline PointType bottom_right() const {
			return bottom_rightv;
		}
		inline PointType upper_left(PointType new_upper_left) {
			upper_leftv = new_upper_left;
			return upper_left();
		}
		inline PointType bottom_right(PointType new_bottom_right) {
			bottom_rightv = new_bottom_right;
			return bottom_right();
		}
		Area & operator=(Area & other_quad) {
			upper_left(other_quad.upper_left());
			bottom_right(other_quad.bottom_right());
			return *this;
		}
		Area & operator=(Area const & other_quad) {
			upper_left(other_quad.upper_left());
			bottom_right(other_quad.bottom_right());
			return *this;
		}
		Area & operator=(Area && other_quad) {
			upper_left(other_quad.upper_left());
			bottom_right(other_quad.bottom_right());
			return *this;
		}
		Area & operator=(Area const && other_quad) {
			upper_left(other_quad.upper_left());
			bottom_right(other_quad.bottom_right());
			return *this;
		}
		bool operator==(Area const & other_quad) {
			return
				(upper_left() == other_quad.upper_left()) &
				(bottom_right() == other_quad.bottom_right());
		}

		PointType * as_two_points() {
			return (PointType *) this;
		}
		PointType const * as_two_points_const() const {
			return (PointType const *) this;
		}

		PointType middle_left() const {
			PointType mleft = upper_left();
			mleft.v(mleft.v() + height());
			return mleft;
		}

		PointType middle_right() const {
			PointType mright = bottom_right();
			mright.v(mright.v() - height());
			return mright;
		}

		PointType upper_right() const {
			PointType uright = upper_left();
			uright.h(bottom_right().h());
			return uright;
		}

		PointType bottom_left() const {
			PointType bleft = bottom_right();
			bleft.h(upper_left().h());
			return bleft;
		}

		/* TODO Find a way to make points comparaisons
		 * flexible.
		 * Currently we only work with Windows coordinates.
		 * However, if we had to use this library with
		 * projects using standard math coordinates, or
		 * other kind of coordinates system (where X isn't
		 * for horizontal, Y isn't for vertical, ...),
		 * we'll have to have a way for the "points" objects
		 * to compare with other points, and indicate whether
		 * that other point is before, at the same level, or
		 * after another one, based on whatever coordinate
		 * system is used for comparaisons.
		 * 
		 * Traits might be a good candidate for this.
		 */
		intmax_t width() const {
			return imaxabs(
				(intmax_t)
				(bottom_right().h() - upper_left().h()));
		}
		intmax_t height() const {
			return imaxabs(
				(intmax_t)
				(bottom_right().v() - upper_left().v()));
		}

		bool contains(PointType const point) {
			return
				(upper_left() <= point) & (point <= bottom_right());
		}
	
		bool contains(PointType const * __restrict const point) {
			return
				(upper_left() <= point) & (point <= bottom_right());
		}
	};
}
