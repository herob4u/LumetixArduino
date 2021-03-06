#ifndef COMMON_H
#define COMMON_H

#define NUM_CHANNELS 16

#define SET_BIT(x, n) ((x)|= (1 << n))
#define CLEAR_BIT (x, n) ((x)&= ~(1 << n))
#define GET_BIT(x, n) ((x) & (1 << n))
#define LED_MASK(n) ( 1 << n )
#define LED_MASK_REVERSE(x) ( x << 15)
#define LED_MASK_ALL ( 0xFFFF)

/* Physical Mapping for our virtual panel where entries are LED pinouts from left to right */
static unsigned int PhysicalMapping[16]
{
    7,6,5,4,3,2,1,0,
    15,14,13,12,11,10,9,8
};



#define ENUM_BITFLAG(enumClass) \
inline enumClass operator |(const enumClass& A, const enumClass& B) { return static_cast<enumClass>(static_cast<short>(A) | static_cast<short>(B)); }  \
inline enumClass operator &(const enumClass& A, const enumClass& B) { return static_cast<enumClass>(static_cast<short>(A) & static_cast<short>(B)); }\
inline enumClass& operator |=(enumClass& A, const enumClass& B)     { return (A = A|B); }                                                              \
inline enumClass& operator &=(enumClass& A, const enumClass& B)     { return (A = A&B); }

enum class EGradientDirection : short
{
    HORIZONTAL,
    VERTICAL
};

#define EPSILON 0.00009f

static inline float Lerp(float A, float B, float alpha)
{
    return (1-alpha)*A + alpha*B;
}

static inline float Clamp(float val, float min, float max)
{
    return val < min ? min : val > max ? max : val;
}

static inline float Abs(float val)
{
    return (val >= 0 ? val : -val);
}

static inline bool IsNearlyEqual(float A, float B, float threshold = EPSILON)
{
    return (Abs(A-B) <= threshold);
}

/* Comment out for "release".*/
//#define DEBUG_MODE
#ifdef DEBUG_MODE
    #define LOG(x) Serial.print(x)
    #define LOGN(x) Serial.println(x)
#else
    #define LOG(x)
    #define LOGN(x)
#endif // DEBUG_MODE

#endif // !COMMON_H
