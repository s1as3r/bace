#ifndef _H_TIME
#define _H_TIME

#include "bace/base.h"

// time types
typedef enum WeekDay {
  WeekDay_Sun,
  WeekDay_Mon,
  WeekDay_Tue,
  WeekDay_Wed,
  WeekDay_Thu,
  WeekDay_Fri,
  WeekDay_Sat,
  WeekDay_COUNT,
} WeekDay;

typedef enum Month {
  Month_Jan,
  Month_Feb,
  Month_Mar,
  Month_Apr,
  Month_May,
  Month_Jun,
  Month_Jul,
  Month_Aug,
  Month_Sep,
  Month_Oct,
  Month_Nov,
  Month_Dec,
  Month_COUNT,
} Month;

typedef struct DateTime {
  u16 micro_sec; // [0, 999]
  u16 msec;      // [0, 999]
  u16 sec;       // [0, 60]
  u16 min;       // [0, 59]
  u16 hour;      // [0, 24]
  u16 day;       // [0, 30]
  union {
    WeekDay week_day;
    u32 wday;
  };
  union {
    Month month;
    u32 mon;
  };
  u32 year; // 1 = 1 CE, 0 = 1 BC
} DateTime;

typedef u64 DenseTime;

DenseTime dense_time_from_date_time(DateTime date_time);
DateTime date_time_from_dense_time(DenseTime time);
DateTime date_time_from_micro_seconds(u64 time);
DateTime date_time_from_unix_time(u64 unix_time);

#endif // !_H_TIME
