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

#ifndef mgd_vectormodule
#define mgd_vectormodule

#include "gameramodule.hpp"
#include "vector.hpp"
#include "floatpoint.hpp"
#include "transformation.hpp"
#include "segment.hpp"
#include "path.hpp"
#include "pathlist.hpp"
// #include "holed_path.hpp"

// NEW STUFF

inline PyObject* get_vectorcore_dict() {
  static PyObject* dict = NULL;
  if (dict == NULL)
    dict = get_module_dict("gamera.toolkits.vector.vectorcore");
  return dict;
}

/*
  FLOATPOINT OBJECT
*/
struct FloatPointObject {
  PyObject_HEAD
  FloatPoint* m_x;
};

#ifndef VECTORCOREINTERNAL
inline PyTypeObject* get_FloatPointType() {
  static PyTypeObject* t = 0;
  if (t == 0) {
    PyObject* dict = get_vectorcore_dict();
    if (dict == 0)
      return 0;
    t = (PyTypeObject*)PyDict_GetItemString(dict, "FloatPoint");
    if (t == 0) {
      PyErr_SetString(PyExc_RuntimeError,
		      "Unable to get FloatPoint type for gamera.toolkits.vector.vectorcore.\n");
      return 0;
    }
  }
  return t;
}
#else
extern PyTypeObject* get_FloatPointType();
#endif

inline FloatPoint coerce_FloatPoint(PyObject* obj) {
  PyTypeObject* t = get_FloatPointType();
  if (t == 0) {
    PyErr_SetString(PyExc_RuntimeError, "Couldn't get FloatPoint type.");
    throw std::runtime_error("Couldn't get FloatPoint type.");
  }
  if (PyObject_TypeCheck(obj, t))
    return FloatPoint(*(((FloatPointObject*)obj)->m_x));
  
  PyTypeObject* t2 = get_PointType();
  if (t2 == 0) {
    PyErr_SetString(PyExc_RuntimeError, "Couldn't get Point type.");
    throw std::runtime_error("Couldn't get Point type.");
  }
  if (PyObject_TypeCheck(obj, t2)) {
    Point* p = (((PointObject*)obj)->m_x);
    return FloatPoint((double)p->x(), (double)p->y());
  }

  PyObject* py_x0 = NULL;
  PyObject* py_y0 = NULL;
  PyObject* py_x1 = NULL;
  PyObject* py_y1 = NULL;
  py_x0 = PyObject_GetAttrString(obj, "x");
  if (py_x0 != NULL) {
    py_x1 = PyNumber_Float(py_x0);
    if (py_x1 != NULL) {
      double x = PyFloat_AsDouble(py_x1);
      py_y0 = PyObject_GetAttrString(obj, "y");
      if (py_y0 != NULL) {
	py_y1 = PyNumber_Float(py_y0);
	if (py_y1 != NULL) {
	  double y = PyFloat_AsDouble(py_y1);
	  return FloatPoint(x, y);
	}
      }
    }
  }
  PyErr_SetString(PyExc_TypeError, "Argument is not a FloatPoint (or convertible to one.)");
  throw std::runtime_error("Object is not a FloatPoint (or can not be converted to a FloatPoint.)");
}

inline PyObject* create_FloatPointObject(const FloatPoint& d) {
  PyTypeObject* t = get_FloatPointType();
  if (t == 0)
    return 0;
  FloatPointObject* so;
  so = (FloatPointObject*)t->tp_alloc(t, 0);
  so->m_x = new FloatPoint(d);
  return (PyObject*)so;
}

/*
  TRANSFORMATION OBJECTS
*/
struct TransformationObject {
  PyObject_HEAD
  Transformation* m_x;
};

#ifndef VECTORCOREINTERNAL
inline PyTypeObject* get_TransformationType() {
  static PyTypeObject* t = 0;
  if (t == 0) {
    PyObject* dict = get_vectorcore_dict();
    if (dict == 0)
      return 0;
    t = (PyTypeObject*)PyDict_GetItemString(dict, "Transformation");
    if (t == 0) {
      PyErr_SetString(PyExc_RuntimeError,
		      "Unable to get Transformation type for gamera.toolkits.vector.vectorcore.\n");
      return 0;
    }
  }
  return t;
}
#else
extern PyTypeObject* get_TransformationType();
#endif

inline bool is_TransformationObject(PyObject* x) {
  PyTypeObject* t = get_TransformationType();
  if (t == 0)
    return 0;
  return PyObject_TypeCheck(x, t);
}

