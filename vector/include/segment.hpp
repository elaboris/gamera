/*
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

#ifndef mgd_line_curve
#define mgd_line_curve

#include "vector.hpp"
#include "floatpoint.hpp"
#include "transformation.hpp"
#include "path.hpp"
#include <math.h>

template<class T>
class PathBase;

template<class T>
class SegmentBase {
  typedef SegmentBase<T> self;
  typedef PointBase<T> point;
  typedef TransformationBase<T> transformation;
  typedef PathBase<T> path;
  typedef boost::shared_ptr<path> path_ptr;
  point m_start;
  point m_end;
  point m_c1;
  point m_c2;
  bool m_is_line;

public:
  SegmentBase(point start, point end) : 
    m_start(start), m_end(end), m_is_line(true) {}
  SegmentBase(point start, point c1, point c2, point end) :
    m_start(start), m_end(end), m_c1(c1), m_c2(c2), m_is_line(false) {}

  point start() const { return m_start; }
  point end() const { return m_end; }
  point c1() const { 
    if (m_is_line)
      throw std::runtime_error("Cannot get a control point from a line.");
    return m_c1;
  }
  point c2() const { 
    if (m_is_line)
      throw std::runtime_error("Cannot get a control point from a line.");
    return m_c2;
  }
  bool is_line() const { return m_is_line; }

  bool operator==(const self& other) const {
    if (m_is_line && other.is_line())
      return (m_start == other.start() && m_end == other.end());
    else if (!m_is_line && !other.is_line())
      return (m_start == other.start() && m_c1 == other.c1() &&
	      m_c2 == other.c2() && m_end == other.end());
    return false;
  }

  bool operator!=(const self& other) const {
    return !((*this) == other);
  }

private:
  class add_distance {
  public:
    T m_distance;
    add_distance() : m_distance(0) {}
    void operator()(const self& l) {
      m_distance += l.start().distance(l.end());
    }
  };
  
public:
  T length(T accuracy = 0.1) const {
    if (m_is_line)
      return m_start.distance(m_end);
    else {
      add_distance distance_adder;
      curve_to_line_segments(distance_adder, accuracy);
      return distance_adder.m_distance;
    }
  }

  T slope() const {
    if (m_end.y() == m_start.y()) {
      if (m_end.x() < m_start.x())
	return -std::numeric_limits<T>::infinity();
      else
	return std::numeric_limits<T>::infinity();
    }
    return (m_end.x() - m_start.x()) / (m_end.y() - m_start.y());
  }

  self reverse() const {
    if (m_is_line)
      return self(m_end, m_start);
    else
      return self(m_end, m_c2, m_c1, m_start);
  }

  self transform(const transformation& t) const {
    if (m_is_line)
      return self(t.transform(m_start), t.transform(m_end));
    else
      return self(t.transform(m_start), t.transform(m_c1), 
		  t.transform(m_c2), t.transform(m_end));
  }

private:
  class add_line {
  private:
    path_ptr m_path;
  public:
    add_line(path_ptr path) : m_path(path) {}
    void operator()(const self& l) {
      m_path->push_back(l);
    }
  };

public:
  path_ptr convert_to_path(T accuracy = 0.1) const {
    T epsilon;
    size_t size;
    curve_to_line_segments_epsilon(accuracy, epsilon, size);
    path_ptr p(new path());
    p->reserve(size);
    add_line line_adder(p);
    curve_to_line_segments_algorithm(line_adder, epsilon);
    return p;
  }

  void curve_to_line_segments_epsilon(T accuracy = 0.1, T& epsilon, size_t& size) const {
    if (m_is_line) {
      epsilon = 1.0;
      size = 1;
    } else {
      T dd0x = m_start.x() - 2 * m_c1.x() + m_c2.x();
      T dd0y = m_start.y() - 2 * m_c1.y() + m_c2.y();
      T dd0  = dd0x*dd0x + dd0y*dd0y;
      
      T dd1x = m_c1.x() - 2 * m_c2.x() + m_end.x();
      T dd1y = m_c1.y() - 2 * m_c2.y() + m_end.y();
      T dd1  = dd1x*dd1x + dd1y*dd1y;
      
      T dd = 6.0 * sqrt(std::max(dd0, dd1));
      T e2 = 8.0 * accuracy;
      
      if (e2 < dd && dd != 0.0)
	epsilon = std::sqrt(e2 / dd);
      else
	epsilon = 1.0;
      size = size_t(1.0 / epsilon + 0.5);
    }
  }

  template<class Functor>
  void curve_to_line_segments_algorithm(Functor& functor, T epsilon) const {
    if (m_is_line)
      functor(*this);
    else {
      T a = 1.0;
      T b = 0.0;
      point p0 = m_start;
      point p1 = m_end;
      while (a > 0.0) {
	T a_3 = a * a * a;
	T a_2_b = a * a * b * 3.0;
	T b_3 = b * b * b;
	T b_2_a = b * b * a * 3.0;
	p1 = point(m_start.x()*a_3 + m_c1.x()*a_2_b + m_c2.x()*b_2_a + m_end.x()*b_3,
		   m_start.y()*a_3 + m_c1.y()*a_2_b + m_c2.y()*b_2_a + m_end.y()*b_3);
	functor(self(p0, p1));
	p0 = p1;
	a -= epsilon;
	b += epsilon;
      }
      functor(self(p1, m_end));
    }
  }

  template<class Functor>
  void curve_to_line_segments(Functor& functor, T accuracy = 0.1) const {
    T epsilon;
    size_t size;
    curve_to_line_segments_epsilon(accuracy, epsilon, size);
    curve_to_line_segments_algorithm(functor, epsilon);
  }
};

template<class T>
std::ostream& operator<<(std::ostream& s, const SegmentBase<T>& line) {
  if (line.is_line())
    return s << "Segment(" << line.start() << ", " << line.end() << ")";
  else
    return s << "Segment(" << line.start() << ", " << line.c1() << ", " << line.c2() << ", " << line.end() << ")";
}

typedef SegmentBase<NumberType> Segment;

#endif
