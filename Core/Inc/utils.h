/*
 * utils.h
 *
 *  Created on: Jul 22, 2024
 *      Author: ethan
 */

#ifndef INC_UTILS_H_
#define INC_UTILS_H_

typedef enum wsst_error_e {
	WSST_OK = 0,
	WSST_ERROR,
	WSST_BUSY,
	WSST_NOT_SUPPORTED,
	WSST_UNIMPLEMENTED,
	WSST_BAD_PARAMS,
	WSST_OUT_OF_MEM,
	WSST_UNKNOWN
} wsst_error_t;

#endif /* INC_UTILS_H_ */
