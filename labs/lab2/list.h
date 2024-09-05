#ifndef _LAB2_LIST_HPP
#define _LAB2_LIST_HPP
#include "data.h"
typedef struct list_t {
  data_t data;
  struct list_t *next;
} list_t;

void init_list(list_t *head);
list_t *insert(list_t *head, data_t data);
list_t *copy_data(data_t *data);
int count_of_element(list_t *head);
int serialize_list(char **serialized_data, list_t *head);
void free_list(list_t *head);
void print_list(list_t *head);
#endif
