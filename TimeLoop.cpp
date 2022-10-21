#include "TimeLoop.h"

#define YEAR_OFFSET 2020 // 1.1.2020 ==> 1 in the daysCounter


// --------  helper -----------------------------------------------------------------

// leap year calculator expects year argument as year yyyy
#define LEAP_YEAR(Y)     ( ((Y)>0) && !((Y)%4) && ( ((Y)%100) || !((Y)%400) ) )


static const String dow[7] = { "Mo", "Di", "Mi", "Do", "Fr", "Sa", "So" };
static const String monthNames[13]={"", "Jan", "Feb", "Mär", "Apr", "Mai", "Jun", "Jul", "Aug" , "Sep" , "Okt" , "Nov" , "Dez" };


/** @brief Method to calculate the days in a month in a given year
 *
 *                               Dec Jan Feb Mar Apr...                          Dec */
static const int monthDays[13]={ 31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

byte getMonthLength(int myMonth, int myYear) {
    byte monthLength = monthDays[myMonth];
    if (myMonth == 2) { // february
        if (LEAP_YEAR(myYear)) {
            monthLength++;
        }
    }
    return monthLength;
}



/** @brief helper that makes numbers 2 digits
  *
  */
String lFill(String a) {
    // fills a string with letters from left side that the resulting length is reached
    if (a.length() < 2) {
        a = "0" + a;
    }
    return a;
}

// -------- CLASS TimeLoop -----------------------------------------------------------------

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
    breakupDayCounter();
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
    breakupDayCounter();
}

/* methods for seting the date */

void TimeLoop::incrementMonth() {
    dayCounter += getMonthLength(month, year);
    breakupDayCounter();
}

void TimeLoop::decrementMonth() {
    dayCounter -= getMonthLength(month - 1, year);
    breakupDayCounter();
}

void TimeLoop::incrementYear() {
    dayCounter += (LEAP_YEAR(year) ? 366 : 365);
    breakupDayCounter();
}

void TimeLoop::decrementYear() {
    dayCounter -= (LEAP_YEAR(year - 1) ? 366 : 365);
    breakupDayCounter();
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

/** @brief returns a string dd.mm.yyyy
  *
  */
String TimeLoop::getDayMonYear() {

    return lFill(String(day)) + "." + lFill(String(month)) + "." + String(year);
}


/** @brief returns the weekday. Actual, in future or passed by increment days
  *
  */
int TimeLoop::getDow(int increment){
    wDay = ((dayCounter + 4) % 7);  // Monday is day 0
    wDay += increment;
    if (wDay > 6) { wDay = 0; }
    if (wDay < 0) { wDay = 6; }
    return wDay;
}
String TimeLoop::getDowName(int increment) {
    return dow[getDow(increment)];
}

int TimeLoop::getMonth(int increment) {
    int myMonth = month + increment;
    if (myMonth > 12) { myMonth = 1; }
    if (myMonth < 1) { myMonth = 12; }
    return myMonth;


}
String TimeLoop::getMonthName(int increment) {
    return monthNames[getMonth(increment)];
}



/** @brief splits dayCounter in day month year
  *
  * break the given dayCounter counting from 1.1.2020 into day / month / year
  * derived from C library localtime function
*/

void TimeLoop::breakupDayCounter(){
    // note that dayCounter has offset from year 2020

    // calculate the year. Increment from year to year until we step over dayCounter
    year = YEAR_OFFSET;
    unsigned long myDays = 0;
    while((unsigned)(myDays += (LEAP_YEAR(year) ? 366 : 365)) <= dayCounter) {
        year++;
    }
    myDays -= LEAP_YEAR(year) ? 366 : 365; // one step back to the 1.Jan of the year

    // calculate month
    myDays = dayCounter - myDays; // day of year
    for (month = 1; month < 13; month++) {
        byte monthLength = getMonthLength(month, year);

        if (myDays > monthLength) {
          myDays -= monthLength;
        }
        else {
            break;
        }
    }
    //these are the updated class variables
    day = myDays; // day of month
    month = month;
    year = year;
}

