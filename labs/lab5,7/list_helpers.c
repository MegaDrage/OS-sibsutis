#include "list.h"
#include <stdlib.h>
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

void free_list(list_t *head) {
  while (head) {
    list_t *temp = head;
    head = head->next;
    free(temp);
  }
}
