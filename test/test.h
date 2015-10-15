#ifndef __TEST_H__
#define __TEST_H__

#include <stdbool.h>

enum {
  MAX_SUITES = 2048
};

struct Suite {
  const char *name;
  void (*run)(struct Suite *self);
  int num_tests_failed;
  int num_tests_passed;
};

#define T_OK      "✓"
#define T_FAIL    "x"

#define T_SUITE(name) \
  void test_suite_##name(struct Suite *self); \
  __attribute__((constructor)) static void initialize_suite_##name(void) { add_suite(#name, test_suite_##name); } \
  void test_suite_##name(struct Suite *self)

#define T_TEST(name) for ( \
  struct { char *test_name; int i; int num_failed; } runner = { name, 0, 0 }; \
  runner.i == 0; \
  runner.i++, test_finished(self, runner.num_failed) \
)

#define T_ASSERT(expr) assert_run((int)expr, #expr, &runner.num_failed, runner.test_name, __func__, __FILE__, __LINE__);


void assert_run(int result, const char *expr, int *num_failed, const char *name, const char *suite, const char *file, int line);
void add_suite(const char *name, void (*func)(struct Suite *self));
void test_finished(struct Suite *suite, int num_failed);
int run_test_suites(void);

#endif
