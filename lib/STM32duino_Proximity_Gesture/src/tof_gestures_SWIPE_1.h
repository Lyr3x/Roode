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
 * @file tof_gestures_SWIPE_1.h
 * $Date$
 * $Revision$
 */

#ifndef TOF_GESTURES_SWIPE_1_H_
#define TOF_GESTURES_SWIPE_1_H_

#include "tof_gestures_platform.h"
#include "ring_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif
 
/** @defgroup tof_gestures_SWIPE_1 Single SWIPE (1)
 *  @brief    Single Swipe gesture detection based on dynamic thresholds applied on range (single ToF device)
    @par Description
    This tof_gestures module allows to detect in a very robust way a single SWIPE (hand, finger, object) with a single ToF device. Returned
    gestures code are one of the following ones : ::GESTURES_HAND_ENTERING, ::GESTURES_HAND_LEAVING, ::GESTURES_SINGLE_SWIPE during swipe
    detection or ::GESTURES_NULL, ::GESTURES_DISCARDED, ::GESTURES_DISCARDED_TOO_SLOW in case SWIPE is not detected or discarded.
    @par Algorithm Description
    The approach consists in collecting __range__ samples over the time to get the background distance when no object is crossing the
    device FoV. A variation of the returned distance versus the background is detected as a hand/object entering in the FoV. A simple
    state machine is used to detect when the object will go out from the FoV. This module allows to detect any kind of swipes at all speeds
    (very low, medium and very fast) and with any tilts of the hand.
    @par Algorithm tuning
    A set of macros are defined in the module header file to tune the swipe detection.
    @par Known Limitations
    @li A gesture with hand leaving the FoV from the top direction (instead of left/right) may be detected as Swipe
    @li A Swipe may be wrongly detected when an object is placed at the border of the device FoV (this can be corected with some hysteresis filters)
 *  @ingroup tof_gestures
 *  @{  
 */

/** Buffer size used for averaging purpose */
#define SWIPE_1_BUFFER_SIZE 16
/** Background distance accepted variation (20%) */
#define SWIPE_1_BACKGROUND_VARIATION 20
/** Hand distance accepted variation (40%) */
#define SWIPE_1_HAND_VARIATION 40
/** State machine will reset if hand remains more than x msec in the FoV */
#define SWIPE_1_MAX_SWIPE_DURATION 1000
/** Max duration for distance to converge back to background when hand has left FoV */
#define SWIPE_1_BACKGROUND_TIMEOUT 100
/** Number of ranging distances to skip before starting gesture detection (0:max sensitivity) */
#define SWIPE_1_SENSITIVITY 	   0


/**
 * @struct Gesture_SWIPE_1_Data_t
 * @brief Data structure for single swipe gesture detection
 */
typedef struct {
	int gestureCode;                /*!< Output : Gesture intermediate code */
	ring_buffer_t rangeList;          /*!< Private : List of range samples */
    int meanBackground;				/*!< Private : Mean distance of the background */
    int meanHand;					/*!< Private : Mean distance of the detected hand/object */
    int timestamp;					/*!< Private : Used for internal timing */
    int cpt;						/*!< Private : Internal counter */
} Gesture_SWIPE_1_Data_t;    

/**
 * @brief Initialize gesture data
 * @return 0 on success
 */
int tof_gestures_initSWIPE_1(Gesture_SWIPE_1_Data_t *data);
/**
 * @brief Detect gesture
 * @return One of these gestures code from ::Gestures_Code_t : ::GESTURES_HAND_ENTERING, ::GESTURES_HAND_LEAVING, ::GESTURES_SINGLE_SWIPE or
 * ::GESTURES_NULL, ::GESTURES_DISCARDED, ::GESTURES_DISCARDED_TOO_SLOW
 * @warning This function must be called all the time, even if no valid range_mm is available from the ToF device.
 * In that case, call the function with the typical max ranging capability of the device
 *
 */
int tof_gestures_detectSWIPE_1(int32_t range_mm, Gesture_SWIPE_1_Data_t *data); 

 /** @}  */
#ifdef __cplusplus
}					
#endif	
#endif /* TOF_GESTURES_SWIPE_1_H_ */
