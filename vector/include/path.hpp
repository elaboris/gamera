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

#ifndef mgd_path
#define mgd_path

#include "vector.hpp"
#include "segment.hpp"
#include "pathlist.hpp"

class path_exception : public std::runtime_error {
public:
  path_exception(char* message) : std::runtime_error(message) {}
};

template<class T>
class SegmentBase;

template<class T>
class PathListBase;

template<class T>
class PathBase : public std::vector<SegmentBase<T> > {
public:
  typedef SegmentBase<T> segment;
  typedef PointBase<T> point;
  typedef std::vector<segment> base;
  typedef TransformationBase<T> transformation;
  typedef PathBase<T> self;
  typedef boost::shared_ptr<self> self_ptr;
  typedef PathListBase<T> path_list;
  typedef boost::shared_ptr<path_list> path_list_ptr;

  using base::back;
  using base::begin;
  using base::end;
  using base::size;
  using typename base::iterator;
  using typename base::const_iterator;

  PathBase() : base(), m_patching_mode(false), m_holes(new path_list()) {
#ifdef MEM_DEBUG
    std::cerr << "created Path @ " << &(*this) << "\n";
#endif
  }
  PathBase(size_t n) : base(n), m_patching_mode(false), m_holes(new path_list()) {
#ifdef MEM_DEBUG
    std::cerr << "created Path @ " << &(*this) << "\n";
#endif
  }
  PathBase(size_t n, const segment& l) : base(n, l), m_patching_mode(false), m_holes(new path_list()) {
#ifdef MEM_DEBUG
    std::cerr << "created Path @ " << &(*this) << "\n";
#endif
  }

  ~PathBase() { 
#ifdef MEM_DEBUG
    std::cerr << "deleted Path @ " << &(*this) << "\n";
#endif
  }

  void patching_mode(bool mode) { m_patching_mode = mode; }
  bool patching_mode() const { return m_patching_mode; }
  path_list_ptr holes() const { return m_holes; }

  void add_segment(const point& p) {
    if (size())
      push_back(segment(back().end(), p));
    else
      push_back(segment(p, p));
  }

  void add_segment(const point& c1, const point& c2, const point& p) {
    if (size())
      push_back(segment(back().end(), c1, c2, p));
    else
      push_back(segment(p, p));
  }

  void add_segment(const segment& s) {
    push_back(s);
  }

  //////////////////////////////////////////////////////////////////////
  // Mimic the STL std::vector

  // TODO: this needs to work somehow...  For now it
  //       will raise an exception.
  typename base::reference operator[](size_t n) {
    throw std::runtime_error("The 'path[n] = s;' is not implemented.  Use iterators instead for now.");
  }

  void setitem(size_t index, const segment& l) {
    if (index >= size() || index < 0)
      throw std::range_error("Index is out of range for path.");
    insert(begin() + index, l);
  }

  void push_back(const segment& l) {
    if (begin() != end())
      if (back().end() != l.start()) {
	if (m_patching_mode)
	  this->base::push_back(segment(back().end(), l.start()));
	else 
	  throw path_exception("The end of the path is not equal to the start of the line being appended.  This would create a discontinuity in the path.");
      }
    this->base::push_back(l);
  }
  
  void insert(typename base::iterator pos, const segment& l) {
    if (pos != begin())
      if ((*(pos - 1)).end() != l.start()) {
	if (m_patching_mode) {
	  this->base::insert(pos, segment((*(pos-1)).end(), l.start()));
	  ++pos;
	} else {
	  throw path_exception("The start of the segment being inserted does not match the end of the segment preceding it in the path.  This would create a discontinuity in the path.");
	}
      }
    if (pos != end())
      if ((*pos).start() != l.end()) {
	if (m_patching_mode) 
	  this->base::insert(pos, segment(l.end(), (*pos).start()));
	else
	  throw path_exception("The end of the segment being inserted does not match the start of the next segment in the path.  This would create a discontinuity in the path.");
      }
    this->base::insert(pos, l);
  }

  template<class InputIterator>
  void insert(typename base::iterator pos, InputIterator f, InputIterator l) {
    if (f == l)
      return;
    if (pos != begin())
      if ((*(pos - 1)).end() != (*f).start()) {
	if (m_patching_mode) {
	  this->base::insert(pos, segment((*(pos-1)).end(), (*f).start()));
	  ++pos;
	} else 
	  throw path_exception("The start of the segments being inserted does not match the end of the segment preceding them in the path.  This would create a discontinuity in the path.");
      }
    if (pos != end())
      if ((*pos).start() != (*(l-1)).end()) {
	if (m_patching_mode) 
	  this->base::insert(pos, segment((*(l-1)).end(), (*pos).start()));
	else
	  throw path_exception("The end of the segments being inserted does not match the start of the next segment in the path.  This would create a discontinuity in the path.");
      }
    this->base::insert(pos, f, l);
  }

