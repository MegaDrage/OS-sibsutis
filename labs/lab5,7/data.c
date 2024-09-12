#include "data.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int serialize_data(data_t *data, char **serialized, size_t *s_size) {
  if (data == NULL || serialized == NULL || s_size == NULL) {
    return -1;
  }

  int str_len = strlen(data->name);
  *s_size = sizeof(int) + sizeof(int) + str_len + sizeof(short);
  *serialized = malloc(*s_size);
  if (*serialized == NULL) {
    perror("Couldn't allocate memory");
    return -1;
  }

  char *ptr = *serialized;
  memcpy(ptr, &str_len, sizeof(int));
  ptr += sizeof(int);
  memcpy(ptr, &data->number, sizeof(int));
  ptr += sizeof(int);
  memcpy(ptr, data->name, str_len);
  ptr += str_len;
  memcpy(ptr, &data->is_student, sizeof(short));

  return 0;
}

void print_data(data_t *data) {
  printf("number of: %d, Name of: %s, is_student: %d\n", data->number,
         data->name, data->is_student);
}

int deserialize_data(char **serialized_data, data_t *deserialized_data) {
    if (deserialized_data == NULL) {
        perror("DATA IS NULL");
        return -1;
    }

    if (serialized_data == NULL || *serialized_data == NULL) {
        perror("SERIALIZED DATA IS NULL");
        return -1;
    }

    int str_len;

    memcpy(&str_len, *serialized_data, sizeof(int));
    *serialized_data += sizeof(int);

    memcpy(&deserialized_data->number, *serialized_data, sizeof(int));
    *serialized_data += sizeof(int);
    
    deserialized_data->name = malloc((str_len + 1) * sizeof(char));
    if (deserialized_data->name == NULL) {
        perror("MALLOC FAILED");
        return -1;
    }
    memcpy(deserialized_data->name, *serialized_data, str_len);
    deserialized_data->name[str_len] = '\0';
    *serialized_data += str_len;

    memcpy(&deserialized_data->is_student, *serialized_data, sizeof(short));
    *serialized_data += sizeof(short);

    return 0;
}
