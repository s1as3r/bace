#include "bace/time.h"

internal u16 days_in_month_(Month month, u32 year) {
  u16 c = 0;
  switch (month) {
  case Month_Jan:
    c = 31;
    break;
  case Month_Feb: {
    if ((year % 4 == 0) && ((year % 100) != 0 || (year % 400) == 0)) {
      c = 29;
    } else {
      c = 28;
    }
  } break;
  case Month_Mar:
    c = 31;
    break;
  case Month_Apr:
    c = 30;
    break;
  case Month_May:
    c = 31;
    break;
  case Month_Jun:
    c = 30;
    break;
  case Month_Jul:
    c = 31;
    break;
  case Month_Aug:
    c = 31;
    break;
  case Month_Sep:
    c = 30;
    break;
  case Month_Oct:
    c = 31;
    break;
  case Month_Nov:
    c = 30;
    break;
  case Month_Dec:
    c = 31;
    break;
  default:
    assert(false); // unreachable
  }
  return c;
}

internal DateTime date_time_from_unix_seconds_(u64 unix_time) {
  DateTime date = {0};
  date.year = 1970;

  u64 days_since_epoch = unix_time / 86400;
  date.day = (u16)days_since_epoch;
  date.sec = (u16)(unix_time % 60);
  date.min = (u16)((unix_time / 60) % 60);
  date.hour = (u16)((unix_time / 3600) % 24);
  date.wday = (u32)((days_since_epoch + WeekDay_Thu) % 7);

  for (;;) {
    for (date.month = 0; date.month < 12; ++date.month) {
      u16 c = days_in_month_((Month)date.month, date.year);
      if (date.day < c) {
        return date;
      }
      date.day -= c;
    }
    ++date.year;
  }
}

DenseTime dense_time_from_date_time(DateTime date_time) {
  assert(date_time.mon < 12);
  assert(date_time.day <= 30);
  assert(date_time.hour < 24);
  assert(date_time.min < 60);
  assert(date_time.sec < 60);
  assert(date_time.msec < 1000);
  assert(date_time.micro_sec < 1000);

  DenseTime result = 0;
  result += date_time.year;
  result *= 12;
  result += date_time.mon;
  result *= 31;
  result += date_time.day;
  result *= 24;
  result += date_time.hour;
  result *= 60;
  result += date_time.min;
  result *= 60;
  result += date_time.sec;
  result *= 1000;
  result += date_time.msec;
  result *= 1000;
  result += date_time.micro_sec;
  return (result);
}

DateTime date_time_from_dense_time(DenseTime time) {
  DateTime result = {0};
  result.micro_sec = time % 1000;
  time /= 1000;
  result.msec = time % 1000;
  time /= 1000;
  result.sec = time % 60;
  time /= 60;
  result.min = time % 60;
  time /= 60;
  result.hour = time % 24;
  time /= 24;
  result.day = (u16)(time % 31);
  time /= 31;
  result.mon = time % 12;
  time /= 12;
  assert(time <= UINT32_MAX);
  result.year = (u32)time;
  return (result);
}

DateTime date_time_from_micro_seconds(u64 time) {
  u64 whole_seconds = time / 1000000;
  u32 remainder_micros = (u32)(time % 1000000);

  DateTime result = date_time_from_unix_seconds_(whole_seconds);
  result.msec = (u16)(remainder_micros / 1000);
  result.micro_sec = (u16)(remainder_micros % 1000);
  return result;
}

DateTime date_time_from_unix_time(u64 unix_time) {
  return date_time_from_unix_seconds_(unix_time);
}
