#ifndef _LAB2_DATA_H
#define _LAB2_DATA_H
#include <stdlib.h>
typedef struct data_t {
  int number;
  char *name;
  short is_student;
} data_t;

data_t *init_data(int number, char *name, short is_student);
int serialize_data(data_t *data, char **serialized, size_t *s_size);
void print_data(data_t *data);
int deserialize_data(char **serialized_data, data_t *deserialized_data);
#endif
