/*
* HackflightSimReceiver.h : Support USB controller for flight simulators
*
* Copyright (C) 2018 Simon D. Levy
*
* MIT License
*/

#pragma once

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>

#include <receiver.hpp>

#include "joystickapi.h"

static const uint16_t VENDOR_STM	        = 0x0483;

static const uint16_t PRODUCT_TARANIS		= 0x5710;
static const uint16_t PRODUCT_PS3_CLONE		= 0x0003;
static const uint16_t PRODUCT_XBOX360_CLONE	= 0xfafe;
static const uint16_t PRODUCT_EXTREMEPRO3D	= 0xc215;
static const uint16_t PRODUCT_F310	        = 0xc21d;
static const uint16_t PRODUCT_PS4	        = 0x09cc;

namespace hf {

    class SimReceiver : public Receiver {

        public:

            SimReceiver(void) 
            {
                _reversedVerticals = false;
                _springyThrottle = false;
                _useButtonForAux = false;
                _joyid = 0;
                _cycle = 0;

                _buttonState = 0;
            }

            void begin(void)
            {
                // Set up axes based on OS and controller
                productInit();
            }

            bool gotNewFrame(void)
            {
                return (++_cycle % 3) ? false : true;
            }

            void readRawvals(void)
            {
                static int32_t axes[6];
                static uint8_t buttons;

                // Grab the axis values in an OS-specific way
                productPoll(axes, buttons);

                // Display axes (helps debug new controllers)
                //hf::Debug::printf("0:%d  1:%d  2:%d 3:%d  4:%d  5:%d", axes[0], axes[1], axes[2], axes[3], axes[4], axes[5]);

                // Normalize the axes to demands in [-1,+1]
                for (uint8_t k=0; k<5; ++k) {
                    rawvals[k] = (axes[_axismap[k]] - productGetBaseline()) / 32767.f;
                }

                // Invert throttle, pitch if indicated
                if (_reversedVerticals) {
                    rawvals[0] = -rawvals[0];
                    rawvals[2] = -rawvals[2];
                }

                // For game controllers, use buttons to fake up values in a three-position aux switch
                if (_useButtonForAux) {
                    for (uint8_t k=0; k<3; ++k) {
                        if (buttons == _buttonmap[k]) {
                            _buttonState = k;
                        }
                    }
                    rawvals[4] = buttonsToAux[_buttonState];
                }
            }

            void halt(void)
            {
            }

        protected:

            virtual uint8_t getAux1State(void) override
            {
                return _springyThrottle ? 2 : Receiver::getAux1State();
            }

            virtual uint8_t getAux2State(void) override
            {
                return 1; // always armed!
            }

        private:

            // Implemented differently for each OS
            void     productInit(void);
            void     productPoll(int32_t axes[6], uint8_t & buttons);
            int32_t  productGetBaseline(void);

            // Determined dynamically based on controller
            bool     _reversedVerticals;
            bool     _springyThrottle;
            bool     _useButtonForAux;
            uint8_t  _axismap[5];   // Thr, Ael, Ele, Rud, Aux
            uint8_t  _buttonmap[3]; // Aux=0, Aux=1, Aux=2
            int      _joyid;        // Linux file descriptor or Windows joystick ID

            // Simulate auxiliary switch via pushbuttons
            uint8_t _buttonState;
            const float buttonsToAux[3] = {-.1f, 0.f, .8f};

            // Helps mock up periodic availability of new data frame (output data rate; ODR)
            uint64_t _cycle;          

    }; // class SimReceiver

} // namespace hf
