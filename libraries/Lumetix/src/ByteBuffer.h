#ifndef BYTE_BUFFER_H
#define BYTE_BUFFER_H
#include <Arduino.h> // Unfortunate... also includes stdio
#include <string.h>

/*  Byte Buffer class that emulates the Android/Java buffer functionalities 
*   This class can be used to perform compatible operations and transmissions
*   of raw data between Android and Arduino.
*/
struct ByteBuffer
{
    static ByteBuffer Allocate(size_t numBytes);
    ~ByteBuffer();
    ByteBuffer& operator=(const ByteBuffer& Other) = delete;

    ByteBuffer& Resize(size_t numBytes);

    ByteBuffer& PutByte(byte b);
    ByteBuffer& PutInt(long i);  // IMPORTANT. Size mismatch between Arduino and Android possible. Be explicit! (uint32_t ?). "FIXED": Using long for 4-bytes
    ByteBuffer& PutFloat(float f);
    ByteBuffer& PutDouble(double d);
    ByteBuffer& PutShort(short s);
    ByteBuffer& PutChar(char c);
    ByteBuffer& PutBytes(byte* bytes, size_t count);

    void        GetBytes(byte* inOutBytes, size_t count);
    byte        GetByte();
    long        GetInt();
    float       GetFloat();
    double      GetDouble();
    short       GetShort();
    char        GetChar();
    
    byte* ToByteArray() const;
    byte  Get();                    // Relative Get Method
    byte  Get(uint32_t idx);        // Absolute Get Method
    inline size_t GetNumBytes() const { return m_Count; }

private:
    template<typename T>
    T Read();

    template<typename T>
    T Read(uint32_t pos);

    template<typename T>
    void Write(T data);

    template<typename T>
    void Write(uint32_t pos, T data);

    ByteBuffer();
private:
    uint32_t m_rpos;
    uint32_t m_wpos;
    byte* m_Data;
    size_t m_Count;

    static const char* s_OverflowMsg;
};

template<typename T>
T ByteBuffer::Read()
{
    T data = Read<T>(m_rpos);
    m_rpos += sizeof(T);

    return data;
}

template<typename T>
T ByteBuffer::Read(uint32_t pos)
{
    if((pos + sizeof(T)) > m_Count)
    {
        Serial.println(s_OverflowMsg);
        abort();
        return (T)0;
    }

    return  *( (T*)(&m_Data[pos]) );
}

template<typename T>
void ByteBuffer::Write(T data)
{
    Write<T>(m_wpos, data);
    m_wpos += sizeof(T);
}

template<typename T>
void ByteBuffer::Write(uint32_t pos, T data)
{
    if(pos + sizeof(T) > m_Count)
    {
        Serial.println(s_OverflowMsg);
        abort();
        return;
    }

    // memset/memcpy doesn't work on arduino for some reason
    memcpy(&m_Data[pos], (void*)&data, sizeof(T));
}
#endif // !BYTE_BUFFER_H
