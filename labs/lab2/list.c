#include "list.h"
#include "data.h"
#include "serialized_data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void init_list(list_t *head) {
  if (!head)
    return;
  head->next = NULL;
  head->previous = NULL;
}

list_t *copy_data(data_t *data) {
  list_t *temp = calloc(1, sizeof(list_t));
  if (!temp)
    return NULL;
  temp->data = *data;
  temp->next = NULL;
  temp->previous = NULL;
  return temp;
}

list_t *push(list_t *head, data_t data) {
  list_t *temp = copy_data(&data);
  if (!temp) {
    return head;
  }

  if (!head) {
    return temp;
  }

  temp->next = head;
  head->previous = temp;
  return temp;
}

list_t *push_back(list_t *head, data_t data) {
  list_t *temp = copy_data(&data);
  if (!head) {
    return temp;
  }
  list_t *current = head;
  while (current->next) {
    current = current->next;
  }
  current->next = temp;
  temp->previous = current;
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
list_t *insert(list_t *head, list_t *after, data_t data) {
  if (!after) {
    return push(head, data); 
  }

  list_t *temp = copy_data(&data);
  if (!temp) {
    return head; 
  }

  temp->next = after->next; 
  if (after->next) {
    after->next->previous = temp; 
  }
  temp->previous = after; 
  after->next = temp; 
  return head;
}

void delete_serialized_string(list_t *head) {
  while (head) {
    free(head->data.name);
    head = head->next;
  }
}

list_t *get_first_element(list_t *head) {
  if (!head) {
    return NULL;
  }
  while (head->previous) {
    head = head->previous;
  }
  return head;
}

list_t *get_last_element(list_t *head) {
  if (!head) {
    return NULL;
  }
  while (head->next) {
    head = head->next;
  }
  return head;
}

list_t *get_next_element(list_t *current) {
  if (!current) {
    return NULL;
  }
  return current->next;
}

list_t *get_previous_element(list_t *current) {
  if (!current) {
    return NULL;
  }
  return current->previous;
}
