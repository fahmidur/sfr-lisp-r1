#include <string.h>
#include "Error.h"

Error* Error_new(char* msg) {
  Error* self = malloc(sizeof(Error));
  size_t msg_strlen = strlen(msg);
  size_t msg_buflen = msg_strlen+1;
  self->msg = calloc(1, msg_buflen);
  memcpy(self->msg, msg, msg_buflen);
  return self;
}

void Error_del(Error* self) {
  free(self->msg);
  free(self);
}

void Error_print(Error* self) {
  printf("Error(%s)", self->msg);
}
