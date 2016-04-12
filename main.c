#include "kernel/rtk.h"
#include "test/test.h"

int main(void) {
#ifdef TEST
  return run_test_suites();
#else
  RtkLaunch();
#endif
  return 0;
}
