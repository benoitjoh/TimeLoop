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
        TimeLoop(byte dummy);
        void setMsPerHourCorrection(int millisPerHourCorr);

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
        String getSec();

        int getDayOfWeek(int offset);
        String getDayOfWeekName(int offset);

        int getDayOfYear(int offset);

        int getMonth(int offset);
        String getMonthName(int offset);

        void incrementMonth();
        void incrementYear() ;
        void decrementMonth();
        void decrementYear();

        void resetSecToZero();

    protected:
    private:
        //methods
        void breakupDayCounter();

        // variables
        int msPerHourCorrection;
        bool correctionLocked;


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
        int dayOfYear;

};

#endif // TimeLoop_H
