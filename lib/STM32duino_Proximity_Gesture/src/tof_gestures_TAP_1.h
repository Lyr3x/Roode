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
 * @file tof_gestures_TAP_1.h
 * $Date$
 * $Revision$
 */

#ifndef TOF_GESTURES_TAP_1_H_
#define TOF_GESTURES_TAP_1_H_

#include "tof_gestures_platform.h"
#include "ring_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif
 
/** @defgroup tof_gestures_TAP_1 Single TAP (1)
 *  @brief    Single Tap gesture detection based on simple analysis (MAD) of range (single ToF device)
    @par Description
    This tof_gestures module allows to detect in a very robust way a single TAP (flat hand going down) with a single ToF device. Returned
    gestures code are one of the following ones : ::GESTURES_SINGLE_TAP or ::GESTURES_NULL in case no TAP is detected.
    @par Algorithm Description
    The approach consists in collecting __range__ samples when a target (hand) is detected by the ranging device. Then, MAD
    (Mean of Aboslute Difference versus the mean) is calculated and TAP gesture is detected based on a threshold.
 *  @ingroup tof_gestures
 *  @{  
 */



/** Mean variation to start tap detection (5%) */
#define TAP_1_BACKGROUND_VARIATION 5
/** Buffer size used for background mean calculation */
#define TAP_1_BUFFER_SIZE 16
/** Minimum number of ranging data to get before deciding to detect a TAP */
#define TAP_1_NB_OF_DATA_TO_DECIDE 4

/**
 * @struct Gesture_TAP_1_Data_t
 * @brief Data structure for single tap gesture detection
 */
typedef struct {
	int gestureCode;                /*!< Output : Gesture intermediate code */
    ring_buffer_t rangeList;  		/*!< Private : List of range samples */
    int timestampLastDetectedTap;	/*!< Private : Time of the last detected TAP */
    int nbOfDataToDecide;   		/*!< Private : Number of range data collected to detect a TAP in a very robust way */
    int meanBackground;				/*!< Private : Mean distance of the background */
    int timestamp;					/*!< Private : Used for internal timing */
} Gesture_TAP_1_Data_t;    

/**
 * @brief Initialize gesture data
 * @return 0 on success
 */
int tof_gestures_initTAP_1(Gesture_TAP_1_Data_t *data);


/**
 * @brief Detect gesture
 * @return One of these gestures code from ::Gestures_Code_t : ::GESTURES_SINGLE_TAP or ::GESTURES_NULL
 * @warning This function must be called all the time, even if no valid range_mm is available from the ToF device.
 * In that case, call the function with the typical max ranging capability of the device
 */
int tof_gestures_detectTAP_1(int32_t range_mm, Gesture_TAP_1_Data_t *data);

 /** @}  */
#ifdef __cplusplus
}					
#endif	
#endif /* TOF_GESTURES_TAP_1_H_ */
