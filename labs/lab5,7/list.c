#include "list.h"
#include "data.h"
#include <stdio.h>
#include <stdlib.h>
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
