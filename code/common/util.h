#ifndef COMMON_UTIL_H_
#define COMMON_UTIL_H_

#include <stdio.h>

// allows to toggle on and off error messages
#define PRINT_ERRORS 1

// makes error 1 liners
int printError(char* message,
	       int   return_val);

// makes error 1 line with returning NULL
void* printErrorNull(char* message);

// prints before exiting program
void printFatal(char* message, int id);


int printError(char* message, int return_val) {
  if (1 == PRINT_ERRORS) {
    printf("%s\n", message);
  }
  return return_val;
}

void* printErrorNull(char* message) {
  if (1 == PRINT_ERRORS) {
    printf("%s\n", message);
  }
  return NULL;
}

void printFatal(char* message, int id) {
  printf("\n*************************************\n");
  printf("Fatel Error %d:\n%s\n", id, message);
  printf("*************************************\n");
  exit(id);
}

#endif COMMON_UTIL_H_
