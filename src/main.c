#include <stdio.h>

#include <cglm/cglm.h>

int main()
{
    vec2 test = GLM_VEC2_ONE_INIT;
    test[0] = 10.0f;
    printf("Hello %f, %f\n", test[0], test[1]);

    return 0;
}