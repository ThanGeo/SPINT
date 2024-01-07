#ifndef APPROX_H
#define APPROX_H

typedef enum ApproxType
{
    // point approximations

    // polygon approximations
    POL_APRIL = 10,
    POL_RI,
    POL_CH_5C,
    POL_RA,
    // linestring approximations
    LS_APRIL = 20,

} ApproxTypeE;


#endif