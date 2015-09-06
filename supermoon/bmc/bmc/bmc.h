/* 
 * File:   bmc.h
 * Author: root
 *
 * Created on September 21, 2012, 12:54 PM
 */

#ifndef BMC_H
#define	BMC_H

#ifdef	__cplusplus
extern "C" {
#endif

#define TRUE    1
#define FALSE   0
#define AI_CHAN 16
#define V_SCALE 1000.0
#define C_SCALE 10.0
#define STATION 1

    struct didata {
        unsigned char D0 : 1; // 
        unsigned char D1 : 1; // 
        unsigned char D2 : 1; // 
        unsigned char D3 : 1; // 
        unsigned char D4 : 1; // 
        unsigned char D5 : 1; // 
        unsigned char D6 : 1; // 
        unsigned char D7 : 1; // 
    } volatile ditype;

    struct dodata {
        unsigned char D0 : 1; // 
        unsigned char D1 : 1; // 
        unsigned char D2 : 1; // 
        unsigned char D3 : 1; // 
        unsigned char D4 : 1; // 
        unsigned char D5 : 1; // 
        unsigned char D6 : 1; // 
        unsigned char D7 : 1; // 
    } volatile dotype;

    typedef struct bmcdata {
        double pv_voltage, cm_voltage, cm_current,cm_null,cm_amps;
        struct didata datain;
        struct dodata dataout;
        lsampl_t    raw[AI_CHAN];    // number if AI channels with binary values
        int32_t utc;
    }
    volatile bmctype;

#ifdef	__cplusplus
}
#endif

#endif	/* BMC_H */

