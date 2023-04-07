
#include "py/builtin.h"
#include "py/runtime.h"
#include "emscripten.h"

/*

int setInterval(func_t cb, int period) {
  return EM_ASM_INT({ return setInterval($0, $1); }, cb, period);  // The part in brace is actual javascript
};
int setTimeout(func_t cb, int period) {
  return EM_ASM_INT({ return setTimeout($0, $1); }, cb, period);
};
int clearInterval(int id) {
  return EM_ASM_INT({ return clearInterval($0); }, id);
};
int clearTimeout(int id) {
  return EM_ASM_INT({ return clearTimeout($0); }, id);
};
*/

struct py_jswindow_cb_holder {
  mp_obj_t cb_obj;
  int period;
};

STATIC void invoke_cb(struct py_jswindow_cb_holder *holder, mp_obj_t cb_obj) {
  nlr_buf_t nlr;
  if(nlr_push(&nlr) == 0) {
    mp_call_function_1(cb_obj, MP_OBJ_FROM_PTR(holder));
    nlr_pop();
  } else {
    // uncaught exception
    mp_obj_base_t *exc = (mp_obj_base_t *)nlr.ret_val;
    mp_printf(MICROPY_ERROR_PRINTER, "Unhandled exception in setTimeout/setInterval\n");
    mp_obj_print_exception(MICROPY_ERROR_PRINTER, MP_OBJ_FROM_PTR(exc));
  }

}

STATIC void invoke_py_setInterval_cb(void *arg) {
  struct py_jswindow_cb_holder *holder = (struct py_jswindow_cb_holder *)arg;
  int period = holder->period;
  if(period < 0)
    return;
  emscripten_async_call(invoke_py_setInterval_cb, holder, period);
  invoke_cb(holder, holder->cb_obj);
}

// same as setInterval but skip triggering ourselves again
STATIC void invoke_py_setTimeout_cb(void *arg) {
  struct py_jswindow_cb_holder *holder = (struct py_jswindow_cb_holder *)arg;
  int period = holder->period;
  if(period < 0)
    return;
  invoke_cb(holder, holder->cb_obj);
}

STATIC struct py_jswindow_cb_holder *js_jswindow_createHolder(mp_obj_t cb_obj, mp_obj_t period_obj) {
if(!mp_obj_is_callable(cb_obj))
    mp_raise_msg(&mp_type_TypeError, MP_ERROR_TEXT("not callable"));
  int period = mp_obj_get_int(period_obj);
  if(period < 0)
    mp_raise_msg(&mp_type_TypeError, MP_ERROR_TEXT("period must be >= 0"));
  struct py_jswindow_cb_holder *holder = m_malloc(sizeof(struct py_jswindow_cb_holder));
  holder->cb_obj = cb_obj;
  holder->period = period;
  return holder;
}

STATIC mp_obj_t js_jswindow_setInterval(mp_obj_t cb_obj, mp_obj_t period_obj) {
  struct py_jswindow_cb_holder *holder = js_jswindow_createHolder(cb_obj, period_obj);
  emscripten_async_call(invoke_py_setInterval_cb, holder, holder->period);
  return MP_OBJ_FROM_PTR(holder);
}

STATIC mp_obj_t js_jswindow_setTimeout(mp_obj_t cb_obj, mp_obj_t period_obj) {
  struct py_jswindow_cb_holder *holder = js_jswindow_createHolder(cb_obj, period_obj);
  emscripten_async_call(invoke_py_setTimeout_cb, holder, holder->period);
  return MP_OBJ_FROM_PTR(holder);
}

// shared implementation for both clearTimeout/clearInterval
STATIC mp_obj_t js_jswindow_clearTimeout(mp_obj_t holder_obj) {
  if (holder_obj == NULL || holder_obj == mp_const_none)
    mp_raise_type(&mp_type_TypeError);
  struct py_jswindow_cb_holder *holder = MP_OBJ_TO_PTR(holder_obj);
  if(holder != NULL)
    holder->period = -1;
  return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_2(jswindow_setInterval_obj, js_jswindow_setInterval);
MP_DEFINE_CONST_FUN_OBJ_2(jswindow_setTimeout_obj, js_jswindow_setTimeout);
MP_DEFINE_CONST_FUN_OBJ_1(jswindow_clearTimeout_obj, js_jswindow_clearTimeout);
MP_DEFINE_CONST_FUN_OBJ_1(jswindow_clearInterval_obj, js_jswindow_clearTimeout);

STATIC const mp_rom_map_elem_t mp_module_jswindow_globals_table[] = {
  { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_jswindow) },
  { MP_ROM_QSTR(MP_QSTR_setInterval), MP_ROM_PTR(&jswindow_setInterval_obj) },
  { MP_ROM_QSTR(MP_QSTR_setTimeout), MP_ROM_PTR(&jswindow_setTimeout_obj) },
  { MP_ROM_QSTR(MP_QSTR_clearTimeout), MP_ROM_PTR(&jswindow_clearTimeout_obj) },
  { MP_ROM_QSTR(MP_QSTR_clearInterval), MP_ROM_PTR(&jswindow_clearInterval_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_jswindow_globals, mp_module_jswindow_globals_table);

const mp_obj_module_t mp_module_jswindow = {
  .base = { &mp_type_module },
  .globals = (mp_obj_dict_t *)&mp_module_jswindow_globals,
};

MP_REGISTER_MODULE(MP_QSTR_jswindow, mp_module_jswindow);
