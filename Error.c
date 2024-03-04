#include <string.h>
#include "Error.h"
#include "Util.h"

Error* Error_new(char* msg) {
  Error* self = malloc(sizeof(Error));
  size_t msg_strlen = strlen(msg);
  size_t msg_buflen = msg_strlen+1;
  self->msg = calloc(1, msg_buflen);
  memcpy(self->msg, msg, msg_buflen);
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

Error* Error_clone(Error* self) {
  return Error_new(self->msg);
}
