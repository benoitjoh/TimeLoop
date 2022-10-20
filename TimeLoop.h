// **************************************************************************
//
// A light framework to handle date and time without external hardware
// integrated in mainloop provides calender, date and timefunctions
//
//                                                        Johannes Benoit 2022
// ***************************************************************************

#ifndef TIMEHANDLER_H
#define TIMEHANDLER_H

#include "Arduino.h"


// leap year calculator expects year argument as year yyyy
#define LEAP_YEAR(Y)     ( ((Y)>0) && !((Y)%4) && ( ((Y)%100) || !((Y)%400) ) )


class TimeHandler
{
    public:
        TimeHandler(int dummy);

        void setSecondsTicker(long value);
        void setDayTicker(long value);
        //
        bool deciSecondPassed();
        //
        String getHrsMinSec();

        String getDowName();



    protected:
    private:

        // variables

        unsigned long lastDeciSecsIncMillis;
        int deciSecondsCounter;
        long secondsTicker;
        long dayTicker;
        int secs;
        int mins;
        int hrs;
        int day;
        int month;
        int year;
        int wDay;

};

#endif // TIMEHANDLER_H
