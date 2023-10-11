#include <Arduino.h>
#ifndef DIGITAL_IO_H_
#define DIGITAL_IO_H_

#define BACK 15
#define NEXT 4
#define SELECT 18
#define ALARM 14

namespace _10klab{
    namespace digitalIO{
        void setupIO();
        bool readButton(int button);
        void selectPIDParameters();
        void turnOnAlarm(bool one_time);

    }
}

#endif