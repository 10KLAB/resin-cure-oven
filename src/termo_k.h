#include <Arduino.h>

#ifndef TERMO_K_H_
#define TERMO_K_H_

namespace _10klab{
    namespace termo_k{
        float ReadCelsius();
        void SetupPID();
        void SetPIDMode(bool mode);
        void ProfilePIDProcess();
        void setupPIDProfile(int preheat_sp, int preheat_tm, int heat_sp, int heat_tm );
    }
}

#endif