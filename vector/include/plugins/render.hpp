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

#ifndef render_hpp
#define render_hpp

#include "gamera.hpp"
#include "vectormodule.hpp"
#include "plugins/draw.hpp"
#include <list>
#include <vector>
#include <algorithm>

#define SIGN(n) (n>0?1:-1)

namespace Gamera {
  struct ActiveSegment {
    double m_x;
    double m_slope;
    long m_end_line;
    bool m_new_segment;
    inline ActiveSegment(const Segment& segment) {
      m_slope = segment.slope();
      double y_part = ceil(segment.start().y()) - segment.start().y();
      m_x = segment.start().x() + m_slope * y_part;
      m_end_line = long(floor(segment.end().y()));
      m_new_segment = true;
    }
    inline void increment() {
      m_x += m_slope;
      m_new_segment = false;
    }
  };

  struct ActiveSegmentDone {
    long m_line;
    ActiveSegmentDone(long line) : m_line(line) {}
    bool operator()(const ActiveSegment& l) const {
      return l.m_end_line <= m_line + 1;
    }
  };

  struct SortByStartY {
    bool operator()(const Segment& a, const Segment& b) {
      return a.start().y() < b.start().y();
    }
  };

  struct SortByX {
    bool operator()(const ActiveSegment& a, const ActiveSegment& b) {
      return a.m_x < b.m_x;
    }
  };

  struct SegmentAdder {
    std::vector<Segment>& m_gel;
    SegmentAdder(std::vector<Segment>& gel) : m_gel(gel) {}
    void operator()(const Segment& segment) {
      if (floor(segment.start().y()) == floor(segment.end().y()))
       	return;
      if (segment.end().y() < segment.start().y())
	m_gel.push_back(segment.reverse());
      else
	m_gel.push_back(segment);
    }
  };
    
  template<class T>
  void draw_path_filled(T& image, const boost::shared_ptr<Path> path, 
			typename T::value_type value, double accuracy) {
    // This code is temporary, and converts the temporary Python data
    // structure into a C++ one.

    SortByStartY sort_by_start_y;
    SortByX sort_by_x;
    
    typedef std::vector<Segment> segment_vector;
    typedef std::list<ActiveSegment> active_segment_list;

    segment_vector gel;
    {
      size_t est_length = 0;
      double epsilon;
      size_t size;
      const boost::shared_ptr<PathList> holes = path->holes();
      for (Path::const_iterator i = path->begin(); i != path->end(); ++i) {
	(*i).curve_to_line_segments_epsilon(accuracy, epsilon, size);
	est_length += size;
      }
      for (PathList::const_iterator i = holes->begin(); i != holes->end(); ++i)
	for (Path::const_iterator j = (*i)->begin(); j != (*i)->end(); ++j) {
	  (*j).curve_to_line_segments_epsilon(accuracy, epsilon, size);
	  est_length += size;
	}

      gel.reserve(est_length);
      SegmentAdder segment_adder(gel);
      for (Path::const_iterator i = path->begin(); i != path->end(); ++i)
	(*i).curve_to_line_segments(segment_adder, accuracy);
      for (PathList::const_iterator i = holes->begin(); i != holes->end(); ++i)
	for (Path::const_iterator j = (*i)->begin(); j != (*i)->end(); ++j)
	  (*j).curve_to_line_segments(segment_adder, accuracy);
    }

    std::sort(gel.begin(), gel.end(), sort_by_start_y);

    segment_vector::iterator geli = gel.begin();
    active_segment_list ael;

    // Step 1
    long start_line = (size_t)floor(geli->start().y());
    for (long line = start_line; geli != gel.end() && line < (long)image.nrows(); ++line) {
      // Step 2
      while (long(floor(geli->start().y())) == line && geli != gel.end())
	ael.push_back(ActiveSegment(*(geli++)));
      ael.sort(sort_by_x);

      // Step 3
      if (line >= 0 && ael.size()) {
	active_segment_list::iterator start = ael.begin();
	active_segment_list::iterator i = ael.begin();
	i++;
	bool draw = true;
	for (; i != ael.end(); ++i) {
	  if (draw) {
	    long start_col = std::max((long)(*start).m_x, (long)0);
	    long end_col = std::min((long)(*i).m_x, (long)image.ncols());
	    for (long x = start_col; x < end_col; ++x)
	      image.set((size_t)line, (size_t)x, value);
	  }
	  start = i;
	  draw = !draw;
	}
      }
      
      // Step 4
      ael.remove_if(ActiveSegmentDone(line));
      for (active_segment_list::iterator i = ael.begin(); i != ael.end(); ++i)
	i->increment();
    }
  }

  template<class T>
  void draw_main_path_hollow(T& image, const boost::shared_ptr<Path> path,
			     typename T::value_type value, double accuracy = 0.1) {
    for (Path::const_iterator i = path->begin(); i != path->end(); ++i) {
      if ((*i).is_line())
	draw_line(image, (*i).start().y(), (*i).start().x(),
		  (*i).end().y(), (*i).end().x(), value);
      else
	draw_bezier(image, (*i).start().y(), (*i).start().x(),
		    (*i).c1().y(), (*i).c1().x(),
		    (*i).c2().y(), (*i).c2().x(),
		    (*i).end().y(), (*i).end().x(),
		    value, accuracy);
    }
  }

  template<class T>
  void draw_path_hollow(T& image, const boost::shared_ptr<Path> path, 
			typename T::value_type value, double accuracy = 0.1) {
    const boost::shared_ptr<PathList> holes = path->holes();
    draw_main_path_hollow(image, path, value, accuracy);
    for (PathList::const_iterator i = holes->begin(); i != holes->end(); ++i)
      draw_main_path_hollow(image, *i, value, accuracy);
  }
}

#endif
