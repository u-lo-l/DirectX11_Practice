#ifndef __COMPLEX_HLSL__
# define __COMPLEX_HLSL__

# define Complex float2

#ifndef PI
# define PI (3.14159274f)
#endif
#ifndef EPSILON
# define EPSILON (1e-7f)
#endif 

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