
#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <DS4Structs.h>
#include <VM.h>

typedef struct {
  PyObject_HEAD

  USBReport01_t report;
  int delta;
  float IMUX, IMUY, IMUZ;
} ReportObject;

class PyVM : public VM
{
public:
  PyVM(uint8_t* bytecode, uint8_t stacksize)
    : VM(bytecode, false, stacksize),
      m_StackSize(stacksize)
  {
    memset(m_Stack, 0, stacksize);
  }

  bool Step(ReportObject* report) {
    m_DELTA = report->delta;
    m_IMUX = report->IMUX;
    m_IMUY = report->IMUY;
    m_IMUZ = report->IMUZ;

    return VM::Step((controller_state_t*)((uint8_t*)&report->report + 1));
  }

  PyObject* GetStack() {
    return PyBytes_FromStringAndSize((char*)m_Stack, m_SP);
  }

  void Push(const uint8_t* values, int size) {
    for (int i = 0; i < size; ++i)
      m_Stack[m_SP++] = values[i];
  }

  int GetOffset() const {
    return m_Offset;
  }

  int GetSP() const {
    return m_SP;
  }

  void SetSP(int sp) {
    m_SP = sp;
  }

  int GetTH() const {
    return m_TH;
  }

  void SetTH(int th) {
    m_TH = th;
  }

  int m_StackSize;
};

typedef struct {
  PyObject_HEAD

  PyVM *pVM;
  PyObject* pBytecode; // Bytes object
} VMObject;

//==============================================================================
// Report methods

static void Report_dealloc(ReportObject* self)
{
  Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* Report_new(PyTypeObject* type, PyObject* args, PyObject* kwargs)
{
  ReportObject* self = (ReportObject*)type->tp_alloc(type, 0);
  if (self) {
    memset(&self->report, 0, sizeof(self->report));
    self->delta = 5000;
    self->IMUX = self->IMUY = self->IMUZ = 0.0f;
  }

  return (PyObject*)self;
}

static int Report_init(ReportObject* self, PyObject* args, PyObject* kwargs)
{
  static char *kwlist[] = {
    "LPadX", "LPadY", "RPadX", "RPadY", "Hat",
    "Square", "Cross", "Circle", "Triangle", "L1", "R1", "L2", "R2", "Share", "Options", "L3", "R3", "PS", "TPad",
    "L2Value", "R2Value", "DELTA", "IMUX", "IMUY", "IMUZ",
    NULL
  };

  int lpadx = 127, lpady = 127, rpadx = 127, rpady = 127, hat = 8;
  int Square = 0, cross = 0, circle = 0, triangle = 0, l1 = 0, r1 = 0, l2 = 0, r2 = 0, share = 0, options = 0, l3 = 0, r3 = 0, ps = 0, tpad = 0;
  int L2Value = 0, r2val = 0;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|iiiiippppppppppppppiiifff:__init__", kwlist, &lpadx, &lpady, &rpadx, &rpady, &hat,
                                   &Square, &cross, &circle, &triangle, &l1, &r1, &l2, &r2, &share, &options, &l3, &r3, &ps, &tpad,
                                   &L2Value, &r2val, &self->delta, &self->IMUX, &self->IMUY, &self->IMUZ))
    return -1;

  self->report.LPadX = lpadx;
  self->report.LPadY = lpady;
  self->report.RPadX = rpadx;
  self->report.RPadY = rpady;
  self->report.Hat = hat;
  self->report.Square = Square;
  self->report.Cross = cross;
  self->report.Circle = circle;
  self->report.Triangle = triangle;
  self->report.L1 = l1;
  self->report.R1 = r1;
  self->report.L2 = l2;
  self->report.R2 = r2;
  self->report.Share = share;
  self->report.Options = options;
  self->report.L3 = l3;
  self->report.R3 = r3;
  self->report.PS = ps;
  self->report.TPad = tpad;
  self->report.L2Value = L2Value;
  self->report.R2Value = r2val;

  return 0;
}

static PyMethodDef Report_methods[] = {
  { NULL }
};

#define IDCAT(a, b) a##b

