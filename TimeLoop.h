// **************************************************************************
//
// A light framework to handle date and time without external hardware
// integrated in mainloop provides calender, date and timefunctions
//
//                                                        Johannes Benoit 2022
// ***************************************************************************

#ifndef TimeLoop_H
#define TimeLoop_H

#include "Arduino.h"


// leap year calculator expects year argument as year yyyy
#define LEAP_YEAR(Y)     ( ((Y)>0) && !((Y)%4) && ( ((Y)%100) || !((Y)%400) ) )

byte getMonthLength(int myMonth, int myYear);

class TimeLoop
{
    public:
        TimeLoop(int dummy);

        void setSecondsCounter(long value);
        void setDayCounter(long value);
        bool incrementSecondsCounter(long increment);
        void incrementDayCounter(int increment);
        long getSecondsCounter();
        long getDayCounter();

        //
        byte actualize();

        //
        String getHrsMinSec();
        String getDayMonYear();

        byte getDow(int increment);
        String getDowName();

        byte getMonth(int increment);
        String getMonthName();

        void incrementMonth();
        void incrementYear() ;
        void decrementMonth();
        void decrementYear();

    protected:
    private:
        //methods
        void breakupDayCounter();

        // variables
        unsigned long lastDeciSecsIncMillis;
        int deciSecondsCounter;
        long secondsCounter;
        long dayCounter;

        int secs;
        int mins;
        int hrs;
        int day;
        int month;
        int year;
        int wDay;

};

#endif // TimeLoop_H
