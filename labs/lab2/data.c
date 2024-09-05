#include "data.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int serialize_data(data_t *data, char **serialized, size_t *s_size) {
  if (data == NULL || serialized == NULL || s_size == NULL) {
    return -1;
  }

  int str_len = strlen(data->name);
  *s_size = sizeof(int) + sizeof(int) + str_len + sizeof(short);
  *serialized = malloc(*s_size);
  if (*serialized == NULL) {
    perror("Couldn't allocate memory");
    return -1;
  }

  char *ptr = *serialized;
  memcpy(ptr, &str_len, sizeof(int));
  ptr += sizeof(int);
  memcpy(ptr, &data->number, sizeof(int));
  ptr += sizeof(int);
  memcpy(ptr, data->name, str_len);
  ptr += str_len;
  memcpy(ptr, &data->is_student, sizeof(short));

  return 0;
}

void print_data(data_t *data) {
  printf("number of: %d, Name of: %s, is_student: %d\n", data->number,
         data->name, data->is_student);
}
