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
  static PyObject* pathlist_new(PyTypeObject* pytype, PyObject* args,
			    PyObject* kwds);
  static void pathlist_dealloc(PyObject* self);
  // methods
  static PyObject* pathlist_area(PyObject* self, PyObject* args);
  static PyObject* pathlist_transform(PyObject* self, PyObject* args);
  // sequence interface
  static int pathlist_len(PyObject* self);
  static PyObject* pathlist_getitem(PyObject* self, int index);
  static int pathlist_setitem(PyObject* self, int index, PyObject* obj);
  static PyObject* pathlist_insert(PyObject* self, PyObject* args);
  static PyObject* pathlist_append(PyObject* self, PyObject* args);
}

static PyTypeObject PathListType = {
  PyObject_HEAD_INIT(NULL)
  0,
};

static PySequenceMethods pathlist_sequence_methods;

static PyGetSetDef pathlist_getset[] = {
  { NULL }
};

static PyMethodDef pathlist_methods[] = {
  { "append", pathlist_append, METH_VARARGS,
    "**append** (POINT *p*)\n\nAdds a new line segment to *p* at the end of the pathlist.\n\n   *or*\n\n**append** (POINT *c1*, POINT *c2*, POINT *p*)\n\nAdds a new bezier curve through control points *c1* and *c2* to point *p* to the end of the pathlist.  *or*\n\n**append** (Segment *s*)\n\nAdds segment *s* to the end of the pathlist.  **append** is an alias for **add_segment**."},
  { "insert", pathlist_insert, METH_VARARGS,
    "**insert** (Int *n*, Segment *s*)\n\nInserts segment *s* at position *n*.  If ``patching_mode`` is ``False``, the new segment may not introduce any incontinuities into the pathlist."},
  { "transform", pathlist_transform, METH_VARARGS,
    "**transform** (Transformation *t*, bool in_place = ``False``)\n\nTransforms all Paths in the PathList using the given transformation.  When ``in_place`` is ``False``, the transformed Paths are returned as a new PathList object.  When ``in_place`` is ``True``, the Paths in the PathList object itself are transformed."},
  { "area", pathlist_area, METH_NOARGS,
    "**area** ()\n\nReturns the sum of the areas of the Paths in the PathList."},
  { NULL }
};

PyTypeObject* get_PathListType() {
  return &PathListType;
}

static PyObject* pathlist_new(PyTypeObject* pytype, PyObject* args,
			      PyObject* kwds) {
  PyObject* py_arg = 0;
  if (PyArg_ParseTuple(args, "|O", &py_arg) <= 0)
    return 0;
  boost::shared_ptr<PathList> p(new PathList());
  if (py_arg != 0) {
    PyObject* seq = PySequence_Fast(py_arg, "Argument must be a sequence of Path objects.");
    if (seq == 0)
      return 0;
    int size = PySequence_Fast_GET_SIZE(seq);
    for (int i = 0; i < size; ++i) {
      PyObject* py_path = PySequence_Fast_GET_ITEM(seq, i);
      if (!is_PathObject(py_path)) {
	PyErr_SetString(PyExc_TypeError, "Argument must be a sequence of Path objects.");
	Py_DECREF(seq);
	return 0;
      }
      p->push_back(((PathObject*)py_path)->m_x);
    }
  }
  
  PathListObject* so;
  so = (PathListObject*)pytype->tp_alloc(pytype, 0);
  so->m_x = p;
  return (PyObject*)so;
}

static void pathlist_dealloc(PyObject* self) {
  PathListObject* x = (PathListObject*)self;
  // This is a trick to get the boost::shared_ptr to free,
  // since deallocating PathObject from C (Python's tp_free)
  // will not trigger the destructor (for some reason).
  x->m_x = boost::shared_ptr<PathList>();
  self->ob_type->tp_free(self);
}

