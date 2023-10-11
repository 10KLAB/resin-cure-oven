#include <Arduino.h>
#ifndef SCREEN_CONTROL_H_
#define SCREEN_CONTROL_H_


namespace _10klab{
    namespace screen{
        void SetUpScreen();
        void ShowLineOne(String text);
        void ShowLineTwo(String text);
        void drawGraph(float value, int setpoint);


    }
}

#endif