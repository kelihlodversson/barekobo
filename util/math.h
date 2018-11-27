#pragma once

/** simple math functions to replace the ones from the missing standard math library.
  */

namespace hfh3
{

/** round a double precision floating point variable to the nearest whole number */
inline double Round(double d)
{
    return __builtin_round(d);
}

/** Calculate the square root of a double precision floating point value
  * using the VSQRT ARM floating point instruction. */
inline double Sqrt(double d)
{
    double res;
    asm (
        "vsqrt.f64 %[result], %[input]"
        : [result] "=w" (res)
        : [input] "w" (d)
    );

    return res;
}

/** Calculate the square root of a single precision floating point value
  * using the VSQRT ARM floating point instruction. */
inline float Sqrt(float d)
{
    float res;
    asm (
        "vsqrt.f32 %[result], %[input]"
        : [result] "=t" (res)
        : [input] "t" (d)
    );

    return res;
}

}
