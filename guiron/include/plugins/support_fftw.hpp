/*
 * Copyright (C) 2005 Robert Ferguson
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

#ifndef lcninja_support_fftw
#define lcninja_support_fftw

#include "plugins/image_utilities.hpp"
#include "plugins/arithmetic.hpp"
#include "plugins/deformations.hpp"
#include <fftw3.h>


/* FFT Utility Functions
 *
 * The functions in this section are general useful for 
 * fourier functions, but not particularly useful for anything
 * else.
 */

inline ComplexPixel squaredMagnitude(ComplexPixel r) {
  return r.real() * r.real() + r.imag() * r.imag();
}

inline ComplexPixel magnitude(ComplexPixel r) {
  return sqrt(squaredMagnitude(r));
}

inline ComplexPixel phase(ComplexPixel r) {
  return atan2(r.imag(), r.real());
}

template<class T> 
inline fftw_complex* copy_gamera_image_to_fftw_complex(const T& src) {
  fftw_complex* output = static_cast<fftw_complex *>(fftw_malloc(sizeof(fftw_complex) * src.nrows() * src.ncols()));
  typename T::const_vec_iterator srcvec = src.vec_begin(); 
  for(int i=0; srcvec != src.vec_end(); srcvec++, i++) {
    output[i][0] = (*srcvec).real();
    output[i][1] = (*srcvec).imag();
  }
  return output;
}

template<class T>
inline ComplexImageView* copy_fftw_complex_to_gamera_image(const T& src, fftw_complex* fftw) {
  ComplexImageData* dest_data =
    new ComplexImageData(src.size(), src.offset_y(), src.offset_x());
  ComplexImageView* dest =
    new ComplexImageView(*dest_data, src);
  
  ComplexImageView::vec_iterator destvec = dest->vec_begin();
  
  for(int i=0; destvec != dest->vec_end(); destvec++, i++)
    *destvec = ComplexPixel(fftw[i][0], fftw[i][1]);

  return dest;
}

/* FFTW Support
 *
 * The following functions are callable within Gamera and
 * provide basic FFT support.
 */

template<class T>
ComplexImageView* dft(const T& src) {

      size_t w = src.ncols();
      size_t h = src.nrows();

      fftw_complex *srccomplicated,  *destcomplicated;
      fftw_plan p;

      srccomplicated = copy_gamera_image_to_fftw_complex(src);
      destcomplicated = static_cast<fftw_complex *>(fftw_malloc(sizeof(fftw_complex) * src.nrows() * src.ncols()));


      p = fftw_plan_dft_2d(w, h, srccomplicated, destcomplicated, FFTW_FORWARD, FFTW_ESTIMATE);
     
      fftw_execute(p);

      ComplexImageView* dest = copy_fftw_complex_to_gamera_image(src, destcomplicated);
      
      fftw_destroy_plan(p);

      fftw_free(srccomplicated);
      fftw_free(destcomplicated);

    return dest;
}

template<class T>
ComplexImageView* idft(const T& src) {


  size_t w = src.ncols();
  size_t h = src.nrows();

      typename T::const_vec_iterator srcvec = src.vec_begin(); 

      fftw_complex *srccomplicated,  *destcomplicated;
      fftw_plan p;

      srccomplicated = copy_gamera_image_to_fftw_complex(src);
      destcomplicated = static_cast<fftw_complex *>(fftw_malloc(sizeof(fftw_complex) * src.nrows() * src.ncols()));

      p = fftw_plan_dft_2d(w, h, srccomplicated, destcomplicated, FFTW_BACKWARD, FFTW_ESTIMATE);
     
      fftw_execute(p);

      ComplexImageView* dest = copy_fftw_complex_to_gamera_image(src, destcomplicated);

      fftw_destroy_plan(p);

      fftw_free(srccomplicated);
      fftw_free(destcomplicated);

    return dest;
 
}

