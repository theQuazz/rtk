#include "kernel/rtk.h"
#include "test/test.h"

int main(void) {
#ifdef TEST
  return run_test_suites();
#else
  RtkInit();
  RtkLaunch();
#endif
  return 0;
}
