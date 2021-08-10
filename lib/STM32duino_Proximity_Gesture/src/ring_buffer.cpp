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
 * $Date: 2015-11-10 11:21:53 +0100 (Tue, 10 Nov 2015) $
 * $Revision: 2612 $
 */
 
 
#include "ring_buffer.h"

#define ABS(a) ((a>=0) ? (a) : -(a))

int RB_init(ring_buffer_t* rb, int size)
{
    rb->buffer_end = rb->buffer + ((size<RB_MAX_SIZE) ? size : RB_MAX_SIZE);
    rb->size = size;
    rb->data_start = rb->buffer;
    rb->data_end = rb->buffer;
    rb->count = 0;
    return (size<=RB_MAX_SIZE) ? 0 : -1;
}

int RB_push(ring_buffer_t* rb, int data)
{
    if (rb == NULL || rb->buffer == NULL)
        return -1;

    *rb->data_end = data;
    rb->data_end++;
    if (rb->data_end == rb->buffer_end)
        rb->data_end = rb->buffer;

    if (RB_full(rb)) {
        if ((rb->data_start + 1) == rb->buffer_end)
            rb->data_start = rb->buffer;
        else
            rb->data_start++;
    } else {
        rb->count++;
    }

    return 0;
}

int RB_pop(ring_buffer_t* rb)
{
    if (rb == NULL || rb->buffer == NULL)
        return false;

    int8_t data = *rb->data_start;
    rb->data_start++;
    if (rb->data_start == rb->buffer_end)
        rb->data_start = rb->buffer;
    rb->count--;

    return data;
}

bool RB_full(ring_buffer_t* rb)
{
    return rb->count == rb->size;
}

void RB_trace(ring_buffer_t*rb)
{
    int i=0;
    int *ptr;
    
    ptr = rb->data_start;
    //trace_printf("TOF_GESTURES Ring Buffer : ");
    for(i=0;i<rb->count;i++)
    {
        //trace_printf("%d,",*ptr++);
        if(ptr == rb->buffer_end)
            ptr = rb->buffer;
    }
    //trace_printf("\n");
}

int RB_sum(ring_buffer_t*rb)
{
    int i=0;
    int sum=0;
    int *ptr;
    
    ptr = rb->data_start;
    for(i=0;i<rb->count;i++)
    {
        sum += *ptr++;
        if(ptr == rb->buffer_end)
            ptr = rb->buffer;
    }
    return sum;
}

int RB_mean(ring_buffer_t*rb)
{
    return RB_sum(rb)/rb->count;
}

int RB_mad(ring_buffer_t*rb)
{
    int i;
    int *ptr;
    int mad=0;
    int mean;
    int data;
    
    mean = RB_mean(rb);
       
    ptr = rb->data_start;
    for(i=0;i<rb->count;i++)
    {
        data = *ptr++;
        mad += ABS((data - mean));
        if(ptr == rb->buffer_end)
            ptr = rb->buffer;
    }
    return mad/rb->count;
}

int RB_dir(ring_buffer_t*rb)
{
    int direction = 0;   
    int i=0;
    int *ptr;
    int value1, value2;
    
    ptr = rb->data_start;
    if (rb->count == 0 || rb->count == 1){
        return 0;
    } else {
        value1 = *ptr++; if(ptr == rb->buffer_end) ptr = rb->buffer;
        value2 = *ptr++; if(ptr == rb->buffer_end) ptr = rb->buffer;
        direction = ((value2 - value1) > 0) ? 1 : (((value2 -value1) < 0) ? -1 : 0);
        value1 = value2;
    }
    
    for(i=2;i<rb->count;i++)
    {
        value2 = *ptr++; if(ptr == rb->buffer_end) ptr = rb->buffer;
        if ((direction==1) && ((value2-value1)<0)) {
            direction = 0;
            break;
        }
        if ((direction==-1) && ((value2-value1)>0)) {
            direction = 0;
            break;
        }
        if (direction==0) {
            direction = ((value2 - value1) > 0) ? 1 : (((value2 -value1) < 0) ? -1 : 0);
        }
        value1 = value2;
    }
    return direction;
}
