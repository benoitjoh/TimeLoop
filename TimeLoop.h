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

        int getDow(int increment);
        String getDowName(int increment);

        int getMonth(int increment);
        String getMonthName(int increment);

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
        unsigned long lastSecsIncMillis;
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
