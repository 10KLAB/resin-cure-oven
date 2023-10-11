#include <Arduino.h>
#ifndef PID_CONTROL_H_
#define PID_CONTROL_H_


namespace _10klab{
    namespace PIDControl{
        void SetPID();
        void SetPIDMode(bool mode);
        void ProfilePIDProcess();


    }
}

#endif