template <class T>
typename ImageFactory<T>::view_type* move_dc_to_center(const T &src)
{
  size_t bottom_pad = src.nrows() % 2;
  size_t right_pad = src.ncols() % 2;
  const typename ImageFactory<T>::view_type* new_src;
  if (bottom_pad || right_pad) {
    new_src = pad_image_default(src, 0, right_pad, bottom_pad, 0);
  } else {
    new_src = &src;
  }

  typename ImageFactory<T>::data_type* dest_data =
    new typename ImageFactory<T>::data_type(new_src->size(), new_src->offset_y(), 
					    new_src->offset_x());
  typename ImageFactory<T>::view_type* dest =
    new typename ImageFactory<T>::view_type(*dest_data, *new_src);
  image_copy_fill(*new_src, *dest);
  
  typename ImageFactory<T>::view_type srcquadrant(*new_src, new_src->offset_y(), new_src->offset_x(), new_src->center_y(), new_src->center_x());
  typename ImageFactory<T>::view_type destquadrant(*dest, new_src->center_y(), new_src->center_x(), new_src->center_y(), new_src->center_x());
  image_copy_fill(srcquadrant, destquadrant);

  srcquadrant = ImageFactory<T>::view_type(*new_src, new_src->center_y(), new_src->center_x(), new_src->center_y(), new_src->center_x());
  destquadrant = ImageFactory<T>::view_type(*dest, new_src->offset_y(), new_src->offset_x(), new_src->center_y(), new_src->center_x());
  image_copy_fill(srcquadrant, destquadrant);
 
  srcquadrant = ImageFactory<T>::view_type(*new_src, new_src->center_y(), new_src->offset_x(), new_src->center_y(), new_src->center_x());
  destquadrant = ImageFactory<T>::view_type(*dest, new_src->offset_y(), new_src->center_x(), new_src->center_y(), new_src->center_x());
  image_copy_fill(srcquadrant, destquadrant);

  srcquadrant = ImageFactory<T>::view_type(*new_src, new_src->offset_y(), new_src->center_x(), new_src->center_y(), new_src->center_x());
  destquadrant = ImageFactory<T>::view_type(*dest, new_src->center_y(), new_src->offset_x(), new_src->center_y(), new_src->center_x());
  image_copy_fill(srcquadrant, destquadrant);
  
  return dest;
}

template <class T>
typename ImageFactory<T>::view_type* log_scale_pixels(const T &src)
{
    typename ImageFactory<T>::data_type* dest_data =
      new typename ImageFactory<T>::data_type(src.size(), src.offset_y(), 
					      src.offset_x());
    typename ImageFactory<T>::view_type* dest =
      new typename ImageFactory<T>::view_type(*dest_data, src);

    typename T::const_vec_iterator srcvec = src.vec_begin(); 
    typename T::vec_iterator destvec = dest->vec_begin();

    double max = 0.0;

    for(; srcvec != src.vec_end(); srcvec++) {
      double tmp = (*srcvec).real() * (*srcvec).real();
      if (tmp > max)
	max = tmp;
    }

    double c = 255.0 / (log(1.0+abs(max)));

    srcvec = src.vec_begin();

    for(; srcvec != src.vec_end(); srcvec++, destvec++)
      *destvec = c * log(abs(*srcvec));

    return dest;
}

template <class T>
typename ImageFactory<T>::view_type* log_polar(const T &src)
{
    typename ImageFactory<T>::data_type* dest_data =
      new typename ImageFactory<T>::data_type(src.size(), src.offset_y(), 
					      src.offset_x());
    typename ImageFactory<T>::view_type* dest =
      new typename ImageFactory<T>::view_type(*dest_data, src);

    double dTheta = 2*M_PI / src.ncols();
    double b = pow(10, (double)(log10((double)src.nrows()) / src.nrows()));

    for (size_t i = 0; i < src.nrows(); ++i) {
      for (size_t j = 0; j < src.ncols(); ++j) {

	double r = pow(b, (double)(i)-1);
	double theta = (double)(j) * dTheta;
	double  x_value = r * cos(theta) + (double)(src.ncols()) / 2;
	double  y_value = r * sin(theta) + (double)(src.nrows()) / 2;
	size_t x = (size_t) floor(x_value+0.5);
	size_t y = (size_t) floor(y_value+0.5);
	if ((x > 0) && (y > 0) && (x < src.ncols()) && (y<src.nrows()))
	  dest->set(i,j, src.get(y, x));
      }
    }
    
    return dest;
}

#endif
