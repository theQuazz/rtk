#include "rtk.h"
#include "test/test.h"

int main(void) {
#ifdef TEST
  return run_test_suites();
#else
  rtk_init();
  rtk_launch();
#endif
}
