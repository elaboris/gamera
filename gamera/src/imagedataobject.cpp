/*
 *
 * Copyright (C) 2001-2005 Ichiro Fujinaga, Michael Droettboom, and Karl MacMillan
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

#define GAMERACORE_INTERNAL
#include "gameramodule.hpp"

using namespace Gamera;

extern "C" {
  static PyObject* imagedata_new(PyTypeObject* pytype, PyObject* args,
				 PyObject* kwds);
  static void imagedata_dealloc(PyObject* self);
  // get/set
  static PyObject* imagedata_get_stride(PyObject* self);
  static PyObject* imagedata_get_ncols(PyObject* self);
  static PyObject* imagedata_get_nrows(PyObject* self);
  static PyObject* imagedata_get_page_offset_x(PyObject* self);
  static PyObject* imagedata_get_page_offset_y(PyObject* self);
  static PyObject* imagedata_get_size(PyObject* self);
  static PyObject* imagedata_get_bytes(PyObject* self);
  static PyObject* imagedata_get_mbytes(PyObject* self);
  static PyObject* imagedata_get_pixel_type(PyObject* self);
  static PyObject* imagedata_get_storage_format(PyObject* self);
  static int imagedata_set_page_offset_x(PyObject* self, PyObject* v);
  static int imagedata_set_page_offset_y(PyObject* self, PyObject* v);
  static int imagedata_set_nrows(PyObject* self, PyObject* v);
  static int imagedata_set_ncols(PyObject* self, PyObject* v);
  // methods
  static PyObject* imagedata_dimensions(PyObject* self, PyObject* args);
}

static PyTypeObject ImageDataType = {
  PyObject_HEAD_INIT(NULL)
  0,
};

static PyGetSetDef imagedata_getset[] = {
  { "nrows", (getter)imagedata_get_nrows, (setter)imagedata_set_nrows,
    "(int property get/set)\n\nThe number of rows", 0 },
  { "ncols", (getter)imagedata_get_ncols, (setter)imagedata_set_ncols,
    "(int property get/set)\n\nThe number of columns", 0 },
  { "page_offset_x", (getter)imagedata_get_page_offset_x,
    (setter)imagedata_set_page_offset_x,
    "(int property get/set)\n\nThe *x* offset in the page for the data", 0 },
  { "page_offset_y", (getter)imagedata_get_page_offset_y,
    (setter)imagedata_set_page_offset_y,
    "(int property get/set)\n\nThe *y* offset in the page for the data", 0 },
  { "stride", (getter)imagedata_get_stride, 0,
    "(int property get/set)\n\nThe length of the data stride", 0 },
  { "size", (getter)imagedata_get_size, 0,
    "(Size property get/set)\n\nThe size of the image data", 0 },
  { "bytes", (getter)imagedata_get_bytes, 0,
    "(int property get/set)\n\nThe size of the data in bytes", 0 },
  { "mbytes", (getter)imagedata_get_mbytes, 0,
    "(int property get/set)\n\nThe size of the data in megabytes", 0 },
  { "pixel_type", (getter)imagedata_get_pixel_type, 0,
    "(int property get/set)\n\nThe type of the pixels.  See `pixel types`__ for more info.\n\n.. __: image_types.html#pixel-types", 0 },
  { "storage_format", (getter)imagedata_get_storage_format, 0,
    "(int property get/set)\n\nThe format of the storage.  See `storage formats`__ for more info.\n\n.. __: image_types.html#storage-formats", 0 },
  { NULL }
};

static PyMethodDef imagedata_methods[] = {
  { "dimensions", imagedata_dimensions, METH_VARARGS },
  { NULL }
};

PyTypeObject* get_ImageDataType() {
  return &ImageDataType;
}

static PyObject* imagedata_new(PyTypeObject* pytype, PyObject* args,
			       PyObject* kwds) {
  int format, pixel;
  int num_args = PyTuple_GET_SIZE(args);

  if (num_args == 4) {
    PyObject* py_point = NULL;
    PyObject* py_dim = NULL;
    if (PyArg_ParseTuple(args, "OOii", &py_dim, &py_point, &pixel, &format)) {
      if (is_DimObject(py_dim)) {
	try {
	  return create_ImageDataObject(*(((DimObject*)py_dim)->m_x), coerce_Point(py_point), pixel, format);
	} catch (std::invalid_argument e) {
	  ;
	}
      }
    }
  }

  PyErr_Clear();
  
  if (num_args == 1) {
    PyObject* py_rect;
    if (PyArg_ParseTuple(args, "O", &py_rect)) {
      if (is_RectObject(py_rect)) {
	Rect* rect = ((RectObject*)py_rect)->m_x;
	return create_ImageDataObject(rect->dim(), rect->origin(), pixel, format);
      }
    }
  }

#ifdef GAMERA_DEPRECATED
  PyErr_Clear();

  if (num_args == 6) {
    int nrows, ncols, page_offset_y, page_offset_x, format, pixel;
    if (PyArg_ParseTuple(args, "iiiiii", &nrows, &ncols, &page_offset_y,
			 &page_offset_x, &pixel, &format)) {
      if (send_deprecation_warning(
"ImageData(nrows, ncols, page_offset_y, page_offset_x, pixel_type, \n"
"storage_format) is deprecated.\n\n"
"Reason: (x, y) coordinate consistency.\n\n"
"Use ImageData(Dim(ncols, nrows), (page_offset_x, page_offset_y),\n"
"pixel_type, storage_format) instead.", 
"imagedataobject.cpp", __LINE__) == 0)
	return 0;
      return create_ImageDataObject(Dim(ncols, nrows), Point(page_offset_x, page_offset_y), pixel, format);
    }
  }
#endif      
  
  PyErr_Clear();
  PyErr_SetString(PyExc_TypeError, "Invalid arguments to ImageData constructor.  Valid forms are: (Dim dim, Point p, pixel_type = 0, storage_format = 0), and (Rect rect, pixel_type = 0, storage_format = 0). ");
  return 0;
}
 
static void imagedata_dealloc(PyObject* self) {
  ImageDataObject* x = (ImageDataObject*)self;
  delete x->m_x;
  self->ob_type->tp_free(self);
}

#define CREATE_GET_FUNC(name) static PyObject* imagedata_get_##name(PyObject* self) {\
  ImageDataBase* x = ((ImageDataObject*)self)->m_x; \
  return PyInt_FromLong((int)x->name()); \
}

#define CREATE_SET_FUNC(name) static int imagedata_set_##name(PyObject* self, PyObject* value) {\
  ImageDataBase* x = ((ImageDataObject*)self)->m_x; \
  x->name((size_t)PyInt_AS_LONG(value)); \
  return 0; \
}

CREATE_GET_FUNC(stride)
CREATE_GET_FUNC(ncols)
CREATE_GET_FUNC(nrows)
CREATE_GET_FUNC(page_offset_x)
CREATE_GET_FUNC(page_offset_y)
CREATE_GET_FUNC(size)
CREATE_GET_FUNC(bytes)

CREATE_SET_FUNC(page_offset_x)
CREATE_SET_FUNC(page_offset_y)
CREATE_SET_FUNC(nrows)
CREATE_SET_FUNC(ncols)

static PyObject* imagedata_get_mbytes(PyObject* self) {
  ImageDataBase* x = ((ImageDataObject*)self)->m_x;
  return Py_BuildValue("d", x->mbytes());
}

static PyObject* imagedata_get_pixel_type(PyObject* self) {
  return Py_BuildValue("i", ((ImageDataObject*)self)->m_pixel_type);
}

static PyObject* imagedata_get_storage_format(PyObject* self) {
  return Py_BuildValue("i", ((ImageDataObject*)self)->m_storage_format);
}

static PyObject* imagedata_dimensions(PyObject* self, PyObject* args) {
  ImageDataBase* x = ((ImageDataObject*)self)->m_x;
  int num_args = PyTuple_GET_SIZE(args);
  if (num_args == 1) {
    PyObject* py_dim;
    if (PyArg_ParseTuple(args, "O", &py_dim)) {
      if (is_DimObject(py_dim)) {
	x->dim(*(((DimObject*)py_dim)->m_x));
	Py_INCREF(Py_None);
	return Py_None;
      }
    }
  }
#ifdef GAMERA_DEPRECATED
  PyErr_Clear();
  if (num_args == 2) {
    int nrows, ncols;
    if (PyArg_ParseTuple(args, "ii", &nrows, &ncols)) {
      if (send_deprecation_warning(
"ImageData.dimensions(nrows, ncols) is deprecated.\n\n"
"Reason: (x, y) coordinate consistency.\n\n"
"Use ImageData.dimensions(Dim(ncols, nrows)) instead.",
"imagedataobject.cpp", __LINE__) == 0)
	return 0;
      x->dimensions((size_t)nrows, (size_t)ncols); // deprecated call
      Py_INCREF(Py_None);
      return Py_None;
    }
  }
#endif
  PyErr_Clear();
  PyErr_SetString(PyExc_TypeError, "Invalid arguments to ImageData.dimensions.  Must be one Dim argument.");
  return 0;
}

void init_ImageDataType(PyObject* module_dict) {
  ImageDataType.ob_type = &PyType_Type;
  ImageDataType.tp_name = "gameracore.ImageData";
  ImageDataType.tp_basicsize = sizeof(ImageDataObject);
  ImageDataType.tp_dealloc = imagedata_dealloc;
  ImageDataType.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
  ImageDataType.tp_getset = imagedata_getset;
  ImageDataType.tp_methods = imagedata_methods;
  ImageDataType.tp_new = imagedata_new;
  ImageDataType.tp_getattro = PyObject_GenericGetAttr;
  ImageDataType.tp_alloc = NULL; // PyType_GenericAlloc;
  ImageDataType.tp_free = NULL; // _PyObject_Del;
  ImageDataType.tp_doc = 
"There are many ways to initialize ImageData:\n\n"
"  - ImageData(Dim *dim*, Point *offset*, Int *pixel_type*, Int *storage_format*)\n\n"
"  - ImageData(Rect *rect*, Int *pixel_type*, Int *storage_format*)\n\n"
"**Deprecated forms:**\n\n"
"  - ImageData(Int *nrows*, Int *ncols*, Int *page_offset_y*, Int *page_offset_x*, "
"Int *pixel_type*, Int *storage_format*)\n\n"
"*pixel_type*\n"
"  An integer value specifying the type of the pixels in the image.\n"
"  See `pixel types`__ for more information.\n\n"
".. __: image_types.html#pixel-types\n\n"
"*storage_format*\n"
"  An integer value specifying the method used to store the image data.\n"
"  See `storage formats`__ for more information.\n\n"
".. __: image_types.html#storage-formats\n";
  PyType_Ready(&ImageDataType);
  PyDict_SetItemString(module_dict, "ImageData", (PyObject*)&ImageDataType);
  // Some constants
  PyDict_SetItemString(module_dict, "FLOAT",
		       Py_BuildValue("i", Gamera::FLOAT));
  PyDict_SetItemString(module_dict, "COMPLEX",
		       Py_BuildValue("i", Gamera::COMPLEX));
  PyDict_SetItemString(module_dict, "ONEBIT",
		       Py_BuildValue("i", ONEBIT));
  PyDict_SetItemString(module_dict, "GREYSCALE",
		       Py_BuildValue("i", GREYSCALE));
  PyDict_SetItemString(module_dict, "GREY16",
		       Py_BuildValue("i", GREY16));
  PyDict_SetItemString(module_dict, "RGB",
		       Py_BuildValue("i", RGB));
  PyDict_SetItemString(module_dict, "DENSE",
		       Py_BuildValue("i", DENSE));
  PyDict_SetItemString(module_dict, "RLE",
		       Py_BuildValue("i", RLE));
}


