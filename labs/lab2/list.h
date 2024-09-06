#ifndef _LAB2_LIST_HPP
#define _LAB2_LIST_HPP

#include "data.h"
#include "serialized_data.h"

/**
 * Структура для элемента списка
 */
typedef struct list_t {
  data_t data;
  struct list_t *next;
  struct list_t *previous;
} list_t;

// Функции для работы со списком
void delete_serialized_string(list_t *head);
void init_list(list_t *head);
list_t *insert(list_t *head, list_t *after, data_t data);
list_t *copy_data(data_t *data);
int count_of_element(list_t *head);
int serialize_list(char **serialized_data, list_t *head,
                   serialized_data_t *s_data_t);
int deserialize_into_list(char *serialized_data, list_t **head,
                          serialized_data_t *s_data_t);
void free_list(list_t *head);
void print_list(list_t *head);

// Дополнительные функции для работы со списком
list_t *push(list_t *head, data_t data);
list_t *push_back(list_t *head, data_t data);
list_t *get_first_element(list_t *head);
list_t *get_last_element(list_t *head);
list_t *get_next_element(list_t *current);
list_t *get_previous_element(list_t *current);

#endif // _LAB2_LIST_HPP
