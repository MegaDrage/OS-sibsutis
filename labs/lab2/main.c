#include "data.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
  list_t *head = NULL;

  
  data_t data1 = {1, "Alice", 1};
  data_t data2 = {2, "Bob", 0};
  data_t data3 = {3, "Charlie", 1};
  data_t data4 = {4, "David", 0};

  
  head = push(head, data1);
  head = push(head, data2);
  head = push_back(head, data3);

  
  printf("Original list:\n");
  print_list(head);
  printf("Count of elements: %d\n", count_of_element(head));

  
  list_t *first = get_first_element(head);
  list_t *last = get_last_element(head);
  list_t *next = get_next_element(first);
  list_t *prev = get_previous_element(last);

  printf("\nNavigation:\n");
  printf("First element: ");
  print_data(&first->data);
  printf("Last element: ");
  print_data(&last->data);
  printf("Next element after first: ");
  print_data(&next->data);
  printf("Previous element before last: ");
  print_data(&prev->data);

  
  head = insert(head, first, data4);

  
  printf("\nList after insertion:\n");
  print_list(head);
  printf("Count of elements: %d\n", count_of_element(head));

  
  char *serialized_data;
  serialized_data_t s_data_t;
  if (!serialize_list(&serialized_data, head, &s_data_t)) {
    printf("\nSerialized data: %d\n", serialized_data[0]);
  }
  list_t *other = NULL;
  if (!deserialize_into_list(serialized_data, &other, &s_data_t)) {
    printf("\nDeserialized list:\n");
    print_list(other);
  }

  
  delete_serialized_string(other);
  free(serialized_data);
  free_list(head);
  free_list(other);

  return 0;
}
