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
 * @file tof_gestures.h
 * $Date: 2015-11-10 11:21:53 +0100 (Tue, 10 Nov 2015) $
 * $Revision: 2612 $
 */

#ifndef TOF_GESTURES_H_
#define TOF_GESTURES_H_

#include <stdlib.h>
#include "tof_gestures_platform.h"

#ifdef __cplusplus
extern "C" {
#endif
    
/** @defgroup tof_gestures 
 *  @brief    ToF Gesture functions detecting gestures
 */
       
/** @defgroup tof 
 *  @brief    ToF functions (performing operations on typical ToF data such as range, signalRate
 */

/** @defgroup misc 
 *  @brief    Misc functions
 */    


/** Gesture codes
 */
enum Gestures_Code_t{
    GESTURES_NULL                   = 0,    /*!< No gesture detected */
    GESTURES_SINGLE_TAP             = 1,    /*!< Single tap detected : __TAP__*/
    GESTURES_SINGLE_SWIPE           = 2,    /*!< Single swipe detected (flat hand) : __WIPE__*/
    GESTURES_DOUBLE_TAP             = 3,    /*!< Double tap detected : __TAPS__*/
    GESTURES_DOUBLE_SWIPE           = 4,    /*!< Double swipe detected : __SWIPES__*/
    GESTURES_HAND_ENTERING          = 5,    /*!< Hand is entering in device field of view : __LC__ */
    GESTURES_HAND_LEAVING           = 6,    /*!< Hand is leaving from device field of view : __LC__ */
    GESTURES_LEVEL_CONTROLLED       = 7,    /*!< Hand is controlling a level from the measured distance : __LC__ */
    GESTURES_SWIPE_LEFT_RIGHT       = 8,    /*!< Directional swipe from left to right : __DIRSWIPE__ */
    GESTURES_SWIPE_RIGHT_LEFT       = 9,    /*!< Directional swipe from right to left : __DIRSWIPE__ */
    GESTURES_FLIP_PAGE_LEFT_RIGHT   = 10,    /*!< Page flip from left to right (hand at 45) : __FLIPPAGE__ */
    GESTURES_FLIP_PAGE_RIGHT_LEFT   = 11,    /*!< Page flip from right to left (hand at 45) : __FLIPPAGE__ */
    
    GESTURES_STARTED                = -1,   /*!< A gesture start is detected but full gesture detection is not completed (yet) */
    GESTURES_DISCARDED              = -2,   /*!< Gesture is discarded */
    GESTURES_DISCARDED_TOO_SLOW     = -3,   /*!< Gesture is discarded because it is too slow */
    GESTURES_DISCARDED_TOO_FAST     = -4,   /*!< Gesture is discarded because it is too fast */
};

#ifndef MIN
#   define MIN(a,b) (((a)<(b))?(a):(b))
#endif 
#ifndef MAX
#   define MAX(a,b) (((a)>(b))?(a):(b))
#endif 
#ifdef __cplusplus
}					
#endif	
#endif /* TOF_GESTURES_H_ */
