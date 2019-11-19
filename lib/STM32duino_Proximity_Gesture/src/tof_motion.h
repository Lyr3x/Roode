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
 * @file tof_motion.h
 * $Date: 2015-11-10 11:21:53 +0100 (Tue, 10 Nov 2015) $
 * $Revision: 2612 $
 */

#ifndef TOF_MOTION_H_
#define TOF_MOTION_H_

#include "tof_gestures_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup tof_motion Motion
 *  @brief    Motion detector based on simple threshold
    @par Description
    This module returns one of the states defined in ::Gestures_Motion_Code_t based on a programmable range threshold.
    When a transition state is returned (::GESTURES_MOTION_RAISE_UP or ::GESTURES_MOTION_DROP_DOWN), it also returns the
    duration (in ms) of the previous state. For instance, assuming ::GESTURES_MOTION_RAISE_UP is returned, duration is
    the time the state ::GESTURES_MOTION_DOWN_STATE has been maintained before the down=>up transition.   
 *  @ingroup tof
 *  @{  
 */
    
/** Codes associated to Motion detection
 */
enum Gestures_Motion_Code_t{
    GESTURES_MOTION_NULL        = 0, /*!< No motion detected */
    GESTURES_MOTION_DOWN_STATE  = 1, /*!< Hand is down (below threshold) */
    GESTURES_MOTION_UP_STATE    = 2, /*!< Hand is up (above threshold) */
    GESTURES_MOTION_RAISE_UP     = 3, /*!< Hand is raising up (transition from DOWN_STATE to UP_STATE is detected) */
    GESTURES_MOTION_DROP_DOWN    = 4, /*!< Hand is dropping down (transition from UP_STATE to DOWN_STATE is detected) */
};

/**
 * @struct MotionData_t
 * @brief Data structure for motion detection
 */ 
typedef struct {
    int threshold;               /*!< Input : Range threshold */
    long duration;               /*!< Output : Duration (in ms) */
    int previousRange;           /*!< Private : Obvious */
    unsigned long timestamp;     /*!< Private : Obvious */
    bool firstTime;              /*!< Private : True only the first time */   
} MotionData_t;

/**
 * @brief Initialize Motion data
 * @return 0 on success
 */
int tof_initMotion(int threshold, MotionData_t *data);
/**
 * @brief Return current motion state
   @return One of the states defined in ::Gestures_Motion_Code_t 
 */
int tof_getMotion(int32_t range_mm, MotionData_t *data);

 /** @}  */
    
#ifdef __cplusplus
}					
#endif	
#endif /* TOF_MOTION_H_ */
