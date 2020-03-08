#ifndef COMMON_H
#define COMMON_H

#define NUM_CHANNELS 16

#define SET_BIT(x, n) ((x)|= (1 << n))
#define CLEAR_BIT (x, n) ((x)&= ~(1 << n))
#define GET_BIT(x, n) ((x) & (1 << n))
#define LED_MASK(n) ( 1 << n )
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

static float Lerp(float A, float B, float alpha)
{
    return (1-alpha)*A + alpha*B;
}
#endif // !COMMON_H
