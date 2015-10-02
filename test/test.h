#ifndef __TEST_H__
#define __TEST_H__

#include <stdbool.h>

#define TEST(name) __attribute__ ((__constructor__)) void name(void)

void test_start(const char *description);
void test_end(void);
void assert_ok(bool expr, char *failure_msg);

/*
TEST(IntTest) {
  test_start("1 shouldn't equal 2");
  assert_ok(1 != 2);
  test_end();
}
 */

#endif
