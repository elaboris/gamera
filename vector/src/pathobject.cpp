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

#define VECTORCOREINTERNAL
#include "vectormodule.hpp"

extern "C" {
  static PyObject* path_new(PyTypeObject* pytype, PyObject* args,
			    PyObject* kwds);
  static void path_dealloc(PyObject* self);
  // get/set
  static PyObject* path_get_patching_mode(PyObject* self);
  static PyObject* path_set_patching_mode(PyObject* self, PyObject* obj);
  static PyObject* path_get_holes(PyObject* self);
  // methods
  static PyObject* path_is_closed(PyObject* self, PyObject* args);
  static PyObject* path_add_segment(PyObject* self, PyObject* args);
  static PyObject* path_area(PyObject* self, PyObject* args);
  static PyObject* path_repr(PyObject* self);
  static PyObject* path_transform(PyObject*, PyObject* args);
  static PyObject* path_convert_to_lines(PyObject*, PyObject* args);
  // sequence interface
  static int path_len(PyObject* self);
  static PyObject* path_getitem(PyObject* self, int index);
  static int path_setitem(PyObject* self, int index, PyObject* obj);
  static PyObject* path_concat(PyObject* self, PyObject* other);
  static PyObject* path_inplace_concat(PyObject* self, PyObject* other);
  static PyObject* path_insert(PyObject* self, PyObject* args);
}

static PyObject* PyExc_PathException = NULL;

static PyTypeObject PathType = {
  PyObject_HEAD_INIT(NULL)
  0,
};

static PySequenceMethods path_sequence_methods;

static PyGetSetDef path_getset[] = {
  { "patching_mode", (getter)path_get_patching_mode, (setter)path_set_patching_mode, 
    "(bool property)\n\nGets/sets the patching mode of the path.  When ``patching_mode`` == ``True``, any insertions of segments may cause extra line segments to be inserted in order to keep the Path continuous.  When ``patching_mode`` == ``False``, inserting segments that create discontinuities will raise a ``PathException.``", 0},
  { "holes", (getter)path_get_holes, NULL, 
    "(PathList property)\n\nReturns the holes of this Path", 0},
  { NULL }
};

static PyMethodDef path_methods[] = {
  { "is_closed", path_is_closed, METH_NOARGS,
    "**is_closed** ()\n\nReturns ``True`` if the path is a closed (the start of the first segment and the end of the last segment are the same point)."},
  { "add_segment", path_add_segment, METH_VARARGS,
    "**add_segment** (POINT *p*)\n\nAdds a new line segment to *p* at the end of the path.\n\n   *or*\n\n**add_segment** (POINT *c1*, POINT *c2*, POINT *p*)\n\nAdds a new bezier curve through control points *c1* and *c2* to point *p* to the end of the path.\n\n  *or*\n\n**add_segment** (Segment *s*)\n\nAdds segment *s* to the end of the path.\n\n**add_segment** is an alias for **append**."},
  { "append", path_add_segment, METH_VARARGS,
    "**append** (POINT *p*)\n\nAdds a new line segment to *p* at the end of the path.\n\n   *or*\n\n**append** (POINT *c1*, POINT *c2*, POINT *p*)\n\nAdds a new bezier curve through control points *c1* and *c2* to point *p* to the end of the path.  *or*\n\n**append** (Segment *s*)\n\nAdds segment *s* to the end of the path.  **append** is an alias for **add_segment**."},
  { "insert", path_add_segment, METH_VARARGS,
    "**insert** (Int *n*, Segment *s*)\n\nInserts segment *s* at position *n*.  If ``patching_mode`` is ``False``, the new segment must not introduce any incontinuities into the path."},
  { "area", path_area, METH_NOARGS,
    "**area** ()\n\nReturns the area of the path (if closed), otherwise raises a ``PathException``."},
  { "transform", path_transform, METH_VARARGS,
    "**transform** (Transformation *t*, bool in_place = ``False``)\n\nTransforms the path using the given transformation.  When ``in_place`` is ``False``, the transformed path is returned as a new Path object.  When ``in_place`` is ``True``, the Path object itself is transformed."},
  { "convert_to_lines", path_convert_to_lines, METH_VARARGS,
    "**convert_to_lines** (Float *accuracy* = 0.1)\n\nReturns a new path which contains only line segments (no bezier curves) by approximating all of the bezier curves to line segments with the given accuracy."},
  { NULL }
};

PyTypeObject* get_PathType() {
  return &PathType;
}

PyObject* get_PathException() {
  return PyExc_PathException;
}

static PyObject* path_new(PyTypeObject* pytype, PyObject* args,
			  PyObject* kwds) {
  Path* new_path;
  int num_args = PyTuple_GET_SIZE(args);
  if (num_args != 0) {
    PyErr_SetString(PyExc_TypeError, "(At the moment), the Path constructor takes no arguments.");
    return 0;
  }
  PathObject* so;
  so = (PathObject*)pytype->tp_alloc(pytype, 0);
  boost::shared_ptr<Path> p(new Path());
  so->m_x = p;
  return (PyObject*)so;
}

