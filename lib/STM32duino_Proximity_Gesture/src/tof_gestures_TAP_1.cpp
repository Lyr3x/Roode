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
 * $Date$
 * $Revision$
 */

#include "tof_gestures.h"
#include "tof_gestures_TAP_1.h"

int tof_gestures_initTAP_1(Gesture_TAP_1_Data_t *data){
    int status=0;
    status |= (int)(RB_init(&(data->rangeList),TAP_1_BUFFER_SIZE));
    data->timestampLastDetectedTap = 0;
    data->nbOfDataToDecide = TAP_1_NB_OF_DATA_TO_DECIDE;
    data->meanBackground = 0;
    data->gestureCode = GESTURES_NULL;
    data->timestamp = 0;
    return status;
}

int tof_gestures_detectTAP_1(int32_t range_mm, Gesture_TAP_1_Data_t *data){
    int time = GET_TIME_STAMP();
    int madRange = 0;
    int dir;
    int delta_time = 0;
    int background_threshold = 0;

    // Default state
	if (data->gestureCode == GESTURES_NULL){
		// Calculate background distance (averaging)
		data->meanBackground = (data->rangeList.count==0) ? 0 : RB_mean(&(data->rangeList));
		background_threshold = (((100-TAP_1_BACKGROUND_VARIATION) * data->meanBackground) / 100);
		// Check current distance vs background distance
		if (range_mm < background_threshold){
			// big variation detected => hand entering in FoV
			data->gestureCode = GESTURES_HAND_ENTERING;
			RB_init(&(data->rangeList),TAP_1_BUFFER_SIZE);
			RB_push(&(data->rangeList), range_mm);
			data->timestamp = time;
			//TOF_GESTURES_DEBUG(TAP_1,"(background_thd=%d, current=%d, hand_entering=1)", background_threshold, range_mm);
		}else{
    		// no variation => keep averaging background
			RB_push(&(data->rangeList), range_mm);
    		//TOF_GESTURES_DEBUG(TAP_1,"(background_thd=%d, current=%d, hand_entering=0)", background_threshold, range_mm);
    	}


	// Hand entering
	} else if (data->gestureCode == GESTURES_HAND_ENTERING){
		// Push the range value in the buffer
		RB_push(&(data->rangeList), range_mm);

		// Try to detect a Tap gesture as soon as possible (4 data are enough for normal/fast gestures speed)
		if(data->rangeList.count == data->nbOfDataToDecide){
			// Calculate MAD
			madRange = RB_mad(&(data->rangeList));
			dir = RB_dir(&(data->rangeList));
			delta_time = time - data->timestampLastDetectedTap;
			// Try to detect a TAP
			//  MAD is higher than a threshold, ranging elements stored in the buffer are decreasing and last TAP has been
			//  detected more than 250 ms ago (too avoid 2 TAPS being reported in case of a slow tap)
			if((madRange > 12) && (dir<0) && (delta_time>250)){
				// Big negative range variation => TAP
				data->timestampLastDetectedTap = time;
				RB_trace(&(data->rangeList));
				RB_init(&(data->rangeList),16);
				data->gestureCode = GESTURES_SINGLE_TAP;
				//TOF_GESTURES_DEBUG(TAP_1,"(madRange=%d, delta_time=%d, dir=%d) => TAP", madRange, delta_time, dir);

			}else{
				// No TAP detected
				RB_init(&(data->rangeList),16);
				data->gestureCode = GESTURES_NULL;
				//TOF_GESTURES_DEBUG(TAP_1,"(madRange=%d, delta_time=%d, dir=%d) => no TAP", madRange, delta_time, dir);
			}

			//TOF_GESTURES_DEBUG(TAP_1,"(hand_entering=1, current=%d, decide=1, madRange=%d, dir=%d, tap=%d)", range_mm, madRange, dir, (int)(data->gestureCode == GESTURES_SINGLE_TAP));

		}
		//TOF_GESTURES_DEBUG(TAP_1,"(hand_entering=1, current=%d, decide=0, tap=0)", range_mm, (int)(data->rangeList.count == data->nbOfDataToDecide));

	// Tap detected
	} else {
		// Reset gesture code state machine
		data->gestureCode = GESTURES_NULL;
	}

	 return data->gestureCode;
}

