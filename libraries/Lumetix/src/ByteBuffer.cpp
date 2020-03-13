#include "ByteBuffer.h"

const char* ByteBuffer::s_OverflowMsg = "Buffer overflow detected!\n";

ByteBuffer ByteBuffer::Allocate(size_t numBytes)
{
    ByteBuffer outBuffer;
    outBuffer.m_Count = numBytes;
    outBuffer.m_rpos = 0;
    outBuffer.m_wpos = 0;

    outBuffer.m_Data = (byte*)(malloc(sizeof(byte) * numBytes));
    return outBuffer;
}

ByteBuffer::ByteBuffer()
    : m_Data(nullptr)
    , m_Count(0)
    , m_rpos(0)
    , m_wpos(0)
{
}

ByteBuffer::~ByteBuffer()
{
    if(m_Data)
    {
        free (m_Data);
        m_Data = nullptr;
    }
}

ByteBuffer& ByteBuffer::Resize(size_t numBytes)
{
    if(m_Data)
    {
        free(m_Data);
        m_Data = nullptr;
    }

    m_Count = numBytes;
    m_rpos = 0;
    m_wpos = 0;
    m_Data = (byte*)malloc(sizeof(byte) * numBytes);

    return *this;
}

ByteBuffer& ByteBuffer::PutByte(byte b)
{
    Write<byte>(b);
    return *this;
}

ByteBuffer& ByteBuffer::PutInt(long i)
{
    Write<long>(i);
    return *this;
}

ByteBuffer& ByteBuffer::PutFloat(float f)
{
    Write<float>(f);
    return *this;
}

ByteBuffer& ByteBuffer::PutDouble(double d)
{
    Write<double>(d);
    return *this;
}

ByteBuffer& ByteBuffer::PutShort(short s)
{
    Write<short>(s);
    return *this;
}

ByteBuffer& ByteBuffer::PutChar(char c)
{
    Write<char>(c);
    return *this;
}

ByteBuffer& ByteBuffer::PutBytes(byte* bytes, size_t count)
{
    if(bytes)
    {
        for(int i = 0; i < count; i++)
        {
            PutByte(bytes[i]);
        }
    }

    return *this;
}


byte ByteBuffer::Get()
{
    return Read<byte>();
}

byte ByteBuffer::Get(uint32_t idx)
{
    return Read<byte>(idx);
}

void ByteBuffer::GetBytes(byte* inOutBytes, size_t count)
{
    memcpy(inOutBytes, m_Data, count);
}

byte ByteBuffer::GetByte()
{
    return Read<byte>();
}

long ByteBuffer::GetInt()
{
    return Read<long>();
}

float ByteBuffer::GetFloat()
{
    return Read<float>();
}

double ByteBuffer::GetDouble()
{
    return Read<double>();
}

short ByteBuffer::GetShort()
{
    return Read<short>();
}

char ByteBuffer::GetChar()
{
    return Read<char>();
}

byte* ByteBuffer::ToByteArray() const
{
    byte* outBytes = (byte*)(malloc(sizeof(byte) * m_Count));
    memcpy(outBytes, m_Data, m_Count);
    return outBytes;
}