static void path_dealloc(PyObject* self) {
  PathObject* x = (PathObject*)self;
  // This is a trick to get the boost::shared_ptr to free,
  // since deallocating PathObject from C (Python's tp_free)
  // will not trigger the destructor (for some reason).
  x->m_x = boost::shared_ptr<Path>();
  self->ob_type->tp_free(self);
}

#define CREATE_BOOL_GET_FUNC(name) static PyObject* path_get_##name(PyObject* self) {\
  Path* x = ((PathObject*)self)->m_x.get(); \
  if (x->name()) { \
    Py_INCREF(Py_True); \
    return Py_True; \
  } else { \
    Py_INCREF(Py_False); \
    return Py_False; \
  } \
} \

#define CREATE_BOOL_SET_FUNC(name) static PyObject* path_set_##name(PyObject* self, PyObject* value) {\
  Path* x = ((PathObject*)self)->m_x.get(); \
  if (PyObject_IsTrue(value)) \
    x->patching_mode(true); \
  else \
    x->patching_mode(false); \
  Py_INCREF(Py_None); \
  return Py_None; \
} \

CREATE_BOOL_GET_FUNC(patching_mode)
CREATE_BOOL_SET_FUNC(patching_mode)

static PyObject* path_get_holes(PyObject* self) {
  boost::shared_ptr<Path> x = ((PathObject*)self)->m_x;
  boost::shared_ptr<PathList> p = x->holes();
  return create_PathListObject(p);
}

static PyObject* path_is_closed(PyObject* self, PyObject* args) {
  Path* x = ((PathObject*)self)->m_x.get();
  if (x->is_closed()) {
    Py_INCREF(Py_True);
    return Py_True;
  } else {
    Py_INCREF(Py_False);
    return Py_False;
  }
}

