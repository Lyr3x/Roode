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
#include "tof_gestures_SWIPE_1.h"

int tof_gestures_initSWIPE_1(Gesture_SWIPE_1_Data_t *data){
    int status=0;
    status |= (int)(RB_init(&(data->rangeList),SWIPE_1_BUFFER_SIZE));
    data->meanBackground = 0;
    data->meanHand = 0;
    data->gestureCode = GESTURES_NULL;
    data->cpt = SWIPE_1_SENSITIVITY;
    return status;
}

int tof_gestures_detectSWIPE_1(int32_t range_mm, Gesture_SWIPE_1_Data_t *data){
    int time = GET_TIME_STAMP();

    /* Default state */
    if (data->gestureCode == GESTURES_NULL){
    	// Calculate background distance (averaging)
    	data->meanBackground = (data->rangeList.count==0) ? 0 : RB_mean(&(data->rangeList));
    	// Check current distance vs background distance
    	if (range_mm < (((100-SWIPE_1_BACKGROUND_VARIATION) * data->meanBackground) / 100)){
    		// big variation detected => hand entering in FoV
    		if (data->cpt == 0) {
    			// Potential spikes (duster ?) skipped
    			data->cpt = SWIPE_1_SENSITIVITY;
				data->gestureCode = GESTURES_HAND_ENTERING;
				RB_init(&(data->rangeList),SWIPE_1_BUFFER_SIZE);
				RB_push(&(data->rangeList), range_mm);
				data->timestamp = time;
				//TOF_GESTURES_DEBUG(SWIPE_1,"(background=%d, current=%d) => HAND_ENTERING", data->meanBackground, range_mm);
    		} else {
    			data->cpt--;
    			//TOF_GESTURES_DEBUG(SWIPE_1,"(background=%d, current=%d) => SKIP", data->meanBackground, range_mm);
    		}
    	}else{
    		// no variation => keep averaging background
    		RB_push(&(data->rangeList), range_mm);
    	}

    /* Hand entering */
    } else if (data->gestureCode == GESTURES_HAND_ENTERING){
    	// Calculate hand distance (averaging)
    	data->meanHand = (data->rangeList.count==0) ? 0 : RB_mean(&(data->rangeList));
    	// Check current distance vs hand mean distance
    	if ((range_mm < ((data->meanHand * (100-SWIPE_1_HAND_VARIATION)) / 100)) || ((((data->meanHand * (100+SWIPE_1_HAND_VARIATION)) / 100) < range_mm))){
    		// Big variation => Hand leaving
			data->gestureCode = GESTURES_HAND_LEAVING;
			RB_init(&(data->rangeList),SWIPE_1_BUFFER_SIZE);
			RB_push(&(data->rangeList), range_mm);
			data->timestamp = time;
			//TOF_GESTURES_DEBUG(SWIPE_1,"(meanHand=%d, current=%d) => HAND_LEAVING", data->meanHand, range_mm);
    	} else if ((time - data->timestamp) > SWIPE_1_MAX_SWIPE_DURATION) {
    		// Too slow => discard (swipe is too slow, this may be a background change for a long time)
			data->gestureCode = GESTURES_DISCARDED_TOO_SLOW;
			RB_init(&(data->rangeList),SWIPE_1_BUFFER_SIZE);
			//TOF_GESTURES_DEBUG(SWIPE_1,"DISCARDED_TOO_SLOW");
    	} else {
    		// No big variation => remains in this state
			RB_push(&(data->rangeList), range_mm);
			//TOF_GESTURES_DEBUG(SWIPE_1, "Wait (meanHand=%d, current=%d)", data->meanHand, range_mm);
    	}

    /* Hand leaving */
    } else if (data->gestureCode == GESTURES_HAND_LEAVING){
    	// Check current distance vs background distance (previously calculated)
    	if ((((data->meanBackground * (100-SWIPE_1_BACKGROUND_VARIATION)) / 100) < range_mm)){
    		// Almost same => Swipe detected/finished
    		data->gestureCode = GESTURES_SINGLE_SWIPE;
    		//data->cpt = 0;
    		RB_init(&(data->rangeList),SWIPE_1_BUFFER_SIZE);
    		//return_code = GESTURES_SINGLE_SWIPE;
    		//TOF_GESTURES_DEBUG(SWIPE_1, "(background=%d, current==%d) => SINGLE_SWIPE\n", data->meanBackground, range_mm);
    		//uart_printf("2 : Next state : %d, (background=%d), range=%d (SWIPE)\n", data->gestureCode, data->meanBackground, range_mm);
    	} else if ((time - data->timestamp) > SWIPE_1_BACKGROUND_TIMEOUT){
    		// To slow to converge back to background distance => discard (background may have change during the gesture)
    		data->gestureCode = GESTURES_DISCARDED;
			RB_init(&(data->rangeList),SWIPE_1_BUFFER_SIZE);
			//TOF_GESTURES_DEBUG(SWIPE_1,"DISCARDED");
    	} else {
    		// Wait for convergence towards background
    		//TOF_GESTURES_DEBUG(SWIPE_1,"Wait (background=%d, current=%d)", data->meanBackground, range_mm);
    	}

    /* Swipe detected or discarded */
    } else {
    	data->gestureCode = GESTURES_NULL;
    }

    return data->gestureCode;
}
