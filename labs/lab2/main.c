#include "data.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
  list_t *head = NULL;
  init_list(head);

  data_t data1 = {1, "Alice", 1};
  data_t data2 = {2, "Bob", 0};

  head = insert(head, data1);
  head = insert(head, data2);
  print_list(head);
  printf("Count of elements: %d\n", count_of_element(head));

  char *serialized_data;
  if (!serialize_list(&serialized_data, head)) {
    printf("Serialized data: %d\n", serialized_data[0]);
  }
  free(serialized_data);

  free_list(head);
  return 0;
}