inline PyObject* create_TransformationObject(const Transformation& d) {
  PyTypeObject* t = get_TransformationType();
  if (t == 0)
    return 0;
  TransformationObject* so;
  so = (TransformationObject*)t->tp_alloc(t, 0);
  so->m_x = new Transformation(d);
  return (PyObject*)so;
}

/*
  SEGMENT OBJECTS
*/
struct SegmentObject {
  PyObject_HEAD
  Segment* m_x;
};

#ifndef VECTORCOREINTERNAL
inline PyTypeObject* get_SegmentType() {
  static PyTypeObject* t = 0;
  if (t == 0) {
    PyObject* dict = get_vectorcore_dict();
    if (dict == 0)
      return 0;
    t = (PyTypeObject*)PyDict_GetItemString(dict, "Segment");
    if (t == 0) {
      PyErr_SetString(PyExc_RuntimeError,
		      "Unable to get Segment type for gamera.toolkits.vector.vectorcore.\n");
      return 0;
    }
  }
  return t;
}
#else
extern PyTypeObject* get_SegmentType();
#endif

inline bool is_SegmentObject(PyObject* x) {
  PyTypeObject* t = get_SegmentType();
  if (t == 0)
    return 0;
  return PyObject_TypeCheck(x, t);
}

inline PyObject* create_SegmentObject(const Segment& d) {
  PyTypeObject* t = get_SegmentType();
  if (t == 0)
    return 0;
  SegmentObject* so;
  so = (SegmentObject*)t->tp_alloc(t, 0);
  so->m_x = new Segment(d);
  return (PyObject*)so;
}

/*
  PATH OBJECTS
*/
struct PathObject {
  PyObject_HEAD
  boost::shared_ptr<Path> m_x;
};

#ifndef VECTORCOREINTERNAL
inline PyTypeObject* get_PathType() {
  static PyTypeObject* t = 0;
  if (t == 0) {
    PyObject* dict = get_vectorcore_dict();
    if (dict == 0)
      return 0;
    t = (PyTypeObject*)PyDict_GetItemString(dict, "Path");
    if (t == 0) {
      PyErr_SetString(PyExc_RuntimeError,
		      "Unable to get Path type for gamera.toolkits.vector.vectorcore.\n");
      return 0;
    }
  }
  return t;
}
#else
extern PyTypeObject* get_PathType();
#endif

#ifndef VECTORCOREINTERNAL
inline PyObject* get_PathException() {
  static PyObject* t = 0;
  if (t == 0) {
    PyObject* dict = get_vectorcore_dict();
    if (dict == 0)
      return 0;
    t = (PyObject*)PyDict_GetItemString(dict, "PathException");
    if (t == 0) {
      PyErr_SetString(PyExc_RuntimeError,
		      "Unable to get PathException type for gamera.toolkits.vector.vectorcore.\n");
      return 0;
    }
  }
  return t;
}
#else
extern PyObject* get_PathException();
#endif

inline bool is_PathObject(PyObject* x) {
  PyTypeObject* t = get_PathType();
  if (t == 0)
    return 0;
  return PyObject_TypeCheck(x, t);
}

inline PyObject* create_PathObject(boost::shared_ptr<Path> d) {
  PyTypeObject* t = get_PathType();
  if (t == 0)
    return 0;
  PathObject* so;
  so = (PathObject*)t->tp_alloc(t, 0);
  so->m_x = d;
  return (PyObject*)so;
}

/*
  PATHLIST OBJECTS
*/
struct PathListObject {
  PyObject_HEAD
  boost::shared_ptr<PathList> m_x;
};

#ifndef VECTORCOREINTERNAL
inline PyTypeObject* get_PathListType() {
  static PyTypeObject* t = 0;
  if (t == 0) {
    PyObject* dict = get_vectorcore_dict();
    if (dict == 0)
      return 0;
    t = (PyTypeObject*)PyDict_GetItemString(dict, "PathList");
    if (t == 0) {
      PyErr_SetString
	(PyExc_RuntimeError,
	 "Unable to get PathList type for gamera.toolkits.vector.vectorcore.\n");
      return 0;
    }
  }
  return t;
}
#else
extern PyTypeObject* get_PathListType();
#endif

inline bool is_PathListObject(PyObject* x) {
  PyTypeObject* t = get_PathListType();
  if (t == 0)
    return 0;
  return PyObject_TypeCheck(x, t);
}

inline PyObject* create_PathListObject(boost::shared_ptr<PathList> d) {
  PyTypeObject* t = get_PathListType();
  if (t == 0)
    return 0;
  PathListObject* so;
  so = (PathListObject*)t->tp_alloc(t, 0);
  so->m_x = d;
  return (PyObject*)so;
}

#endif
