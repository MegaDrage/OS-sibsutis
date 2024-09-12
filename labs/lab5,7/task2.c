#define _GNU_SOURCE
#include "list.h"
#include <dlfcn.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void print_maps() {
  char maps_file[256];
  snprintf(maps_file, sizeof(maps_file), "/proc/%d/maps", getpid());
  int fd = open(maps_file, O_RDONLY);
  if (fd == -1) {
    perror("open");
    return;
  }

  char buffer[4096];
  ssize_t bytes_read;
  while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
    write(STDOUT_FILENO, buffer, bytes_read);
  }
  close(fd);
}

int main() {
  void *handle;
  void (*init_list)(list_t *);
  list_t *(*push)(list_t *, data_t data);
  void (*print_list)(list_t *);
  void (*free_list)(list_t *);
  char *error;
  
  handle = dlopen("./liblist.so", RTLD_LAZY);
  if (!handle) {
    fprintf(stderr, "%s\n", dlerror());
    exit(EXIT_FAILURE);
  }
  
  printf("Содержимое /proc/%d/maps до загрузки функций:\n", getpid());
  print_maps();
  
  *(void **)(&init_list) = dlsym(handle, "init_list");
  *(list_t ***)(&push) = dlsym(handle, "push");
  *(void **)(&print_list) = dlsym(handle, "print_list");
  *(void **)(&free_list) = dlsym(handle, "free_list");

  if ((error = dlerror()) != NULL) {
    fprintf(stderr, "%s\n", error);
    dlclose(handle);
    exit(EXIT_FAILURE);
  }
  
  printf("Содержимое /proc/%d/maps после загрузки функций:\n", getpid());
  print_maps();
 
  list_t *myList = NULL;
  init_list(myList);

  data_t data1 = {1, "Alice", 1};
  data_t data2 = {2, "Bob", 0};
  data_t data3 = {3, "Charlie", 1};
  myList = push(myList, data1);
  myList = push(myList, data2);
  myList = push(myList, data3);

  printf("Список: ");
  print_list(myList);

  free_list(myList);

  dlclose(handle);

  printf("Содержимое /proc/%d/maps после выгрузки библиотеки:\n", getpid());
  print_maps();

  return 0;
}