#define INPUT8_GETSET(name, minval, maxval)                             \
  static PyObject* IDCAT(Report_get_, name)(ReportObject* self, void*) { \
    return Py_BuildValue("i", (int)self->report . name);                 \
  }                                                                      \
  static int IDCAT(Report_set_, name)(ReportObject* self, PyObject* value, void*) { \
    if (!PyLong_Check(value)) {                                          \
      PyErr_SetString(PyExc_TypeError, #name " value must be an int");   \
      return -1;                                                         \
    }                                                                    \
    long v = PyLong_AsLong(value);                                       \
    if ((v < minval) || (v > maxval)) {                                 \
      PyErr_SetString(PyExc_ValueError, #name " must be in the " #minval ".." #maxval " range"); \
      return -1;                                                         \
    }                                                                    \
    self->report . name = CLAMPU8(PyLong_AsLong(value));                  \
    return 0;                                                            \
  }

#define BUTTON_GETSET(name) \
  static PyObject* IDCAT(Report_get_, name)(ReportObject* self, void*) { \
    PyObject* ret = self->report . name ? Py_True : Py_False;            \
    Py_INCREF(ret);                                                      \
    return ret;                                                          \
  }                                                                      \
  static int IDCAT(Report_set_, name)(ReportObject* self, PyObject* value, void*) { \
    if (!PyBool_Check(value)) { \
      PyErr_SetString(PyExc_TypeError, #name " must be a bool");        \
      return -1;                                                        \
    }                                                                   \
    self->report . name = (value == Py_True);                           \
    return 0;                                                           \
  }


INPUT8_GETSET(LPadX, 0, 255);
INPUT8_GETSET(LPadY, 0, 255);
INPUT8_GETSET(RPadX, 0, 255);
INPUT8_GETSET(RPadY, 0, 255);
INPUT8_GETSET(Hat, 0, 8);
INPUT8_GETSET(L2Value, 0, 255);
INPUT8_GETSET(R2Value, 0, 255);

BUTTON_GETSET(Square);
BUTTON_GETSET(Cross);
BUTTON_GETSET(Circle);
BUTTON_GETSET(Triangle);
BUTTON_GETSET(L1);
BUTTON_GETSET(R1);
BUTTON_GETSET(L2);
BUTTON_GETSET(R2);
BUTTON_GETSET(Share);
BUTTON_GETSET(Options);
BUTTON_GETSET(L3);
BUTTON_GETSET(R3);
BUTTON_GETSET(PS);
BUTTON_GETSET(TPad);

static PyObject* Report_get_DELTA(ReportObject* self, void*)
{
  return Py_BuildValue("i", self->delta);
}

static int Report_set_DELTA(ReportObject* self, PyObject* value, void*)
{
  if (!PyLong_Check(value)) {
    PyErr_SetString(PyExc_TypeError, "DELTA must be an integer");
    return -1;
  }

  long v = PyLong_AsLong(value);
  if ((v <= 0) || (v > 32767)) {
    PyErr_SetString(PyExc_ValueError, "DELTA must be in the 0..32767 range");
    return -1;
  }

  self->delta = v;
  return 0;
}

#define INPUTF_GETSET(name)                                             \
  static PyObject* IDCAT(Report_get_, name)(ReportObject* self, void*) { \
    return Py_BuildValue("f", self -> name);                             \
  }                                                                      \
  static int IDCAT(Report_set_, name)(ReportObject* self, PyObject* value, void*) { \
    if (!PyFloat_Check(value)) {                                          \
    PyErr_SetString(PyExc_TypeError, #name " must be a float");         \
      return -1;                                                        \
    }                                                                   \
    self -> name = PyFloat_AsDouble(value);                             \
    return 0;                                                           \
  }

INPUTF_GETSET(IMUX);
INPUTF_GETSET(IMUY);
INPUTF_GETSET(IMUZ);

static PyGetSetDef Report_getsetters[] = {
  { "RPadX", (getter)Report_get_RPadX, (setter)Report_set_RPadX, "Right pad X value", NULL },
  { "RPadY", (getter)Report_get_RPadY, (setter)Report_set_RPadY, "Right pad Y value", NULL },
  { "LPadX", (getter)Report_get_LPadX, (setter)Report_set_LPadX, "Left pad X value", NULL },
  { "LPadY", (getter)Report_get_LPadY, (setter)Report_set_LPadY, "Left pad Y value", NULL },
  { "Hat", (getter)Report_get_Hat, (setter)Report_set_Hat, "DPad value", NULL },
  { "L2Value", (getter)Report_get_L2Value, (setter)Report_set_L2Value, "L2 button value", NULL },
  { "R2Value", (getter)Report_get_R2Value, (setter)Report_set_R2Value, "R2 button value", NULL },

  { "Square", (getter)Report_get_Square, (setter)Report_set_Square, "Square button state", NULL },
  { "Cross", (getter)Report_get_Cross, (setter)Report_set_Cross, "Cross button state", NULL },
  { "Circle", (getter)Report_get_Circle, (setter)Report_set_Circle, "Circle button state", NULL },
  { "Triangle", (getter)Report_get_Triangle, (setter)Report_set_Triangle, "Triangle button state", NULL },
  { "L1", (getter)Report_get_L1, (setter)Report_set_L1, "L1 button state", NULL },
  { "R1", (getter)Report_get_R1, (setter)Report_set_R1, "R1 button state", NULL },
  { "L2", (getter)Report_get_L2, (setter)Report_set_L2, "L2 button state", NULL },
  { "R2", (getter)Report_get_R2, (setter)Report_set_R2, "R2 button state", NULL },
  { "Share", (getter)Report_get_Share, (setter)Report_set_Share, "Share button state", NULL },
  { "Options", (getter)Report_get_Options, (setter)Report_set_Options, "Options button state", NULL },
  { "L3", (getter)Report_get_L3, (setter)Report_set_L3, "L3 button state", NULL },
  { "R3", (getter)Report_get_R3, (setter)Report_set_R3, "R3 button state", NULL },
  { "PS", (getter)Report_get_PS, (setter)Report_set_PS, "PS button state", NULL },
  { "TPad", (getter)Report_get_TPad, (setter)Report_set_TPad, "TPad button state", NULL },

  { "DELTA", (getter)Report_get_DELTA, (setter)Report_set_DELTA, "Microseconds since the last report", NULL },
  { "IMUX", (getter)Report_get_IMUX, (setter)Report_set_IMUX, "IMU X angle", NULL },
  { "IMUY", (getter)Report_get_IMUY, (setter)Report_set_IMUY, "IMU Y angle", NULL },
  { "IMUZ", (getter)Report_get_IMUZ, (setter)Report_set_IMUZ, "IMU Z angle", NULL },

  { NULL }
};

static PyTypeObject ReportType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  "vmwrapper.Report",
  sizeof(ReportObject),
  0,
  (destructor)Report_dealloc,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
  "VM wrapper report objects",
  0,
  0,
  0,
  0,
  0,
  0,
  Report_methods,
  0,
  Report_getsetters,
  0,
  0,
  0,
  0,
  0,
  (initproc)Report_init,
  0,
  Report_new,
};

//==============================================================================
// VM methods

static void VM_dealloc(VMObject* self)
{
  delete self->pVM;
  Py_XDECREF(self->pBytecode);

  Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* VM_new(PyTypeObject* type, PyObject* args, PyObject* kwargs)
{
  VMObject* self = (VMObject*)type->tp_alloc(type, 0);
  if (self) {
    self->pVM = NULL;
    self->pBytecode = NULL;
  }

  return (PyObject*)self;
}

static int VM_init(VMObject* self, PyObject* args, PyObject* kwargs)
{
  static char *kwlist[] = { "bytecode", "stacksize", NULL };
  int stacksize;

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "Oi:__init__", kwlist, &self->pBytecode, &stacksize))
    return -1;

  if (!PyBytes_Check(self->pBytecode)) {
    self->pBytecode = NULL;
    PyErr_SetString(PyExc_TypeError, "bytecode must be a bytes object");
    return -1;
  }

  Py_INCREF(self->pBytecode);
  self->pVM = new PyVM((uint8_t*)PyBytes_AsString(self->pBytecode), stacksize);

  return 0;
}

static PyObject* VM_get_stack(VMObject* self, PyObject* args, PyObject* kwargs)
{
  static char* kwlist[] = { NULL };
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, ":get_stack", kwlist))
    return NULL;

  return self->pVM->GetStack();
}

