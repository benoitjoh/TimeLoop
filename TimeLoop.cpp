#include "TimeLoop.h"


const String dow[7] = {"Mo", "Di", "Mi", "Do", "Fr", "Sa", "So" };


static const int monthDays[]={0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; // starts months from 1


/** @brief helper that makes numbers 2 digits
  */
String lFill(String a) {
    // fills a string with letters from left side that the resulting length is reached
    if (a.length() < 2) {
        a = "0" + a;
    }
    return a;
}


/** @brief Constructor
  */
TimeLoop::TimeLoop(int dummy) {
    lastDeciSecsIncMillis = 0;
    deciSecondsCounter = 0;

}

/* getter / setter */

void TimeLoop::setSecondsTicker(long value) {
    secondsTicker = value;
}

void TimeLoop::setDayTicker(long value) {
    dayTicker = value;
}

long TimeLoop::getSecondsTicker() {
    return secondsTicker;
}
long TimeLoop::getDayTicker() {
    return dayTicker;
}


/** @brief adds value to secondsTicker. returns true if overflow
  */
bool TimeLoop::incrementSecondsTicker(long value) {
    secondsTicker = secondsTicker + value;
    if (secondsTicker < 0) {
        secondsTicker += 86400;
        dayTicker--;
        return true;
    }
    if (secondsTicker >= 86400) {
        secondsTicker -= 86400;
        dayTicker++;
        return true;
    }
    return false;
}

void TimeLoop::incrementDayTicker(long value) {
    dayTicker = dayTicker + value;
}






/** @brief called in a mainloop it increments the timer
  *
  * if 100ms has passed,       --> return 1
  * if 1 second has passed      --> return 2
  * if a day finished (midnight) --> returns 3
  */
byte TimeLoop::actualize() {
    if ( millis() - lastDeciSecsIncMillis >= 100 ) {
        deciSecondsCounter++;
        lastDeciSecsIncMillis += 100;
        if (deciSecondsCounter >= 10) {
          deciSecondsCounter = 0;
          if (incrementSecondsTicker(1)) {
            return 3;
          }
          return 2;
        }
        return 1;
    }
    return 0;
}

String TimeLoop::getHrsMinSec() {

    secs = secondsTicker % 60;
    unsigned int remains = secondsTicker / 60;
    mins = remains % 60;
    hrs = remains / 60;
    return lFill(String(hrs)) + ":" + lFill(String(mins)) + ":" +  lFill(String(secs));
}

byte TimeLoop::getDow(int increment){
    wDay = ((dayTicker + 4) % 7);  // Monday is day 0
    wDay += increment;
    if (wDay > 6) { wDay = 0}
    if (wDay < 0) { wDay = 6}
    return wDay;
}
String TimeLoop::getDowName() {
    return dow[getDow(0)];
}

byte getMonth(int increment) {
    return month;
}

/** @brief (one liner)
  *
  * break the given time_t into time components
  * this is a more compact version of the C library localtime function

void TimeLoop::breakTime(){
//
// note that year is offset from 1970 !!!

  uint8_t year;
  uint8_t month, monthLength;
  uint32_t time;
  unsigned long days;

  time = (uint32_t)timeInput;
  tm.Second = time % 60;
  time /= 60; // now it is minutes
  tm.Minute = time % 60;
  time /= 60; // now it is hours
  tm.Hour = time % 24;
  time /= 24; // now it is days
  tm.Wday = ((time + 4) % 7) + 1;  // Sunday is day 1

  year = 0;
  days = 0;
  while((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= time) {
    year++;
  }
  tm.Year = year; // year is offset from 1970

  days -= LEAP_YEAR(year) ? 366 : 365;
  time  -= days; // now it is days in this year, starting at 0

  days=0;
  month=0;
  monthLength=0;
  for (month=0; month<12; month++) {
    if (month==1) { // february
      if (LEAP_YEAR(year)) {
        monthLength=29;
      } else {
        monthLength=28;
      }
    } else {
      monthLength = monthDays[month];
    }

    if (time >= monthLength) {
      time -= monthLength;
    } else {
        break;
    }
  }
  tm.Month = month + 1;  // jan is month 1
  tm.Day = time + 1;     // day of month
}
*/
