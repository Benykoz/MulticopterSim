/*
 * Test code for joystick on Linux
 *
 * Copyright (C) 2019 Simon D. Levy
 *
 * MIT License
 */

#include <Joystick.h>
#include <stdio.h>

int main(int argc, char ** argv)
{
    Joystick js;
    float axes[6] = {0};
    uint8_t buttons = 0;

    while (true) {

        if (!js.poll(axes, buttons)) {
            printf("%f %f %f %f %f %f\n", axes[0], axes[1], axes[2], axes[3], axes[4], axes[5]);
        }
    }

    return 0;
}


