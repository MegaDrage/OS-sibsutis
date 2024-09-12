#include "list.h"
#include "serialized_data.h"
#include <string.h>
void delete_serialized_string(list_t *head) {
  while (head) {
    free(head->data.name);
    head = head->next;
  }
}
int deserialize_into_list(char *serialized_data, list_t **head,
                          serialized_data_t *s_data_t) {
  if (serialized_data == NULL || s_data_t->count_of_elements == 0)
    return -1;
  size_t counter = 0;
  while (counter != s_data_t->count_of_elements) {
    data_t temp;
    temp.name = NULL;
    temp.number = 0;
    temp.is_student = 0;
    if (deserialize_data(&serialized_data, &temp)) {
      return -1;
    }
    *head = push_back(*head, temp);
    counter++;
  }
  return 0;
}
int serialize_list(char **serialized_data, list_t *head,
                   serialized_data_t *s_data_t) {
  if (!head)
    return -1;

  size_t total_size = 0;
  list_t *current = head;
  s_data_t->count_of_elements = count_of_element(head);
  while (current) {
    char *serializer;
    size_t s_size;
    if (serialize_data(&current->data, &serializer, &s_size)) {
      return -1;
    }
    total_size += s_size;
    free(serializer);
    current = current->next;
  }

  *serialized_data = calloc(total_size + 1, sizeof(char));
  if (!serialized_data)
    return -1;

  current = head;
  char *ptr = *serialized_data;
  while (current) {
    char *serializer;
    size_t s_size;
    if (serialize_data(&current->data, &serializer, &s_size)) {
      free(*serialized_data);
      return -1;
    }
    memcpy(ptr, serializer, s_size);
    ptr += s_size;
    free(serializer);
    current = current->next;
  }

  return 0;
}
