/*
This class stores information about a memory block managed by the garbage collector
*/
template <class T>
class PtrDetails
{
  public:
    unsigned refcount; //  reference count 
    T *memPtr;         // pointer to allocated memory
    bool isArray; // checks if it is array or not (for deleting [] or just delete)
    unsigned arraySize; // size of array

    PtrDetails(T *p , int size = 0)
    {
        refcount =1 ; //this class isnt responsible for the increasing the refcount 
        memPtr = p;
        if(size != 0) // here we check the size to decide if its array or not (for deleteing purpose)
        {
            isArray = true;
            arraySize =size;
        }
        else
        {
            isArray = false;
        }
    }
};
// Overloading operator== allows two class objects to be compared.
// This is needed by the STL list class.
template <class T>
bool operator==(const PtrDetails<T> &ob1,
                const PtrDetails<T> &ob2)
{
    return ob1.memPtr == ob2.memPtr;
}