static PyObject* path_add_segment(PyObject* self, PyObject* args) {
  Path* x = ((PathObject*)self)->m_x.get();
  int num_args = PyTuple_GET_SIZE(args);
  if (num_args == 1) {
    PyObject* py_arg = NULL;
    if (PyArg_ParseTuple(args, "O", &py_arg) <= 0)
      return 0;
    if (is_SegmentObject(py_arg))
      x->add_segment(*(((SegmentObject*)py_arg)->m_x));
    else {
      try {
	FloatPoint fp = coerce_FloatPoint(py_arg);
	x->add_segment(fp);
      } catch (std::exception) {
	return 0;
      }
    }
  } else if (num_args == 3) {
    PyObject* py_c1, *py_c2, *py_end;
    if (PyArg_ParseTuple(args, "OOO", &py_c1, &py_c2, &py_end) <= 0)
      return 0;
    try {
      FloatPoint c1 = coerce_FloatPoint(py_c1);
      FloatPoint c2 = coerce_FloatPoint(py_c2);
      FloatPoint end = coerce_FloatPoint(py_end);
      x->add_segment(c1, c2, end);
    } catch (std::exception) {
      return 0;
    }
  } else {
    PyErr_SetString(PyExc_TypeError, "Invalid arguments to add_segment or append.");
    return 0;
  }
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* path_insert(PyObject* self, PyObject* args) {
  Path* x = ((PathObject*)self)->m_x.get();
  int index;
  PyObject* py_arg = NULL;
  if (PyArg_ParseTuple(args, "iO", &index, &py_arg) <= 0)
      return 0;
  if (!is_SegmentObject(py_arg))
    return 0;
  x->insert(x->begin() + index, *(((SegmentObject*)py_arg)->m_x));
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* path_area(PyObject* self, PyObject* args) {
  Path* x = ((PathObject*)self)->m_x.get();
  double result;
  try {
    result = x->area();
  } catch (path_exception e) {
    PyErr_SetString(PyExc_PathException, e.what());
    return 0;
  }
  return PyFloat_FromDouble(result);
}

static PyObject* path_transform(PyObject* self, PyObject* args) {
  Path* x = ((PathObject*)self)->m_x.get();
  PyObject* py_trans;
  int in_place = 0;
  if (PyArg_ParseTuple(args, "O|i", &py_trans, &in_place) <= 0)
    return 0;
  if (!is_TransformationObject(py_trans)) {
    PyErr_SetString(PyExc_TypeError, "transform requires a transformation object");
    return 0;
  }
  Transformation* trans = ((TransformationObject*)py_trans)->m_x;
  if (in_place) {
    x->transform_in_place(*trans);
    Py_INCREF(Py_None);
    return Py_None;
  } else {
    return create_PathObject(x->transform(*trans));
  }
}

static PyObject* path_convert_to_lines(PyObject* self, PyObject* args) {
  Path* x = ((PathObject*)self)->m_x.get();
  double accuracy = 0.1;
  if (PyArg_ParseTuple(args, "|d", &accuracy) <= 0)
    return 0;
  return create_PathObject(x->convert_to_lines(accuracy));
}

static int path_len(PyObject* self) {
  return ((PathObject*)self)->m_x->size();
}

static PyObject* path_getitem(PyObject* self, int index) {
  Path* x = ((PathObject*)self)->m_x.get();
  if (index >= x->size() || index < 0) {
    PyErr_Format(PyExc_IndexError, "'%d' is out of range for path", index);
    return 0;
  }
  return create_SegmentObject((*x)[index]);
}

static int path_setitem(PyObject* self, int index, PyObject* obj) {
  Path* x = ((PathObject*)self)->m_x.get();
  if (index >= x->size() || index < 0) {
    PyErr_Format(PyExc_IndexError, "'%d' is out of range for path", index);
    return 0;
  }
  if (!is_SegmentObject(obj)) {
    PyErr_SetString(PyExc_TypeError, "Paths can only contain Segment objects");
    return 0;
  }
  Segment* segment = ((SegmentObject*)obj)->m_x;
  try {
    x->setitem(index, *segment);
  } catch (path_exception e) {
    PyErr_SetString(PyExc_PathException, e.what());
    return 0;
  }
  return 1;
}

static PyObject* path_concat(PyObject* py_self, PyObject* py_other) {
  if (!is_PathObject(py_other)) {
    PyErr_SetString(PyExc_TypeError, "Paths can only be concatenated with other path objects");
    return 0;
  }
  boost::shared_ptr<Path> self = ((PathObject*)py_self)->m_x;
  boost::shared_ptr<Path> other = ((PathObject*)py_other)->m_x;
  boost::shared_ptr<Path> new_path(new Path());
  new_path->reserve(self->size() + other->size());
  try {
    new_path->insert(new_path->begin(), self->begin(), self->end());
    new_path->insert(new_path->end(), other->begin(), other->end());
  } catch (path_exception e) {
    // delete new_path; Should be implicit by boost::shared_ptr
    PyErr_SetString(PyExc_PathException, e.what());
    return 0;
  }
  return create_PathObject(new_path);
}

static PyObject* path_inplace_concat(PyObject* py_self, PyObject* py_other) {
  if (!is_PathObject(py_other)) {
    PyErr_SetString(PyExc_TypeError, "Paths can only be concatenated with other path objects");
    return 0;
  }
  boost::shared_ptr<Path> self = ((PathObject*)py_self)->m_x;
  boost::shared_ptr<Path> other = ((PathObject*)py_other)->m_x;
  self->reserve(self->size() + other->size());
  try {
    self->insert(self->end(), other->begin(), other->end());
  } catch (path_exception e) {
    PyErr_SetString(PyExc_PathException, e.what());
    return 0;
  }
  return py_self;
}

static PyObject* path_repr(PyObject* self) {
  // Why do they make this so hard?  PyString_FromFormat does
  // not support doubles
  Path* x = ((PathObject*)self)->m_x.get();
  std::ostringstream ostr;
  ostr << *x;
  PyObject* result = PyString_FromStringAndSize(ostr.str().data(), ostr.str().size());
  return result;
}

void init_PathType(PyObject* module_dict) {
  path_sequence_methods.sq_length = path_len;
  path_sequence_methods.sq_item = path_getitem;
  path_sequence_methods.sq_ass_item = path_setitem;
  path_sequence_methods.sq_concat = path_concat;
  path_sequence_methods.sq_inplace_concat = path_inplace_concat;

  PathType.ob_type = &PyType_Type;
  PathType.tp_name = "vectorcore.Path";
  PathType.tp_basicsize = sizeof(PathObject);
  PathType.tp_dealloc = path_dealloc;
  PathType.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
  PathType.tp_new = path_new;
  PathType.tp_getattro = PyObject_GenericGetAttr;
  PathType.tp_alloc = NULL; // PyType_GenericAlloc;
  PathType.tp_getset = path_getset;
  PathType.tp_free = NULL; // _PyObject_Del;
  PathType.tp_methods = path_methods;
  PathType.tp_repr = path_repr;
  PathType.tp_doc = "Path represents a sequence of Segments (line or bezier curve segments).  It is designed to be a general representation of any 2D shape.\n\nUnlike a simple list of Segments, Path objects ensure that for every mutating operation the path is maintained to be continuous.  The patching_mode_ property can be set to change how the continuity is enforced.\n\nEach path also has a list of holes, each hole being a Path object that is cut out of the path.";
  PathType.tp_as_sequence = &path_sequence_methods;
  PyType_Ready(&PathType);
  PyDict_SetItemString(module_dict, "Path", (PyObject*)&PathType);
  
  PyExc_PathException = PyErr_NewException("gamera.toolkits.vector.vectorcore.PathException", NULL, NULL);
}
