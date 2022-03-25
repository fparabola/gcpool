//
// Created by su on 2022/3/17.
//

#ifndef MYPOOL_COMMON_H
#define MYPOOL_COMMON_H

//#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <iostream>
#include <cstring>

#define LOG_INFO 1
#define LOG_DEBUG 2
#define LOG_WARNING 3
#define LOG_ERROR 4

//#define LOGLEVEL LOG_INFO
#define LOGLEVEL LOG_ERROR
//#define DEBUG_ALLOC
//#define DEBUG

#define LOG_PRINT(level,fmt,...)\
    do {\
        if (level >= LOGLEVEL) {\
            printf("%s:%d\t[%s]\t " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__);fflush(stdout); \
        }\
    }\
    while (0)

using std::size_t;

template<class D, class S>
D pointer_cast(S ps) {
    D d = static_cast<D>(static_cast<void*>(ps));
    if(std::is_pointer<S>() && std::is_pointer<D>()) {
        return static_cast<D>(static_cast<void*>(ps));
    } else {
        LOG_PRINT(LOG_DEBUG, "Attempt to cast non-pointer type by pointer_cast");
        throw -1;
    }
}

template<class R, class T>
R& memasref(T* pt) {
//    return *static_cast<R*>(static_cast<void*>(pt));
    return *pointer_cast<R*>(pt);
}

#endif //MYPOOL_COMMON_H
