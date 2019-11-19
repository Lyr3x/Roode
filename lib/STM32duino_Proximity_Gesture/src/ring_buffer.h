/*******************************************************************************
Copyright © 2015, STMicroelectronics International N.V.
All rights reserved.

Use and Redistribution are permitted only in accordance with licensing terms 
available at www.st.com under software reference X-CUBE-6180XA1, and provided
that the following conditions are met:
* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
* Neither the name of STMicroelectronics nor the
names of its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROTECTED BY STMICROELECTRONICS PATENTS AND COPYRIGHTS.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS ARE DISCLAIMED.
IN NO EVENT SHALL STMICROELECTRONICS INTERNATIONAL N.V. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************************/
/*
 * @file ring_buffer.h
 * $Date: 2015-11-10 11:21:53 +0100 (Tue, 10 Nov 2015) $
 * $Revision: 2612 $
 */

#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

#include "tof_gestures_platform.h"

#ifdef __cplusplus
extern "C" {
#endif
    
/** @defgroup misc_ring_buffer Ring Buffer
 *  @brief    Simple ring buffer implementation
    @par Description
    Ring buffer is implemented as a static array of integers of size #RB_MAX_SIZE. The functional size of the ring buffer is
    programmable. When the ring_buffer is full, new elements added are replacing the older elements. This is typically used
    to keep an history of the last ranging values measured from the ToF device.
 *  @ingroup misc
 *  @{  
 */

/** Ring Buffer maximum size */
#define RB_MAX_SIZE 16

/**
 * @struct ring_buffer
 * @brief Simple ring buffer of int with a programmable size (max size is #RB_MAX_SIZE)
 */    
typedef struct
{
    int  buffer[RB_MAX_SIZE];
    int* buffer_end;            
    int* data_start;           
    int* data_end;
    int count;
    int size;
} ring_buffer_t;

/**
 * @brief Initialize Ring Buffer
 * @param rb  Ring Buffer pointer
 * @param size Number of int elements (max size is #RB_MAX_SIZE)
 * @return 0 on success or -1 if size is greater than #RB_MAX_SIZE
 */
int RB_init(ring_buffer_t* rb, int size);

/**
 * @brief Push one element in Ring Buffer (after the last element)
 * @par Function Description
 * If ring buffer is full, added element is replacing the oldest element in the ring buffer
 * @param rb  Ring Buffer pointer
 * @param data Element to add
 * @return 0 on success
 */
int RB_push(ring_buffer_t* rb, int data);

/**
 * @brief pop one element in Ring Buffer (the last element)
 * @param rb  Ring Buffer pointer
 * @return element
 */
int RB_pop(ring_buffer_t* rb);

/**
 * @brief Check if ring buffer is full
 * @param rb  Ring Buffer pointer
 * @return true if full else false
 */
bool RB_full(ring_buffer_t* rb);
 
/**
 * @brief print/trace all elements in the ring buffer
 * @param rb  Ring Buffer pointer
 * @note The TRACE key must be defined in the project
 */
void RB_trace(ring_buffer_t*rb);

/**
 * @brief Return the sum of elements in the ring buffer
 * @param rb  Ring Buffer pointer
 * @return The sum
 */
int RB_sum(ring_buffer_t*rb);

/**
 * @brief Return the mean of all elements in the ring buffer
 * @param rb  Ring Buffer pointer
 * @return The mean (rounded to integer)
 */
int RB_mean(ring_buffer_t*rb);

/**
 * @brief Return the mean of the absolute differences of each element with the mean
 * @param rb  Ring Buffer pointer
 * @return The mad (rounded to integer)
 */
int RB_mad(ring_buffer_t*rb);

/**
 * @brief Return the direction of the curve of points stored in the buffer
 * @param rb  Ring Buffer pointer
 * @return 1 if constantly increase, -1 if constantly decrease, 0 otherwise
 */
int RB_dir(ring_buffer_t*rb);

 /** @}  */

#ifdef __cplusplus
}					
#endif	
#endif /* RING_BUFFER_H_ */
