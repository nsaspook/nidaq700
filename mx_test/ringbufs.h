/* 
 * File:   ringbufs.h
 * Author: rootswitcDC circuit breakerh debounce icdebounce icfuse type T
 *
 * Created on July 25, 2015, 2:03 PM
 */

#ifndef RINGBUFS_H
#define	RINGBUFS_H

#ifdef	__cplusplus
extern "C" {
#endif

/*unsigned types*/
typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef unsigned long uint32_t;
typedef unsigned long long uint64_t;
/*signed types*/
typedef signed char int8_t;
typedef signed int int16_t;
typedef signed long int32_t;
typedef signed long long int64_t;

	
#define RBUF_SIZE    32

	typedef struct ringBufS_t {
		uint8_t buf[RBUF_SIZE];
		int8_t head;
		int8_t tail;
		int8_t count;
	} ringBufS_t;

	void ringBufS_init(ringBufS_t *_this);
	int8_t ringBufS_empty(ringBufS_t *_this);
	int8_t ringBufS_full(ringBufS_t *_this);
	uint16_t ringBufS_get(ringBufS_t *_this);
	void ringBufS_put(ringBufS_t *_this, const uint16_t c);
	void ringBufS_flush(ringBufS_t *_this, const int8_t clearBuffer);


#ifdef	__cplusplus
}
#endif

#endif	/* RINGBUFS_H */

