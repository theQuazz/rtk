#include <stdio.h>

#include "test.h"

int num_suites = 0;
struct Suite suites[MAX_SUITES];

void assert_run(int result, const char *expr, int *num_failed, const char *test, const char *suite, const char *file, int line) {
  if (!result) {
    (*num_failed)++;
    printf("\n\tFailed: %s at %s:%d: expect that %s\n\t", test, file, line, expr);
  }
}

void test_finished(struct Suite *suite, int num_asserts_failed) {
  if (num_asserts_failed == 0) {
    suite->num_tests_passed++;
    printf(T_OK);
  } else {
    suite->num_tests_failed++;
    printf(T_FAIL);
  }
}

void add_suite(const char *name, void (*func)(struct Suite *self)) {
  int num = num_suites++;
  suites[num].run = func;
  suites[num].name = name;
}

int run_test_suites(void) {
  int num_failed = 0, num_passed = 0;
  for ( int i = 0; i < num_suites; i++ ) {
    printf("Suite %s: ", suites[i].name);
    suites[i].run(&suites[i]);
    printf("\n");

    num_passed += suites[i].num_tests_passed;
    num_failed += suites[i].num_tests_failed;
  }
  printf("Passed: %d, Failed: %d\n", num_passed, num_failed);
  return num_failed;
}