static PyObject* VM_push(VMObject* self, PyObject* args, PyObject* kwargs)
{
  static char* kwlist[] = { "values", NULL };
  PyObject* values;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O:push", kwlist, &values))
    return NULL;

  if (!PyBytes_Check(values)) {
    PyErr_SetString(PyExc_TypeError, "stack must be a bytes object");
    return NULL;
  }

  int size = PyBytes_Size(values);
  if (size + self->pVM->GetSP() > self->pVM->m_StackSize) {
    PyErr_SetString(PyExc_RuntimeError, "Stack overflow");
    return NULL;
  }

  self->pVM->Push((uint8_t*)PyBytes_AsString(values), size);

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject* VM_step(VMObject* self, PyObject* args, PyObject* kwargs)
{
  static char* kwlist[] = { "report", NULL };
  PyObject* pyreport;
  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O:step", kwlist, &pyreport))
    return NULL;

  if (!PyObject_IsInstance(pyreport, (PyObject*)&ReportType)) {
    PyErr_SetString(PyExc_TypeError, "report must be a Report");
    return NULL;
  }

  bool ret = self->pVM->Step((ReportObject*)pyreport);

  PyObject* r = (ret ? Py_True : Py_False);
  Py_INCREF(r);
  return r;
}

static PyMethodDef VM_methods[] = {
  { "get_stack", (PyCFunction)VM_get_stack, METH_VARARGS|METH_KEYWORDS },
  { "push", (PyCFunction)VM_push, METH_VARARGS|METH_KEYWORDS },
  { "step", (PyCFunction)VM_step, METH_VARARGS|METH_KEYWORDS },

  { NULL }
};

static PyObject* VM_get_offset(VMObject* self, void*)
{
  return Py_BuildValue("i", (int)self->pVM->GetOffset());
}

#define GETSET_IVAL(name)                                        \
  static PyObject* IDCAT(VM_get_, name)(VMObject* self, void*) { \
    return Py_BuildValue("i", self->pVM-> IDCAT(Get, name)());   \
  }                                                              \
  static int IDCAT(VM_set_, name)(VMObject* self, PyObject* value, void*) { \
    if (!PyLong_Check(value)) {                                           \
      PyErr_SetString(PyExc_TypeError, #name " must be an int");          \
      return -1;                                                        \
    }                                                                   \
    long v = PyLong_AsLong(value);                                      \
    if (( v < -32768) || (v > 32767)) {                                 \
      PyErr_SetString(PyExc_RuntimeError, #name " must be in the -32768..32767 range"); \
      return -1;                                                        \
    }                                                                   \
    self->pVM-> IDCAT(Set, name)(v);                                    \
    return 0;                                                           \
  }

GETSET_IVAL(SP);
GETSET_IVAL(TH);

static PyGetSetDef VM_getsetters[] = {
  { "offset", (getter)VM_get_offset, NULL, "Offset value", NULL },
  { "SP", (getter)VM_get_SP, (setter)VM_set_SP, "SP value", NULL },
  { "TH", (getter)VM_get_TH, (setter)VM_set_TH, "TH value", NULL },

  { NULL }
};

static PyTypeObject VMType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  "vmwrapper.VM",
  sizeof(VMObject),
  0,
  (destructor)VM_dealloc,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
  "VM wrapper objects",
  0,
  0,
  0,
  0,
  0,
  0,
  VM_methods,
  0,
  VM_getsetters,
  0,
  0,
  0,
  0,
  0,
  (initproc)VM_init,
  0,
  VM_new,
};

static struct PyModuleDef vmwrapper_module = {
  PyModuleDef_HEAD_INIT,
  "vmwrapper",
  "VM wrapper module",
  -1,
};

PyMODINIT_FUNC PyInit_vmwrapper(void)
{
  if (PyType_Ready(&VMType) < 0)
    return NULL;
  if (PyType_Ready(&ReportType) < 0)
    return NULL;

  PyObject* mdl = PyModule_Create(&vmwrapper_module);
  if (!mdl)
    return NULL;

  Py_INCREF(&VMType);
  PyModule_AddObject(mdl, "VM", (PyObject*)&VMType);

  Py_INCREF(&ReportType);
  PyModule_AddObject(mdl, "Report", (PyObject*)&ReportType);

  return mdl;
}
