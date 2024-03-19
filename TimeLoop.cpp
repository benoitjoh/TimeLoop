#include "TimeLoop.h"

#define YEAR_OFFSET 2020 // 1.1.2020 is represented by 1 in the daysCounter


/** @brief Constants with the day and month names
  *
  */

static const String dow[7] = { "Mo", "Di", "Mi", "Do", "Fr", "Sa", "So" };
static const String monthNames[13]={"", "Jan", "Feb", "M\xe1r", "Apr", "Mai", "Jun", "Jul", "Aug" , "Sep" , "Okt" , "Nov" , "Dez" };

// --------  helper -----------------------------------------------------------------

/** @brief leap year calculator expects year argument as year yyyy
  *
  * @param year: a year number as yyyy
  */
#define LEAP_YEAR(Y)     ( ((Y)>0) && !((Y)%4) && ( ((Y)%100) || !((Y)%400) ) )



/** @brief Method to calculate the days in a month in a given year
  *
  *
  */

//                               Dec Jan Feb Mar Apr...                          Dec
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



/** @brief helper that makes numbers strings 2 digits with leading zeros
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
TimeLoop::TimeLoop(byte dummy) {
    lastDeciSecsIncMillis = 0;
    lastSecsIncMillis = 0;
    correctionLocked = false;
    // note: the seconds and days- Counter have to be set after
    // construction!
}


/** @brief Sets a correction factor for clock
    Parameter millisPerHourCorr depends from how precise the internal clock is.
    if clock is to slow you can speed it up by +/- x, wich add x ms at the beginning of
    a new hour
    value between -900 and 900 which means about +/- 21,6 sec per day
  */
