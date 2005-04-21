/*
 *
 * Copyright (C) 2005 Michael Droettboom
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

/*
  For more information about affine transformations, see

  http://en.wikipedia.org/wiki/Transformation_matrix
*/

#ifndef mgd_transformation
#define mgd_transformation

#include "vector.hpp"
#include "floatpoint.hpp"
#include <math.h>

template<class T>
class LineCurveBase;

#define PI_180 M_PI / 180.0

#define TRANSFORMATION_PROPERTY(name) \
T name() const { return m_##name; } \
void name(T _##name) { m_##name = _##name; } \

template<class T>
class TransformationBase {
private:
  T m_scale_x, m_rotate0, m_rotate1, m_scale_y, m_translate_x, m_translate_y;
  typedef PointBase<T> point;
  typedef LineCurveBase<T> line_segment;

public:
  typedef TransformationBase<T> self;

  inline TransformationBase(T a=1.0, T b=0.0, T c=0.0, T d=1.0, T x=0.0, T y=0.0) : 
    m_scale_x(a), m_rotate0(b), m_rotate1(c), m_scale_y(d), 
    m_translate_x(x), m_translate_y(y) {}

  TRANSFORMATION_PROPERTY(scale_x);
  TRANSFORMATION_PROPERTY(scale_y);
  TRANSFORMATION_PROPERTY(rotate0);
  TRANSFORMATION_PROPERTY(rotate1);
  TRANSFORMATION_PROPERTY(translate_x);
  TRANSFORMATION_PROPERTY(translate_y);

  inline point transform(const point& p) const {
    return point(m_scale_x*p.x() + m_rotate1*p.y() + m_translate_x, 
		 m_scale_y*p.y() + m_rotate0*p.x() + m_translate_y);
  }

  inline line_segment transform(const line_segment& l) const {
    return l.transform(*this);
  }

  self get_inverse_transformation() const {
    T d = m_scale_y * m_scale_x + m_rotate0 * m_rotate1;
    return self(m_scale_y / d,
		-m_rotate0 / d,
		-m_rotate1 / d,
		m_scale_x / d,
		(m_translate_y * m_rotate1 - m_translate_x * m_scale_y) / d,
		(-m_translate_y * m_scale_x + m_translate_x * m_rotate0) / d);
  }

  point inverse_transform(const point& p) const {
    return get_inverse_transformation().transform(p);
  }

  line_segment inverse_transform(const line_segment& l) const {
    self i = get_inverse_transformation();
    return l.transform(i);
  }

  bool is_identity() const {
    return (m_scale_x == 1.0 && m_scale_y == 1.0 &&
	    m_rotate0 == 0.0 && m_rotate1 == 0.0 &&
	    m_translate_x == 0.0 && m_translate_y == 0.0);
  }

  bool is_rectilinear() const {
    return ((std::abs(m_rotate0) < std::numeric_limits<T>::epsilon() && 
	     std::abs(m_rotate0) < std::numeric_limits<T>::epsilon()) ||
	    (std::abs(m_scale_x) < std::numeric_limits<T>::epsilon() && 
	     std::abs(m_scale_y) < std::numeric_limits<T>::epsilon()));
  }

  T expansion_factor() const {
    return std::sqrt(std::abs(m_scale_x * m_scale_y - m_rotate0 * m_rotate1));
  }

  bool operator==(const self& other) const {
    T e = std::numeric_limits<T>::epsilon();
    return (std::abs(m_scale_x - other.scale_x()) < e &&
	    std::abs(m_rotate0 - other.rotate0()) < e &&
	    std::abs(m_rotate1 - other.rotate1()) < e &&
	    std::abs(m_scale_y - other.scale_y()) < e &&
	    std::abs(m_translate_x - other.translate_x()) < e &&
	    std::abs(m_translate_y - other.translate_y()) < e);
  }

  bool operator!=(const self& other) const {
    return !((*this) == other);
  }

  self operator*(const self& other) const {
    const T a00 = m_scale_x;
    const T a01 = m_rotate0;
    const T a10 = m_rotate1;
    const T a11 = m_scale_y;
    const T a20 = m_translate_x;
    const T a21 = m_translate_y;
    const T b00 = other.scale_x();
    const T b01 = other.rotate0();
    const T b10 = other.rotate1();
    const T b11 = other.scale_y();
    const T b20 = other.translate_x();
    const T b21 = other.translate_y();

    return self(a00*b00 + a01*b10,
		a00*b01 + a01*b11,
		a10*b00 + a11*b10,
		a10*b01 + a11*b11,
 		a20*b00 + a21*b10 + b20,
		a20*b01 + a21*b11 + b21);
  }
};

template<class T>
std::ostream& operator<<(std::ostream& s, const TransformationBase<T>& t) {
  return s << "Transformation(" << t.scale_x() << ", " << t.rotate0() << 
    ", " << t.rotate1() << ", " << t.scale_y() << ", " << t.translate_x() << 
    ", " << t.translate_y() << ")";
}

typedef TransformationBase<double> Transformation;

inline Transformation translate(NumberType x, NumberType y) {
  return Transformation(1, 0, 0, 1, x, y);
}

inline Transformation translate(FloatPoint p) {
  return translate(p.x(), p.y());
}

inline Transformation translate(NumberType s) {
  return translate(s, s);
}

inline Transformation transform_at(FloatPoint p, Transformation t) {
  return translate(-p) * t * translate(p);
}

inline Transformation scale(NumberType x, NumberType y) {
  return Transformation(x, 0, 0, y, 0, 0);
}

inline Transformation scale(FloatPoint p) {
  return scale(p.x(), p.y());
}

inline Transformation scale(NumberType s) {
  return scale(s, s);
}

inline Transformation scale_at(FloatPoint p, NumberType x, NumberType y) {
  return transform_at(p, scale(x, y));
}

inline Transformation scale_at(FloatPoint p, FloatPoint p2) {
  return scale_at(p, p2.x(), p2.y());
}

inline Transformation scale_at(FloatPoint p, NumberType s) {
  return scale_at(p, s, s);
}

inline Transformation shear(NumberType x, NumberType y) {
  return Transformation(1, y, x, 1, 0, 0);
}

inline Transformation shear(FloatPoint p) {
  return shear(p.x(), p.y());
}

inline Transformation shear(NumberType s) {
  return shear(s, s);
}

inline Transformation shear_radians(NumberType x, NumberType y) {
  return Transformation(1, tan(y), tan(x), 1, 0, 0);
}

inline Transformation shear_radians(FloatPoint p) {
  return shear_radians(p.x(), p.y());
}

inline Transformation shear_radians(NumberType s) {
  return shear_radians(s, s);
}

inline Transformation shear_degrees(NumberType x, NumberType y) {
  return shear_radians(x * PI_180, y * PI_180);
}

inline Transformation shear_degrees(FloatPoint p) {
  return shear_degrees(p.x(), p.y());
}

inline Transformation shear_degrees(NumberType s) {
  return shear_degrees(s, s);
}

inline Transformation rotate_radians(NumberType r) {
  NumberType s = sin(r);
  NumberType c = cos(r);
  return Transformation(c, s, -s, c, 0, 0);
}

inline Transformation rotate_degrees(NumberType d) {
  return rotate_radians(d * PI_180);
}

inline Transformation rotate_radians_at(FloatPoint p, NumberType r) {
  return transform_at(p, rotate_radians(r));
}

inline Transformation rotate_degrees_at(FloatPoint p, NumberType d) {
  return transform_at(p, rotate_degrees(d));
}

inline Transformation reflect(bool x, bool y) {
  return Transformation(x?-1:1, 0, 0, y?-1:1, 0, 0);
}

inline Transformation reflect_at(FloatPoint p, bool x, bool y) {
  return transform_at(p, reflect(x, y));
}

inline Transformation identity() {
  return Transformation();
}

#endif
