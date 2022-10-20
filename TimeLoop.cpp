#include "TimeLoop.h"



#define YEAR_OFFSET 2020 // 1.1.2020 = 1

const String dow[7] = { "Mo", "Di", "Mi", "Do", "Fr", "Sa", "So" };


static const int monthDays[]={ 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; // starts months from 1


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

    // note: the seconds and days- Counter have to be set after
    // construction!
}

/* getter / setter */

void TimeLoop::setSecondsCounter(long value) {
    secondsCounter = value;
}

void TimeLoop::setDayCounter(long value) {
    dayCounter = value;
    breakDayCounter();

}

long TimeLoop::getSecondsCounter() {
    return secondsCounter;
}
long TimeLoop::getDayCounter() {
    return dayCounter;
}


/** @brief adds value to secondsCounter. returns true if overflow
  */
bool TimeLoop::incrementSecondsCounter(long increment) {
    secondsCounter = secondsCounter + increment;
    if (secondsCounter < 0) {
        secondsCounter += 86400;
        incrementDayCounter(-1);
        return true;
    }
    if (secondsCounter >= 86400) {
        secondsCounter -= 86400;
        incrementDayCounter(1);
        return true;
    }
    return false;
}

void TimeLoop::incrementDayCounter(int increment) {
    dayCounter = dayCounter + increment;
    breakDayCounter();
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
          if (incrementSecondsCounter(1)) {
            return 3;
          }
          return 2;
        }
        return 1;
    }
    return 0;
}

/** @brief returns a string hh:mm:ss
  *
  */
String TimeLoop::getHrsMinSec() {
    secs = secondsCounter % 60;
    unsigned int remains = secondsCounter / 60;
    mins = remains % 60;
    hrs = remains / 60;
    return lFill(String(hrs)) + ":" + lFill(String(mins)) + ":" +  lFill(String(secs));
}

/** @brief returns a string hh:mm:ss
  *
  */
String TimeLoop::getDayMonYear() {

    return lFill(String(day)) + "." + lFill(String(month)) + "." + String(year);
}


/** @brief returns the day of week. Actual, in future or passed
  *
  */
byte TimeLoop::getDow(int increment){
    wDay = ((dayCounter + 4) % 7);  // Monday is day 0
    wDay += increment;
    if (wDay > 6) { wDay = 0; }
    if (wDay < 0) { wDay = 6; }
    return wDay;
}
String TimeLoop::getDowName() {
    return dow[getDow(0)];
}

byte TimeLoop::getMonth(int increment) {
    return month + increment;
}

/** @brief (one liner)
  *
  * break the given dayCounter counting from 1.1.2020 into day / month / year
  * derived from C library localtime function
*/

void TimeLoop::breakDayCounter(){
    // note that daysCounter has offset from year 2020

    // calculate the year
    year = YEAR_OFFSET;
    unsigned long myDays = 0;
    while((unsigned)(myDays += (LEAP_YEAR(year) ? 366 : 365)) <= dayCounter) {
        year++;
    }

    // calculate month
    myDays -= LEAP_YEAR(year) ? 366 : 365;
    byte monthLength = 0;
    for (month = 1; month < 13; month++) {
        monthLength = monthDays[month];
        if (month == 2) { // february
            if (LEAP_YEAR(year)) {
                monthLength++;
            }
        }
        if (myDays > monthLength) {
          myDays -= monthLength;
        }
        else {
            break;
        }
    }
    day = myDays + 1; // day of month
}

