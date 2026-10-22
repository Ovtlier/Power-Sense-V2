/*
 * structs.h
 *
 *  Created on: Jul 28, 2025
 *      Author: Peter
 */
#include <stdint.h>
#ifndef INC_STRUCTS_H_
#define INC_STRUCTS_H_

typedef enum calib_mode{
    calib_none=0,
    calib_multiplier,
    calib_remap
}calib_mode;

typedef struct raw_calib {
    calib_mode mode;
    union {
        struct {
        double multiplier;
        double offset;
        };
        struct{
        uint32_t remap_low_in;
        uint32_t remap_low_out;
        uint32_t remap_high_in;
        uint32_t remap_high_out;
        bool clamp;
        };
    };

} raw_calib;


#endif /* INC_STRUCTS_H_ */
