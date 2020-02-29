#include "Curve.h"
#include <assert.h>
//#include <cstring>
#include <HardwareSerial.h>

/* Because we can't include cstring for some reason. Thanks Arduino */
static void* memcpy(void* dest, void* src, size_t len)
{
    char *d = (char*)dest;
    const char *s = (char*)src;
    while(len--)
        *d++ = *s++;
    return dest;
}

namespace Interp
{
    static float Lerp(float p1, float p2, float alpha)
    {
        return ((1 - alpha)*p1 + alpha*p2);
    }

    static float Cubic(float p0, float p1, float p2, float p3, float alpha)
    {
        // Recall catmull rom spline
        /*
        *   f(0) = p1
        *   f(1) = p2
        *   f'(0) = (p2-p0) * 0.5
        *   f'(1) = (p3-p1) * 0.5
        */

        return alpha*alpha*alpha*(-0.5f*p0 + 1.5f*p1 - 1.5f*p2 + 0.5f*p3)
            + alpha*alpha*(p0 - 2.5f*p1 + 2 * p2 - 0.5f*p3)
            + alpha*(-0.5f*p0 + 0.5f*p2)
            + p1;
    }
}

Curve::Curve(size_t capacity)
    : m_Capacity(capacity)
{
    assert(capacity > 2);
    m_Keys = (Key*)calloc(capacity, sizeof(Key));

    m_Keys[0] = {0.f, 0.f};
    m_Keys[1] = {1.f, 0.f};

    m_NumKeys = 2;
}

Curve::Curve(Key* keys, size_t count)
{
    assert(keys && count > 2);
    if(keys)
    {
        m_Keys = (Key*)calloc(count * SLACK, sizeof(Key));

        //memcpy(m_Keys, keys, count);
        m_NumKeys = count;
        
        for(int i = 0; i < m_NumKeys; i++)
        {
        	m_Keys[i] = keys[i];
		}
    }
}

Curve::Curve(const Curve& Other)
{
    m_Keys = Other.m_Keys;
    m_Capacity = Other.m_Capacity;
    m_NumKeys = Other.m_NumKeys;
}

Curve::~Curve()
{
    assert(m_Keys);
    free(m_Keys);
    m_Keys = nullptr;
}

void Curve::Clone(const Curve& Other)
{
    if(m_Keys)
    {
        free(m_Keys);
        m_Keys = nullptr;
    }

    m_Keys = (Key*)malloc(Other.m_Capacity * sizeof(Key));
	m_Keys = Other.m_Keys;

    for(int i = 0; i < Other.m_NumKeys; i++)
    {
    	//Key newKey;
    	//newKey.Alpha = Other.m_Keys[i].Alpha;
    	//newKey.Value = Other.m_Keys[i].Value;
    	
    	//m_Keys[i].Alpha = Other.m_Keys[i].Alpha;
    	//m_Keys[i].Value = Other.m_Keys[i].Value;
    	Serial.print("Copied: "); Serial.println(m_Keys[i].Alpha);
	}
    //memcpy(m_Keys, Other.m_Keys, Other.m_NumKeys);

    m_Capacity = Other.m_Capacity;
    m_NumKeys = Other.m_NumKeys;
}

void Curve::AddKey(float alpha, float value)
{
    Key p = {alpha, value};
    AddKey(p);
}

void Curve::AddKey(const Key& k)
{
    if(m_NumKeys + 1 > m_Capacity);
        Resize(m_Capacity * SLACK);
    
    // Overwriting last value
    if(k.Alpha == 1.f)
    {
        m_Keys[m_NumKeys-1].Value = k.Value;
    }
    else if(k.Alpha == 0.f)
    {
        m_Keys[0].Value = k.Value;
    }
    else
    {
        for(int i = 0; i < m_NumKeys; i++)
        {
            // Overwritting an existing key
            if(m_Keys[i].Alpha == k.Alpha)
            {
                m_Keys[i].Value = k.Value;
                return;
            }
            
            // Need to insert key right before this
            if(m_Keys[i].Alpha > k.Alpha)
            {
                Insert(k, i); // Pushes entries right
                return;
            }
        }
    }
}

