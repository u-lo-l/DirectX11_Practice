#ifndef __MATH_HLSL__
# define __MATH_HLSL__

# define Complex float2

const static float PI = 3.14159274f;
const static float EPSILON = 1e-7f;

Complex ComplexMul(Complex A, Complex B)
{
    Complex Result;
    
    Result.x = A.x * B.x - A.y * B.y;
    Result.y = A.x * B.y + A.y * B.x;

    return Result;
}

Complex ComplexConj(Complex A)
{
    A.y = - A.y;
    return A;
}

#endif