  void insert(typename base::iterator pos, size_t n, const segment& l) {
    if (n > 1)
      if (l.start() != l.end())
	throw path_exception("The start and end of the segment being inserted is not the same.  This would create a discontinuity in the path.");
    if (pos != begin())
      if ((*(pos - 1)).end() != l.start())
	throw path_exception("The start of the segment being inserted does not match the end of the segment preceding it in the path.  This would create a discontinuity in the path.");
    if (pos != end())
      if ((*pos).start() != l.end())
	throw path_exception("The end of the segment being inserted does not match the start of the next segment in the path.  This would create a discontinuity in the path.");
    this->base::insert(pos, n, l);
  }

  typename base::iterator erase(typename base::iterator pos) {
    if ((*pos).start != (*pos).end) 
	throw path_exception("Erasing that element would create a discontinuity in the path.");
    return this->base::erase(pos);
  }

  typename base::iterator erase(typename base::iterator first, typename base::iterator last) {
    if (first != begin() && last != end() - 1)
      if (*(first - 1).end() != *(end + 1).start()) 
	throw path_exception("Erasing those elements would create a discontinuity in the path.");
    return this->base::erase(first, last);
  }

  //////////////////////////////////////////////////////////////////////
  // Path operations

  self_ptr transform(const transformation& t) const {
    self_ptr new_path(new self());
    new_path->reserve(size());
    for (typename base::const_iterator i = begin();
	 i != end(); ++i) {
      new_path->push_back((*i).transform(t));
    }
    path_list_ptr new_holes = new_path->holes();
    new_holes->reserve(m_holes->size());
    for (typename path_list::const_iterator i = m_holes->begin();
	 i != m_holes->end(); ++i) {
      new_holes->push_back((*i)->transform(t));
    }
    return new_path;
  }

  void transform_in_place(const transformation& t) {
    for (typename base::iterator i = begin();
	 i != end(); ++i)
      *i = (*i).transform(t);
    for (typename path_list::iterator i = m_holes->begin();
	 i != m_holes->end(); ++i) 
      (*i)->transform_in_place(t);
  }

  bool is_closed() const {
    if (size())
      return back().end() == (*begin()).start();
    else
      return true;
  }

  self_ptr convert_to_lines(T accuracy = 0.1) const {
    self_ptr new_path(new self());
    for (typename base::const_iterator i = begin(); i != end(); ++i) {
      self_ptr sub_path = (*i).convert_to_path(accuracy);
      new_path->insert(new_path->end(), sub_path->begin(), sub_path->end());
    }
    path_list_ptr new_holes = new_path->holes();
    new_holes->reserve(m_holes->size());
    for (typename path_list::const_iterator i = m_holes->begin();
	 i != m_holes->end(); ++i) {
      new_holes->push_back((*i)->convert_to_lines(accuracy));
    }
    return new_path;
  }

  T area() const {
    // TODO: This could use the callback mechanism in segment to
    //       avoid using all this temporary memory
    if (!is_closed())
      throw path_exception("The area of an open path can not be determined.");
    if (size() == 0)
      return 0;
    self_ptr new_path = convert_to_lines();
    T a = 0.0;
    typename base::const_iterator first = new_path->begin();
    typename base::const_iterator last = new_path->end() - 1;  
    typename base::const_iterator second, second_last;
    if (new_path->size() > 1) {
      second = first + 1;
      second_last = last - 1;
    } else {
      second = first;
      second_last = last;
    }
    a = (first->end().x() * (second->end().y() - last->end().y()) +
	 last->end().y() * (first->end().y() - second_last->end().y()));
    for (typename base::const_iterator i = second; i != last; ++i)
      a += i->end().x() * ((i+1)->end().y() - (i-1)->end().y());
    a /= 2.0;
    
    for (typename path_list::iterator i = m_holes->begin();
	 i != m_holes->end(); ++i) 
      a -= (*i)->area();

    return a;
  }

private:
  bool m_patching_mode;
  path_list_ptr m_holes;
};

template<class T>
std::ostream& operator<<(std::ostream& s, const PathBase<T>& path) {
  s << "Path(";
  bool first = true;
  typename PathBase<T>::const_iterator i = path.begin();
  size_t n = 0;
  for (; i != path.end() && n < 16; ++i, ++n) {
    if (first) 
      first = false;
    else
      s << ", ";
    s << (*i);
  }
  if (n == 16)
    s << "...";
  s << " " << path.holes()->size() << " holes)";
  return s;
}

typedef PathBase<NumberType> Path;

#endif
