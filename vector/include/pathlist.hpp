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

#ifndef mgd_pathlist
#define mgd_pathlist

#include "vector.hpp"
#include "path.hpp"
#include "transformation.hpp"

template<class T>
class PathBase;

template<class T>
class PathListBase : public std::vector<boost::shared_ptr<PathBase<T> > > {
public:
  typedef PathBase<T> path;
  typedef boost::shared_ptr<path> path_ptr;
  typedef std::vector<path_ptr> base;
  typedef PathListBase<T> self;
  typedef boost::shared_ptr<self> self_ptr;
  typedef TransformationBase<T> transformation;

  using base::back;
  using base::begin;
  using base::end;
  using base::size;
  using typename base::iterator;
  using typename base::const_iterator;

  PathListBase() : base() {
#ifdef MEM_DEBUG
    std::cerr << "created PathList @ " << &(*this) << "\n";
#endif
  }

  ~PathListBase() {
#ifdef MEM_DEBUG
    std::cerr << "deleted PathList @ " << &(*this) << "\n";
#endif
  }

  //////////////////////////////////////////////////////////////////////
  // Basic operations
  self_ptr transform(const transformation& t) const {
    self_ptr new_paths(new self());
    new_paths->reserve(size());
    for (typename base::const_iterator i = begin(); i != end(); ++i)
      new_paths->push_back((*i)->transform(t));
    return new_paths;
  }

  void transform_in_place(const transformation& t) {
    for (typename base::iterator i = begin(); i != end(); ++i)
      (*i)->transform_in_place(t);
  }

  T area() const {
    T a = 0;
    for (typename base::const_iterator i = begin(); i != end(); ++i)
      a += (*i)->area();
    return a;
  }
};

template<class T>
std::ostream& operator<<(std::ostream& s, const PathListBase<T>& path_list) {
  s << "PathList(length " << path_list.size() << ")";
  return s;
}

typedef PathListBase<NumberType> PathList;

#endif
