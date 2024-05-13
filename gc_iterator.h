// Exception thrown when an attempt is made to
// use an Iter that exceeds the range of the
// underlying object.
class OutOfRangeExc
{
    OutOfRangeExc()
    {
        std::cout<<"OUT OF RANGE" << '\n';
    }
};


/*
This iterator class provides functionality to iterate over elements pointed to by a Pointer object managing an array. 
It doesn't participate in garbage collection and doesn't affect reference counts.
*/
template <class T>
class Iter
{
    T *ptr; // current Position
    T *end; // last position
    T *begin; //first position
    unsigned length; // length of current state
  public:
    Iter() //at first we initialize all with null and for sure no length is being set for now
    {
        ptr = end = begin = NULL;
        length = 0;
    }
    Iter(T *p, T *first, T *last)
    {
        ptr = p;
        end = last;
        begin = first;
        length = last - first;
    }
    // Return length of sequence to which this
    // Iter points.
    unsigned size() { return length; }

    // access the pointer(this version is for normal object (not pointer))
    T &operator*()
    {
        if ((ptr >= end) || (ptr < begin))
            throw OutOfRangeExc();
        return *ptr;
    }
    // access the pointer(this version is for  pointers object (it returns the address))
    T *operator->()
    {
        if ((ptr >= end) || (ptr < begin))
            throw OutOfRangeExc();
        return ptr;
    }
    Iter operator++()
    {
        ptr++;
        return *this;
    }
    //Prefix Increment
    Iter operator--()
    {
        ptr--;
        return *this;
    }
    //Postfix Increment
    Iter operator++(int PostFixPurpose)
    {
        T *tmp = ptr;
        ptr++;
        return Iter<T>(tmp, begin, end);
    }
    Iter operator--(int PostFixPurpose)
    {
        T *tmp = ptr;
        ptr--;
        return Iter<T>(tmp, begin, end);
    }
    // Return a reference to the object at the
    // specified index
    T &operator[](int i)
    {
        if ((i < 0) || (i >= (end - begin)))
            throw OutOfRangeExc();
        return ptr[i];
    }
    // Define the relational operators.
    bool operator==(Iter op2)
    {
        return ptr == op2.ptr;
    }
    bool operator!=(Iter op2)
    {
        return ptr != op2.ptr;
    }
    bool operator<(Iter op2)
    {
        return ptr < op2.ptr;
    }
    bool operator<=(Iter op2)
    {
        return ptr <= op2.ptr;
    }
    bool operator>(Iter op2)
    {
        return ptr > op2.ptr;
    }
    bool operator>=(Iter op2)
    {
        return ptr >= op2.ptr;
    }
    // Subtract an integer from an Iter.
    Iter operator-(int n)
    {
        ptr -= n;
        return *this;
    }
    // Add an integer to an Iter.
    Iter operator+(int n)
    {
        ptr += n;
        return *this;
    }
    // Return number of elements between two Iters.
    int operator-(Iter<T> &itr2)
    {
        return ptr - itr2.ptr;
    }
};
