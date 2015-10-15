#ifndef __RTK_H__
#define __RTK_H__

enum RTKStatus {
  RTK_OK,
  RTK_ERROR,
};

void rtk_init(void);
void rtk_launch(void);

#endif