Key& Curve::EditKey(int atIdx)
{
    assert(atIdx < m_NumKeys && atIdx >= 0);
    return m_Keys[atIdx];
}

void Curve::RemoveKey(int atIdx)
{
    assert(atIdx < m_NumKeys && atIdx >= 0);
    Remove(atIdx);
}

float Curve::Evaluate(float t, bool linear) const
{
    int begin = 0;
    int end = 0;

    for(int i = 0; i < m_NumKeys; i++)
    {
        end = i;

        // Our parameter is between these two points!
        if(m_Keys[begin].Alpha <= t && m_Keys[end].Alpha > t)
        {
            break;
        }
        begin = i;
    }
	
//	Serial.print("Begin: "); Serial.println(begin);
//	Serial.print("End: "); Serial.println(end);

    Key k1 = m_Keys[begin];
    Key k2 = m_Keys[end];
    const float local_t = (t - k1.Alpha)/(k2.Alpha - k1.Alpha);
    if(linear)
    {
        return Interp::Lerp(k1.Value, k2.Value, local_t);
    }
    else
    {
        Key k0 = k1;
        Key k3 = k2;

        if(begin > 0)
            k0 = m_Keys[begin-1];
        if(end < m_NumKeys-1)
            k3 = m_Keys[end+1];

//		Serial.print("k0: "); Serial.println(k0.Value);
//		Serial.print("k1: "); Serial.println(k1.Value);
//		Serial.print("k2: "); Serial.println(k2.Value);
//		Serial.print("k3: "); Serial.println(k3.Value);

        return Interp::Cubic(k0.Value, k1.Value, k2.Value, k3.Value, local_t);
    }
}

void Curve::Rebuild()
{
    // Cook the curve by resorting it by alpha values
    for(int i = 0; i < m_NumKeys - 1; i++)
    {
        if(m_Keys[i].Alpha > m_Keys[i + 1].Alpha)
        {
            Swap(m_Keys[i], m_Keys[i+1]);
        }
    }

    // Need to pad since user hasn't completed the curve
    if(m_Keys[0].Alpha != 0.f)
    {
        Key k = { 0, m_Keys[0].Value };
        Insert(k, 0);
    }

    if(m_Keys[m_NumKeys - 1].Alpha != 1.f)
    {
        Key k = { 1, m_Keys[m_NumKeys - 1].Value };
        Insert(k, m_NumKeys-1);
    }
}

void Curve::Insert(Key k, int atIdx)
{
    // Should never enter here
    if(m_Capacity <= m_NumKeys + 1)
    {
        assert(false); // for logical debugging
        Resize(m_Capacity * SLACK);
    }

    for(int i = m_NumKeys; i > atIdx; i--)
    {
        Swap(m_Keys[i], m_Keys[i-1]);
    }

    m_Keys[atIdx] = k;
    m_NumKeys++;
}

void Curve::Remove(int atIdx)
{
    assert(atIdx < m_NumKeys && atIdx >= 0);

    m_Keys[atIdx].Alpha = 0;
    m_Keys[atIdx].Value = 0;

    // Should never enter here
    if(m_Capacity <= m_NumKeys + 1)
    {
        assert(false); // for logical debugging
        Resize(m_Capacity * SLACK);
    }

    // Bubbles out the removed element
    for(int i = atIdx; i < m_NumKeys; i++)
    {
        Swap(m_Keys[i], m_Keys[i+1]);
    }
}

void Curve::Swap(Key& A, Key& B)
{
    Key tmp = A;
    A = B;
    B = tmp;
}

void Curve::Resize(size_t newCapacity)
{
    assert(newCapacity > (m_NumKeys + 1));
    
    Key* newKeys = (Key*)calloc(newCapacity, sizeof(Key));

    //memcpy(newKeys, m_Keys, m_NumKeys);
	for(int i = 0; i < m_NumKeys; i++)
	{
		newKeys[i] = m_Keys[i];
	}
	
    free(m_Keys);
    m_Keys = newKeys;
}