void TimeLoop::setMsPerHourCorrection(int millisPerHourCorr) {
    msPerHourCorrection = millisPerHourCorr;
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


/** @brief called in a mainloop, it keeps the timer running
  *
  * this method is the actualization event called in the loop of the controller
  *  /!\ it must be called at least each 100 milliseconds. otherwise the timer
  *      looses track.
  *
  * @result     statusvalue what happened:
  *         if 100ms has passed,       --> return 1
  *         if 1 second has passed      --> return 2
  *         if a day finished (midnight) --> returns 3
  */
byte TimeLoop::actualize() {
    unsigned long now = millis();
    if ( now - lastDeciSecsIncMillis >= 100 ) {
        lastDeciSecsIncMillis += 100;
        int delta = now - lastSecsIncMillis;
        if (delta >= 1000) {
        //Serial.println("sec: " + String(secondsCounter) +" last: " + String(lastSecsIncMillis) + " now: " + String(now) + "delta: " + String(delta));
            lastSecsIncMillis += 1000;
            if (incrementSecondsCounter(1)) {
                return 3;
                }
            return 2;
            }
        return 1;
        }
    return 0;
    }



/** @brief adds a value to secondsCounter.
  *
  * if overflow it also increments or decrements the daycounter and returns true
  *
  * if sec==0 which means new hour begins, the msPerHourCorrection is applied
  *
  * @param  increment: positive or negativ integer
  * @result True if day was changed (step over midnight)
  */
bool TimeLoop::incrementSecondsCounter(long increment) {
    secondsCounter += increment;
    // full hour, apply corrections
    if ( (secondsCounter % 3600) == 0) {
        if (correctionLocked == false) {
            // add seconds to counter, and substract the millisecs from the time where last incremented
            int secDelta = msPerHourCorrection / 1000;
            int msDelta = msPerHourCorrection % 1000;
            secondsCounter += secDelta;
            //Serial.println("new hour. correct by: " + String(secDelta) + "s, " + String(msDelta) + "ms");
            lastSecsIncMillis -= msDelta;
            // lock this correction part for 5 seconds
            correctionLocked = true;
            }
        }
        
    if ( (secondsCounter % 3600) == 5) { 
        // unlock correction
        correctionLocked = false; 
        }

    // increment / decrement and overflow    
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


/* methods for seting the date */

void TimeLoop::incrementDayCounter(int increment) {
    dayCounter = dayCounter + increment;
    breakupDayCounter();
}


void TimeLoop::incrementMonth() {
    dayCounter += getMonthLength(month, year);
    breakupDayCounter();
}


void TimeLoop::decrementMonth() {
    if (dayCounter > getMonthLength(month - 1, year)) {
        dayCounter -= getMonthLength(month - 1, year);
        breakupDayCounter();
    }
}


void TimeLoop::incrementYear() {
    dayCounter += (LEAP_YEAR(year) ? 366 : 365);
    breakupDayCounter();
}


void TimeLoop::decrementYear() {
    if (dayCounter > (LEAP_YEAR(year - 1) ? 366 : 365)) {
        dayCounter -= (LEAP_YEAR(year - 1) ? 366 : 365);
        breakupDayCounter();
    }
}

/** @brief set time to the beginning of the actual minute
  *
  */
void TimeLoop::resetSecToZero() {
    secs = secondsCounter % 60;
    secondsCounter = secondsCounter - secs;
    secs = 0;
    return;
}

/* methods for reading dates and time*/


/** @brief get time as a string in 24h format
  *
  * @result     string hh:mm:ss
  *
  */
String TimeLoop::getHrsMinSec() {
    secs = secondsCounter % 60;
    unsigned int remains = secondsCounter / 60;
    mins = remains % 60;
    hrs = remains / 60;
    return lFill(String(hrs)) + ":" + lFill(String(mins)) + ":" +  lFill(String(secs));
}

/** @brief get seconds as string
  *
  * @result     string "ss"
  *
  */
String TimeLoop::getSec() {
    secs = secondsCounter % 60;
    return lFill(String(secs));
}


/** @brief get date as a string in EU format
  *
  * @result      a string dd.mm.yyyy
  *
  */
String TimeLoop::getDayMonYear() {

    return lFill(String(day)) + "." + lFill(String(month)) + "." + String(year);
}


/** @brief returns the number of weekday.
  *        0 = monday
  *        with offset =-1 you can get the dow for yesterday
  *
  * @param      offset: zero, positive or negativ integer
  * @result     integer, day of week
  */
int TimeLoop::getDayOfWeek(int offset){
    wDay = ((dayCounter + 1) % 7);  // Monday is day 0
    wDay += offset;
    if (wDay > 6) { wDay -= 7; }
    if (wDay < 0) { wDay += 7; }
    return wDay;
}


/** @brief returns the shortname of the week day.
  *        with offset =-1 you can get the dow for yesterday
  *
  * @param      offset: zero, positive or negativ integer
  * @result     string (length of 2), short name of week day
  */
String TimeLoop::getDayOfWeekName(int offset) {
    return dow[getDayOfWeek(offset)];
}


/** @brief returns the number of month.
  *        1 = january
  *        with offset =-1 you can get the number of the last month
  *
  * @param      offset: zero, positive or negativ integer
  * @result     integer, number of month
  */
int TimeLoop::getMonth(int offset) {
    int myMonth = month + offset;
    if (myMonth > 12) { myMonth -= 12; }
    if (myMonth < 1) { myMonth += 12; }
    return myMonth;
}


/** @brief returns the shortname of month. (Jan, Feb ...)
  *        with offset =-1 you can get the name of the last month
  *
  * @param      offset: zero, positive or negativ integer
  * @result     string (length of 3), short name of week day
  */
String TimeLoop::getMonthName(int offset) {
    return monthNames[getMonth(offset)];
}


/** @brief returns the number of day of the year
  *        with offset =-1 you can get the value vor yesterday...
  *
  * @param      offset: zero, positive or negativ integer
  * @result     integer, number of day in year
  */
int TimeLoop::getDayOfYear(int offset) {
    int myDay = dayOfYear - offset;
    if (myDay < 0) {
        myDay += (LEAP_YEAR(year-1) ? 366 : 365);
    }
    return dayOfYear;
}


/* method for calculation and transforming */



/** @brief splits dayCounter in day month year
  *
  * break the given dayCounter counting from 1.1.2020 into day / month / year
  * it updates the class variables day, month, year
  *
  * (derived from C library localtime function)
  *
  * @param  None
  * @result None
*/
void TimeLoop::breakupDayCounter(){
    // note that dayCounter has offset from year 2020

    // calculate the year. Increment from year to year until we step over dayCounter
    year = YEAR_OFFSET;
    unsigned long myDays = 0;
    while((unsigned)(myDays += (LEAP_YEAR(year) ? 366 : 365)) < dayCounter) {
        year++;
    }

    myDays -= LEAP_YEAR(year) ? 366 : 365; // one step back to the 1.Jan of the year
    //Serial.println("year:" + String(year) + "myDays:" + String(myDays));

    myDays = dayCounter - myDays; // day of year

    // update class variable for dayOfYear
    dayOfYear = myDays;

    // calculate month
    for (month = 1; month < 13; month++) {
        byte monthLength = getMonthLength(month, year);

        if (myDays > monthLength) {
          myDays -= monthLength;
        }
        else {
            break;
        }
    }
    // update the class variables:
    day = myDays; // day of month
    month = month;
    year = year;
}