static PyObject* pathlist_append(PyObject* self, PyObject* args) {
  boost::shared_ptr<PathList> x = ((PathListObject*)self)->m_x;
  PyObject* py_arg = NULL;
  if (PyArg_ParseTuple(args, "O", &py_arg) <= 0)
    return 0;
  if (!is_PathObject(py_arg))
    return 0;
  x->push_back((((PathObject*)py_arg)->m_x));
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* pathlist_insert(PyObject* self, PyObject* args) {
  boost::shared_ptr<PathList> x = ((PathListObject*)self)->m_x;
  int index;
  PyObject* py_arg = NULL;
  if (PyArg_ParseTuple(args, "iO", &index, &py_arg) <= 0)
      return 0;
  if (!is_PathObject(py_arg))
    return 0;
  x->insert(x->begin() + index, (((PathObject*)py_arg)->m_x));
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* pathlist_area(PyObject* self, PyObject* args) {
  boost::shared_ptr<PathList> x = ((PathListObject*)self)->m_x;
  double result;
  try {
    result = x->area();
  } catch (path_exception e) {
    PyErr_SetString(get_PathException(), e.what());
    return 0;
  }
  return PyFloat_FromDouble(result);
}

static PyObject* pathlist_transform(PyObject* self, PyObject* args) {
  boost::shared_ptr<PathList> x = ((PathListObject*)self)->m_x;
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
    return create_PathListObject(x->transform(*trans));
  }
}

static int pathlist_len(PyObject* self) {
  return ((PathListObject*)self)->m_x->size();
}

static PyObject* pathlist_getitem(PyObject* self, int index) {
  boost::shared_ptr<PathList> x = ((PathListObject*)self)->m_x;
  if (index >= x->size() || index < 0) {
    PyErr_Format(PyExc_IndexError, "'%d' is out of range for pathlist", index);
    return 0;
  }
  return create_PathObject((*x)[index]);
}

static int pathlist_setitem(PyObject* self, int index, PyObject* obj) {
  boost::shared_ptr<PathList> x = ((PathListObject*)self)->m_x;
  if (index >= x->size() || index < 0) {
    PyErr_Format(PyExc_IndexError, "'%d' is out of range for pathlist", index);
    return 0;
  }
  if (!is_PathObject(obj)) {
    PyErr_SetString(PyExc_TypeError, "PathLists can only contain Path objects");
    return 0;
  }
  boost::shared_ptr<Path> path = ((PathObject*)obj)->m_x;
  (*x)[index] = path;
  return 1;
}

static PyObject* pathlist_repr(PyObject* self) {
  // Why do they make this so hard?  PyString_FromFormat does
  // not support doubles
  boost::shared_ptr<PathList> x = ((PathListObject*)self)->m_x;
  std::ostringstream ostr;
  ostr << *x;
  PyObject* result = PyString_FromStringAndSize(ostr.str().data(), ostr.str().size());
  return result;
}

void init_PathListType(PyObject* module_dict) {
  pathlist_sequence_methods.sq_length = pathlist_len;
  pathlist_sequence_methods.sq_item = pathlist_getitem;
  pathlist_sequence_methods.sq_ass_item = pathlist_setitem;

  PathListType.ob_type = &PyType_Type;
  PathListType.tp_name = "vectorcore.PathList";
  PathListType.tp_basicsize = sizeof(PathListObject);
  PathListType.tp_dealloc = pathlist_dealloc;
  PathListType.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
  PathListType.tp_new = pathlist_new;
  PathListType.tp_getattro = PyObject_GenericGetAttr;
  PathListType.tp_alloc = NULL; // PyType_GenericAlloc;
  PathListType.tp_getset = pathlist_getset;
  PathListType.tp_free = NULL; // _PyObject_Del;
  PathListType.tp_methods = pathlist_methods;
  PathListType.tp_repr = pathlist_repr;
  PathListType.tp_doc = "PathList represents a sequence of Paths.\n\nUnlike a mere list of Paths, PathList ensures that all elements are Paths.\n\nPathList objects support the Python list interface.";
  PathListType.tp_as_sequence = &pathlist_sequence_methods;
  PyType_Ready(&PathListType);
  PyDict_SetItemString(module_dict, "PathList", (PyObject*)&PathListType);
}
