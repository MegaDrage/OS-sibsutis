#include "list.h"
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
