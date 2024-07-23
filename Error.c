#include <string.h>
#include <limits.h>
#include "Error.h"
#include "Util.h"

// TODO: this should be made thread-local
int    _g_error_code = 0;
Error  _g_error = {.msg = ""};
void*  _g_error_ptr = NULL;

void ErrorSystem_init() {
  _g_error_code = 0;
  _g_error.code = 0;
  _g_error.msg = "";
  _g_error_ptr = NULL;
}

void ErrorSystem_done() {
  _g_error_code = 0;
  _g_error.code = 0;
  _g_error.msg = "";
  _g_error_ptr = NULL;
}

void ErrorSystem_reset() {
  ErrorSystem_init();
}

void ErrorSystem_set(int code, char* msg)  {
  _g_error_ptr = NULL;
  _g_error_code = code;
  _g_error.code = code;
  _g_error.msg = msg;
}

void ErrorSystem_set_ptr(int code, void* ptr) {
  _g_error_code = code;
  _g_error.code = 0;
  _g_error.msg = "";
  _g_error_ptr = ptr;
}

int ErrorSystem_get_code() {
  return _g_error_code;
}

char* ErrorSystem_get_msg() {
  if(_g_error_code == 0) {
    return "";
  }
  return _g_error.msg;
}

void* ErrorSystem_get_ptr() {
  if(_g_error_code == 0) {
    return NULL;
  }
  return _g_error_ptr;
}

Error* Error_new(char* msg) {
  Error* self = calloc(1, sizeof(Error));
  size_t msg_strlen = strlen(msg);
  size_t msg_buflen = msg_strlen+1;
  self->msg = calloc(1, msg_buflen);
  memcpy(self->msg, msg, msg_buflen);
  self->code = (msg_strlen > 0) ? 1 : 0;
  return self;
}

void Error_del(Error* self) {
  if(self->msg != NULL) {
    free(self->msg);
    self->msg = NULL;
  }
  free(self);
}

void Error_print(Error* self) {
  /* printf("Error(%s)", self->msg); */
  printf("Error(");
  Util_vt_set(VT_COLOR_RED_FG);
  Util_cstr_print(self->msg);
  Util_vt_set(VT_RESET);
  printf(")");
}

ssize_t Error_len(Error* self) {
  size_t size = strlen(self->msg);
  if(size <= SSIZE_MAX) {
    return (ssize_t)size;
  }
  return -1;
}

Error* Error_clone(Error* self) {
  return Error_new(self->msg);
}
