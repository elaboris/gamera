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

#ifndef deform_hpp
#define deform_hpp

#include "gamera.hpp"
#include "vectormodule.hpp"

#define MIN4(a,b,c,d) std::min(std::min(a, b), std::min(c, d))
#define MAX4(a,b,c,d) std::max(std::max(a, b), std::max(c, d))

template<class T>
typename ImageFactory<T>::view_type* affine_transform_image(const T& src, const Transformation& t, bool transform_bb=false) {
  // First, transform the bounding box
  FloatPoint ul(src.ul());
  FloatPoint lr(src.lr());
  FloatPoint ur(src.ur());
  FloatPoint ll(src.ll());
  FloatPoint tul = t.transform(ul);
  FloatPoint tlr = t.transform(lr);
  FloatPoint tur = t.transform(ur);
  FloatPoint tll = t.transform(ll);
  FloatPoint new_ul = FloatPoint(MIN4(tul.x(), tlr.x(), tur.x(), tll.x()),
				 MIN4(tul.y(), tlr.y(), tur.y(), tll.y()));
  FloatPoint new_lr = FloatPoint(MAX4(tul.x(), tlr.x(), tur.x(), tll.x()),
				 MAX4(tul.y(), tlr.y(), tur.y(), tll.y()));
  FloatPoint real_ul, real_lr;
  if (transform_bb) {
    real_ul = new_ul = FloatPoint(std::max(0.0, new_ul.x()), std::max(0.0, new_ul.y()));
    real_lr = new_lr = FloatPoint(std::max(0.0, new_lr.x()), std::max(0.0, new_lr.y()));  
  } else {
    real_ul = new_ul;
    real_lr = new_lr;
    if (real_ul.x() < 0) {
      real_lr = FloatPoint(real_lr.x() - real_ul.x(), real_lr.y());
      real_ul = FloatPoint(0, real_ul.y());
    }
    if (real_ul.y() < 0) {
      real_lr = FloatPoint(real_lr.x(), real_lr.y() - real_ul.y());
      real_ul = FloatPoint(real_ul.x(), 0);
    }
  }
  typename ImageFactory<T>::data_type* dest_data =
    new typename ImageFactory<T>::data_type(size_t(real_lr.y() - real_ul.y()),
					    size_t(real_lr.x() - real_ul.x()),
					    size_t(real_ul.y()), 
					    size_t(real_ul.x()));
  typename ImageFactory<T>::view_type* dest =
    new typename ImageFactory<T>::view_type(*dest_data);

  Transformation it = t.get_inverse_transformation();
  typename choose_accessor<T>::interp_accessor acc = 
    choose_accessor<T>::make_interp_accessor(src);
  
  for (size_t r = 0; r < dest->nrows(); ++r) {
    for (size_t c = 0; c < dest->ncols(); ++c) {
      FloatPoint dp = FloatPoint(c, r) + new_ul;
      FloatPoint op = it.transform(dp) - ul;
      if (op.x() >= 0 && op.x() < src.ncols() - 1 &&
	  op.y() >= 0 && op.y() < src.nrows() - 1) {
	typename T::value_type px = acc(src.upperLeft(), op.x(), op.y());
	dest->set(r, c, px);
      }
    }
  }
  return dest;
}

#endif
