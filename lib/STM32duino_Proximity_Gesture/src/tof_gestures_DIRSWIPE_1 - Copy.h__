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
 * @file tof_gestures_DIRSWIPE_1.h
 * $Date: 2015-11-10 11:21:53 +0100 (Tue, 10 Nov 2015) $
 * $Revision: 2612 $
 */

#ifndef TOF_GESTURES_DIRSWIPE_1_H_
#define TOF_GESTURES_DIRSWIPE_1_H_

#include "tof_gestures_platform.h"
#include "tof_motion.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup tof_gestures_DIRSWIPE_1 Directional SWIPE (1)
 *  @brief    Directional (left/right) Swipe gesture detection based on motion detector (dual ToF devices)
    @par Description
    This tof_gestures module allows to detect a directional swipe (from left to right or from right to left) using
    two ToF devices (left and right). It is based on the tof_motion module with simple threshold and timing considerations.
    It returns one of the following gestures codes : ::GESTURES_SWIPE_LEFT_RIGHT, ::GESTURES_SWIPE_RIGHT_LEFT when gesture is detected
    or ::GESTURES_NULL, ::GESTURES_DISCARDED_TOO_SLOW or ::GESTURES_DISCARDED_TOO_FAST when no gesture is detected.
    @par Known Limitations
    None (very fast swipes can be detected)
 *  @ingroup tof_gestures
 *  @{  
 */

    
/** Codes used to detect directional swipes (internal state machine)
 */
enum Gestures_DirSwipe_Code_t{
    GESTURES_DIRSWIPE_START   = 0, /*!< Ready to detect gesture start */
    GESTURES_DIRSWIPE_END     = 1, /*!< Ready to detect gesture end */
};
    
/**
 * @struct Gesture_DIRSWIPE_1_Data_t
 * @brief Data structure for directional swipes detection (using two ranging devices)
 */ 
typedef struct {
    long minSwipeDuration;                     /*!< Input : Minimum duration of a swipe to be detected */  
    long maxSwipeDuration;                     /*!< Input : Maximum duration of a swipe to be detected */
    MotionData_t motionDetectorLeft;          /*!< Private : Motion detector associated to the left ranging device */
    MotionData_t motionDetectorRight;         /*!< Private : Motion detector associated to the right ranging device */
    int state;                                /*!< Private : Internal state machine */  
    int gesture_start_from_right;             /*!< Private : Obvious */   
    unsigned long timestamp;                  /*!< Private : Obvious */
} Gesture_DIRSWIPE_1_Data_t;

/**
 * @brief Initialize gesture data
 * @return 0 on success
 */
int tof_gestures_initDIRSWIPE_1(int32_t threshold, long minSipeDuration, long maxSwipeDuration, Gesture_DIRSWIPE_1_Data_t *data);
/**
 * @brief Detect gesture
 * @return One of these gestures code from ::Gestures_Code_t : ::GESTURES_SWIPE_LEFT_RIGHT, ::GESTURES_SWIPE_RIGHT_LEFT, ::GESTURES_NULL, ::GESTURES_DISCARDED_TOO_SLOW or ::GESTURES_DISCARDED_TOO_FAST  
 */
int tof_gestures_detectDIRSWIPE_1(int32_t left_range_mm, int32_t right_range_mm, Gesture_DIRSWIPE_1_Data_t *data); 

 /** @}  */

#ifdef __cplusplus
}					
#endif	
#endif /* TOF_GESTURES_DIRSWIPE_1_H_ */
