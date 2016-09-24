#ifndef ___RET_CODE_H__
#define ___RET_CODE_H__

typedef enum {
    /* Common Error */
    RET_OK              =  0,
    RET_ERROR           = -1,
    RET_TIMEOUT         = -2,
    RET_OVER_LENGTH     = -3,

    /* Network Error */
    RET_CONNECT_FAIL    = -11,
} RetCode_t;

#endif /* ___RET_CODE_H__ */
