/*
 * inline_f.h
 *
 *  Created on: Sep 4, 2013
 *      Author: speters
 */

#ifndef INLINE_F_H_
#define INLINE_F_H_
#include "enums.hpp"

//#define bit_set(eim eb) (eim & (1l << eb))

inline static bool bit_set(long eim, const EIM_BIT eb) {
	return (eim & (1l << eb));
}

#endif /* INLINE_F_H_ */
