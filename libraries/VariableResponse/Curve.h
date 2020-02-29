#ifndef CURVE_H
#define CURVE_H

#define SLACK (2)
struct Key
{
    float Alpha;
    float Value;
};

/*  Represent a polynomial Curve interpolated using
*   a cubic Catmull Rom interpolation. 
*/
class Curve
{
public:
    Curve(size_t capacity);
    Curve(Key* keys, size_t count);
    ~Curve();

    /* Add a new key to the curve, ensuring values are unique */
    void AddKey(float alpha, float value);
    void AddKey(const Key& k);

    /* Returns a key at the specified index for edit */
    Key& EditKey(int atIdx);
    void RemoveKey(int atIdx);

    /* Called after a curve has been modified to update and correct the curve values */
    void Rebuild();

    /*  Evaluate the curve at some parameter t [0, 1].
    *   @Param t: Parameter or weight to interpolate with
    *   @Param linear: If true, interpolation is a simple Lerp, otherwise use a Catmull Rom cubic interpolation
    */
    float Evaluate(float t, bool linear = false) const;

    size_t GetNumKeys() { return m_NumKeys; }

private:
    /* Operations on key vector */
    void Resize(size_t newCapacity);
    void Insert(Key k, int atIdx);
    void Remove(int atIdx);
private:
    
    // Well.... can't use STL containers with Arduino so, here we go
    // hacking in our own, bug prone, resizable array of keys.
    Key* m_Keys;
    size_t m_NumKeys;
    size_t m_Capacity;
};
#endif // !CURVE_H
