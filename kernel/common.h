#ifndef __KERNEL_COMMON_H__
#define __KERNEL_COMMON_H__

/** @brief Kernel wide success status code
 *
 *  Returned when a kernel primitive completes without error and is documented
 *  to return zero on success.
 */
enum ReturnStatus {
  RETURN_STATUS_OK = 0,
};

#endif
