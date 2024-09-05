#include "list.h"
#include "data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
void init_list(list_t *head) {
  if (!head)
    return;
  head->next = NULL;
}

list_t *copy_data(data_t *data) {
  list_t *temp = calloc(1, sizeof(list_t));
  if (!temp)
    return NULL;
  temp->data = *data;
  temp->next = NULL;
  return temp;
}

list_t *insert(list_t *head, data_t data) {
  list_t *temp = copy_data(&data);
  if (!head) {
    return temp;
  }
  list_t *current = head;
  while (current->next) {
    current = current->next;
  }
  current->next = temp;
  return head;
}

void free_list(list_t *head) {
  while (head) {
    list_t *temp = head;
    head = head->next;
    free(temp);
  }
}

void print_list(list_t *head) {
  while (head) {
    print_data(&head->data);
    head = head->next;
  }
}

int count_of_element(list_t *head) {
  int count = 0;
  while (head) {
    head = head->next;
    count++;
  }
  return count;
}

int serialize_list(char **serialized_data, list_t *head) {
  if (!head)
    return -1;

  size_t total_size = 0;
  list_t *current = head;

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
