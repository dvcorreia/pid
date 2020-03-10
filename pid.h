// Copyright 2020 Diogo Correia.
//
// Author: Diogo Correia (dv.correia@ua.com, dv_correia@hotmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY TiND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// See http://creativecommons.org/licenses/MIT/ for more information.
//
// -----------------------------------------------------------------------------
//
// PID controll routines optimized for microcontrollers:
// It used discrete PID controll algorithm and aritmetic siplifications that
// are optimized for microcontrollers without floating point hardware support 

#ifndef PID_DISCRETE_H_
#define PID_DISCRETE_H_

namespace pidlib {
    template <class T>
    class Pid{
        private:
            T Kp, Td, Ti;           // PID tunning parameters
            T DP;                   // Decimal pointer slider
            T f;                    // Sampling frequency
            T a0, a1, a2;           // Gains
            T u[2], e[3];           // Control and Error signals
            T uppercap, lowercap;   // Upper and Lower controll signal cap
        public:
            Pid();
            ~Pid(); 

            // Init PID controller:
            // - set sampling frequency
            // - decimal cases to keep
            void init(T frequency, T DP){
                f = frequency;
                DP = DP;
                e = T();
                u = T();
            }

            // Set PID tunning parameters 
            void tune(T Kp, T Td, T Ti){
                Kp = Kp;
                Td = Td;
                Ti = Ti;
            }

            // Set PID output limits
            void limit(T uppercap, T lowercap){
                uppercap = uppercap;
                lowercap = lowercap;
            }

            T run(const T& error){
                // Calculate gains
                a0 = Kp * (DP + DP * DP / (2 * Ti * f) + Td * f) / DP;
                a1 = Kp * (DP * DP / (2 * Ti * f) - 2 * Td * f - DP) / DP;
                a2 = Kp * Td * f / DP;

                // Calculate controll signal
                u[0] = u[1] + (a0 * e[0] + a1 * e[1] + a2 * e[2]) / DP;

                // Update Controll and Error signals
                u[1] = u[0];
                e[2] = e[1];
                e[1] = e[0];

                return u[0]; 
            }
    };
}


#endif

/* PID Parameters (x100)
const uint16_t Kpx = 100, Tdx = 10, Tix = 10;
const uint16_t Kpy = 100, Tdy = 10, Tiy = 10;

uint32_t a0x, a1x, a2x;
uint32_t a0y, a1y, a2y;

int32_t ex[3] = {0}, ey[3] = {0};
int32_t ux[2] = {50}, uy[2] = {50};

uint32_t posx = 0, posy = 0;
uint32_t posx2 = 0;

// PID Coeficients Calculation (x100)
a0x = Kpx * (DP + DP * DP / (2 * Tix * f) + Tdx * f) / DP;
a1x = Kpx * (DP * DP / (2 * Tix * f) - 2 * Tdx * f - DP) / DP;
a2x = Kpx * Tdx * f / DP;

printf("\r\na0x = %d | a1x = %d | a2x = %d", a0x, a1x, a2x);

a0y = Kpy * (DP + DP * DP / (2 * Tiy * f) + Tdy * f) / DP;
a1y = Kpy * (DP * DP / (2 * Tiy * f) - 2 * Tdy * f - DP) / DP;
a2y = Kpy * Tdy * f / DP;

printf("\r\na0y = %d | a1y = %d | a2y = %d", a0y, a1y, a2y);

ey[0] = setPointy - posy;
uy[0] = uy[1] + (a0y * ey[0] + a1y * ey[1] + a2y * ey[2]) / DP;

if (uy[0] > ymaxpwm)
    uy[0] = ymaxpwm;
if (uy[0] < 0)
    uy[0] = 0;

setPWM('y', uy[0]);*/
