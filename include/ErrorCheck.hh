#pragma once
#include <stdlib.h>
#include <cstdio>
#define ERROR_CHECK(ret, expect, msg)\
    do {\
        if ((ret) == (expect)) {\
            perror(msg);\
            exit(-1);\
        }\
    } while(0)