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

#include "tof_motion.h"

int tof_initMotion(int threshold, MotionData_t *data){
    data->threshold = threshold;
    data->firstTime = true;
    data->duration = 0;
    data->previousRange = 0;
    data->timestamp = 0;
    return 0;
}
    
    
int tof_getMotion(int32_t range_mm, MotionData_t *data){
    int return_code;
    unsigned long timestamp = GET_TIME_STAMP();
    bool belowThreshold, isContinuousMotion;
    
    if(data->firstTime)
    {
        data->firstTime = false;
        data->timestamp = timestamp;
        data->previousRange = range_mm;
        belowThreshold = (range_mm < data->threshold);
        isContinuousMotion = true;
        return_code = ( belowThreshold ) ? GESTURES_MOTION_DOWN_STATE : GESTURES_MOTION_UP_STATE;
        data->duration = 1;
        return return_code;
    }
    
    data->duration = 0;
	return_code = GESTURES_MOTION_NULL;
	belowThreshold  = (range_mm < data->threshold);
	isContinuousMotion  = ( belowThreshold ==  (data->previousRange < data->threshold));
	data->previousRange = range_mm;
    
    //TOF_GESTURES_DEBUG(MOTION,"range=%d, belowThreshold=%d, isContinuousMotion=%d", range_mm, belowThreshold,isContinuousMotion);

	// update
	data->duration = (long)(timestamp - data->timestamp);
	if(isContinuousMotion)
	{
		return_code = ( belowThreshold ) ? GESTURES_MOTION_DOWN_STATE : GESTURES_MOTION_UP_STATE;
	}
	else
	{
		return_code = ( belowThreshold ) ? GESTURES_MOTION_DROP_DOWN : GESTURES_MOTION_RAISE_UP;
		data->timestamp = timestamp;
	}
	return return_code;
}
