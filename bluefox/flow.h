/****************************************************************************
 *
 *   Copyright (C) 2013 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#ifndef FLOW_H_
#define FLOW_H_

#include <stdint.h>
#include <math.h>
#include <cmath>

using namespace std;

#define IMG_WIDTH   500
#define IMG_HEIGHT  500
#define BOTTOM_FLOW_FEATURE_THRESHOLD 30
#define BOTTOM_FLOW_VALUE_THRESHOLD	  5000
#define BOTTOM_FLOW_HIST_FILTER		  0
#define FOCAL_LENGTH_MM				  16
#define BOTTOM_FLOW_GYRO_COMPENSATION 1
#define GYRO_COMPENSATION_THRESHOLD   0.01

/**
 * @brief Computes pixel flow from image1 to image2
 */
uint8_t compute_flow(uint8_t *image1, uint8_t *image2, float x_rate, float y_rate, float z_rate,
		float *histflowx, float *histflowy, uint32_t dt);

inline uint32_t __UADD8(uint8_t* a, uint8_t* b)
{
	uint32_t ret;
	uint8_t* p_ret = (uint8_t*)&ret;
	p_ret[0] = *a + *b;
	p_ret[1] = *(a+1) + *(b+1);
	p_ret[2] = *(a+2) + *(b+2);
	p_ret[3] = *(a+3) + *(b+3);
	return ret;
}

inline uint32_t __USAD8(uint8_t* a, uint8_t* b)
{
	uint32_t ret = 0;
	ret += abs((int)*a - (int)*b);
	ret += abs((int)*(a+1) - (int)*(b+1));
	ret += abs((int)*(a+2) - (int)*(b+2));
	ret += abs((int)*(a+3) - (int)*(b+3));
	return ret;
}

inline uint32_t __USADA8(uint8_t* a, uint8_t* b, uint32_t c)
{
	return c + __USAD8(a, b);
}

inline uint32_t __UHADD8(uint8_t* a, uint8_t* b)
{
	uint32_t ret;
	uint8_t* p_ret = (uint8_t*)&ret;
	p_ret[0] = (*a + *b)/2;
	p_ret[1] = (*(a+1) + *(b+1))/2;
	p_ret[2] = (*(a+2) + *(b+2))/2;
	p_ret[3] = (*(a+3) + *(b+3))/2;
	return ret;
}

inline bool FLOAT_AS_BOOL(float a)
{
	return (a!=0.0f);
}
#endif /* FLOW_H_ */
