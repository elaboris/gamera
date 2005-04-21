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

#ifndef tracing_hpp
#define tracing_hpp

extern "C" {
#include "bitmap.h"
#include "lists.h"
#include "path.h"
#include "main.h"
}

#include "vectormodule.hpp"

struct info info;

namespace Gamera {

  // potrace uses packed bits bitmaps (and nothing else),
  // so we have to copy the data.
  // Uses potrace macro BM_UPUT to do so.
  template<class T>
  bitmap_t* gamera_to_potrace_bitmap(T& m) {
    // Potrace seems to like a little border around the glyph,
    // so we pad two pixels around the image accordingly.  
    // No big deal, since we're copying anyway...
    bitmap_t* bm = bm_new((int)m.ncols() + 4, (int)m.nrows() + 4);
    bm_clear(bm, 0);
    for (size_t y = 0; y != m.nrows(); ++y) 
      for (size_t x = 0; x != m.ncols(); ++x) 
	BM_UPUT(bm, x + 2, y + 2, is_black(m.get(y, x)));
    return bm;
  }

  boost::shared_ptr<Path> make_path(curve_t* curve, int m, double offset_x, double offset_y) {
    boost::shared_ptr<Path> path(new Path());
    dpoint_t *c;
    c = curve[m-1].c;
    FloatPoint start(c[2].x + offset_x, c[2].y + offset_y);
    path->add_segment(Segment(start, start));

    for (int i = 0; i < m; ++i) {
      c = curve[i].c;
      switch (curve[i].tag) {
      case CORNER:
	path->add_segment(FloatPoint(c[1].x + offset_x, c[1].y + offset_y));
	path->add_segment(FloatPoint(c[2].x + offset_x, c[2].y + offset_y));
	break;
      case CURVETO:
	path->add_segment(FloatPoint(c[0].x + offset_x, c[0].y + offset_y),
			  FloatPoint(c[1].x + offset_x, c[1].y + offset_y),
			  FloatPoint(c[2].x + offset_x, c[2].y + offset_y));
	break;
      }
    }
    return path;
  }

  // This is basically a new "backend" for potrace that converts
  // to a Paths object.
  template<class T>
  boost::shared_ptr<PathList> potrace_path_to_path_list(T& image, path_t* tree) {
    // Since the image passed to potrace was translated by (2, 2) 
    // we must translate back on the output.
    double offset_x = (double)image.ul_x() - 2;
    double offset_y = (double)image.ul_y() - 2;
    path_t* p;
    boost::shared_ptr<PathList> path_list(new PathList());
    for (p=tree; p; p=p->sibling) {
      boost::shared_ptr<Path> path = make_path(p->fcurve, p->fm, offset_x, offset_y);
      for (path_t* q=p->childlist; q; q=q->sibling)
	path->holes()->push_back(make_path(q->fcurve, q->fm, offset_x, offset_y));
      path_list->push_back(path);
    }
    return path_list;
  }

  template<class T>
  boost::shared_ptr<PathList> potrace(T& image, int turd_size=2, int turn_policy=4, 
				      bool optimize_curve=true, double optimize_tolerance=0.2,
				      double alphamax=1.0) {
    info.turdsize = turd_size;
    info.turnpolicy = turn_policy;
    info.opticurve = optimize_curve ? 1 : 0;
    info.alphamax = alphamax;
    info.opttolerance = optimize_tolerance;

    bitmap_t* bm = NULL;
    bm = gamera_to_potrace_bitmap(image);
    path_t* plist = NULL;
    int r = bm_to_pathlist(bm, &plist);
    if (r) {
      throw std::runtime_error("[potrace] Problem converting to pathlist");
    }
    bm_free(bm);

    r = process_path(plist);
    if (r) {
      throw std::runtime_error("[potrace] Problem converting to pathlist");
    }

    boost::shared_ptr<PathList> path_list = potrace_path_to_path_list(image, plist);
    pathlist_free(plist);
    return path_list;
  }
}

#endif
