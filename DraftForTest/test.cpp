#include <iostream>
#include <list>
#include <typeinfo>
#include <cstdlib>

class OutOfRangeExc {
public:
    OutOfRangeExc() {
        std::cout << "OUT OF RANGE" << '\n';
    }
};

template <class T>
class Iter {
    T *ptr;
    T *end;
    T *begin;
    unsigned length;

public:
    Iter() {
        ptr = end = begin = NULL;
        length = 0;
    }

    Iter(T *p, T *first, T *last) {
        ptr = p;
        end = last;
        begin = first;
        length = last - first;
    }

    unsigned size() { return length; }

    T &operator*() {
        if ((ptr >= end) || (ptr < begin))
            throw OutOfRangeExc();
        return *ptr;
    }

    T *operator->() {
        if ((ptr >= end) || (ptr < begin))
            throw OutOfRangeExc();
        return ptr;
    }

    Iter operator++() {
        ptr++;
        return *this;
    }

    Iter operator--() {
        ptr--;
        return *this;
    }

    Iter operator++(int notused) {
        T *tmp = ptr;
        ptr++;
        return Iter<T>(tmp, begin, end);
    }

    Iter operator--(int notused) {
        T *tmp = ptr;
        ptr--;
        return Iter<T>(tmp, begin, end);
    }

    T &operator[](int i) {
        if ((i < 0) || (i >= (end - begin)))
            throw OutOfRangeExc();
        return ptr[i];
    }

    bool operator==(Iter op2) { return ptr == op2.ptr; }
    bool operator!=(Iter op2) { return ptr != op2.ptr; }
    bool operator<(Iter op2) { return ptr < op2.ptr; }
    bool operator<=(Iter op2) { return ptr <= op2.ptr; }
    bool operator>(Iter op2) { return ptr > op2.ptr; }
    bool operator>=(Iter op2) { return ptr >= op2.ptr; }

    Iter operator-(int n) {
        ptr -= n;
        return *this;
    }

    Iter operator+(int n) {
        ptr += n;
        return *this;
    }

    int operator-(Iter<T> &itr2) { return ptr - itr2.ptr; }
};

template <class T>
class PtrDetails {
public:
    unsigned refcount;
    T *memPtr;
    bool isArray;
    unsigned arraySize{0};

    PtrDetails(T *p, int size = 0) {
        refcount = 1;
        memPtr = p;
        if (size != 0) {
            isArray = true;
            arraySize = size;
        } else {
            isArray = false;
        }
        std::cout << "PtrDetails Constructor called with refcount: " << refcount << ", memPtr: " << memPtr << ", isArray: " << isArray << ", arraySize: " << arraySize << std::endl;
    }
};

template <class T>
bool operator==(const PtrDetails<T> &ob1, const PtrDetails<T> &ob2) {
    return ob1.memPtr == ob2.memPtr;
}

template <class T, int size = 0>
class Pointer {
private:
    static std::list<PtrDetails<T>> refContainer;
    T *addr;
    bool isArray;
    unsigned arraySize;
    static bool first;

    typename std::list<PtrDetails<T>>::iterator findPtrInfo(T *ptr);

public:
    typedef Iter<T> GCiterator;

    Pointer() {
        Pointer(NULL);
    }

    Pointer(T *t);

    Pointer(const Pointer &);

    ~Pointer();

    static bool collect();

    T *operator=(T *t);

    Pointer &operator=(Pointer &rv);

    T &operator*() { return *addr; }

    T *operator->() { return addr; }

    T &operator[](int i) { return addr[i]; }

    operator T *() { return addr; }

    Iter<T> begin() {
        int _size;
        if (isArray)
            _size = arraySize;
        else
            _size = 1;
        return Iter<T>(addr, addr, addr + _size);
    }

    Iter<T> end() {
        int _size;
        if (isArray)
            _size = arraySize;
        else
            _size = 1;
        return Iter<T>(addr + _size, addr, addr + _size);
    }

    static int refContainerSize() { return refContainer.size(); }

    static void showlist();

    static void shutdown();
};

template <class T, int size>
std::list<PtrDetails<T>> Pointer<T, size>::refContainer;

template <class T, int size>
bool Pointer<T, size>::first = true;

template<class T,int size>
Pointer<T,size>::Pointer(T *t){
    std::cout <<"PointerCotr" << std::endl;
    // Register shutdown() as an exit function.
    if (first)
        atexit(shutdown);
    first = false;

    // Lab: Smart Pointer Project Lab
    addr =t ;
    arraySize = size;
    if(size != 0)
    {
        isArray = true;
    }
    else
    {
        isArray = false;
    }
    //push back the content to the refcontainer 
    PtrDetails<T> PD (t,size);
    refContainer.push_back(PD);
    std::cout << "Pointer Constructor called for addr: " << addr << ", isArray: " << isArray << ", arraySize: " << arraySize << std::endl;
}
// Copy constructor.
template< class T, int size>
Pointer<T,size>::Pointer(const Pointer &ob){

    // Lab: Smart Pointer Project Lab
    // make a iterator p to iterate by object adress 
    typename std::list<PtrDetails<T>>::iterator p;
    // Find a pointer in refContainer.
    p=findPtrInfo(ob.addr);

    addr =ob.addr;
    if(size != 0 )
    {
      isArray = true;
      arraySize = size;
    }
     else
    {
     isArray = false;
    }
    // increase refcount 
    p->refcount++;
    std::cout << "Pointer Copy Constructor called for addr: " << addr << ", isArray: " << isArray << ", arraySize: " << arraySize << ", refcount: " << p->refcount << std::endl;
}

// Destructor for Pointer.
template <class T, int size>
Pointer<T, size>::~Pointer(){

    // Lab: New and Delete Project Lab

    typename std::list<PtrDetails<T>>::iterator p;
    p = findPtrInfo(addr);
    if(p->refcount)
    p->refcount--;
    //collect garbage
    //if p->refcount == 0
    collect();
    std::cout << "Pointer Destructor called for addr: " << addr << std::endl;
}

// Collect garbage. Returns true if at least one object was freed.
template <class T, int size>
bool Pointer<T, size>::collect(){

    // LAB: New and Delete Project Lab
    // Note: collect() will be called in the destructor
    typename std::list<PtrDetails<T>>::iterator p;
    bool objectfreed = false;
    do{
    for (p=refContainer.begin();p!=refContainer.end();p++)
     {
        //check if its in use or not if its in use we will skip its iteration
        if(p->refcount > 0)
        {
        continue;
        }

        objectfreed = true;
        //remove the pointer from the refcontainer list
        refContainer.erase(p);
        // delete the memPtr 
        if(p->memPtr)
        {
            if(p->isArray)
            {
                delete [] p->memPtr;
            }
            else
            {
                delete p->memPtr;
            }
        }
        break;
     }
    }
    while(p!=refContainer.end());

    return objectfreed;
}

// Overload assignment of pointer to Pointer.
template <class T, int size>
T *Pointer<T, size>::operator=(T *t){

    // LAB: Smart Pointer Project Lab
    typename std::list<PtrDetails<T>>::iterator p;
    p=findPtrInfo(addr);
    //decrease the refcount because it will be assigned to new adress
    p->refcount--;
    p=findPtrInfo(t);
    PtrDetails<T> PD(t,size);
    refContainer.push_back(PD);
    addr =t ;
    return addr;
}
// Overload assignment of Pointer to Pointer.
template <class T, int size>
Pointer<T, size> &Pointer<T, size>::operator=(Pointer &rv){

    // LAB: Smart Pointer Project Lab
    typename std::list<PtrDetails<T>>::iterator p;
    p=findPtrInfo(addr);
    p->refcount--;
    
    p=findPtrInfo(rv.addr);
    p->refcount++;

    addr=rv.addr;
    return rv;
}

// A utility function that displays refContainer.
template <class T, int size>
void Pointer<T, size>::showlist(){
    typename std::list<PtrDetails<T> >::iterator p;
    std::cout << "refContainer<" << typeid(T).name() << ", " << size << ">:\n";
    std::cout << "memPtr refcount value\n ";
    if (refContainer.begin() == refContainer.end())
    {
        std::cout << " Container is empty!\n\n ";
    }
    for (p = refContainer.begin(); p != refContainer.end(); p++)
    {
        std::cout << "[" << (void *)p->memPtr << "]"
             << " " << p->refcount << " ";
        if (p->memPtr)
            std::cout << " " << *p->memPtr;
        else
            std::cout << "---";
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
// Find a pointer in refContainer.
template <class T, int size>
typename std::list<PtrDetails<T> >::iterator
Pointer<T, size>::findPtrInfo(T *ptr){
    typename std::list<PtrDetails<T> >::iterator p;
    // Find ptr in refContainer.
    for (p = refContainer.begin(); p != refContainer.end(); p++)
        if (p->memPtr == ptr)
            return p;
    return p;
}
// Clear refContainer when program exits.
template <class T, int size>
void Pointer<T, size>::shutdown(){
    if (refContainerSize() == 0)
        return; // list is empty
    typename std::list<PtrDetails<T> >::iterator p;
    for (p = refContainer.begin(); p != refContainer.end(); p++)
    {
        // Set all reference counts to zero
        p->refcount = 0;
    }
    collect();
}
int main() {
    // Create an array of integers
    const int arraySize = 5;
    Pointer<int, arraySize> pArray = new int[arraySize];
    
    // Assign values to the array elements
    for (int i = 0; i < arraySize; ++i) {
        pArray[i] = i * 10;
    }
    
    // Display the array elements
    std::cout << "Array elements:" << std::endl;
    for (int i = 0; i < arraySize; ++i) {
        std::cout << "pArray[" << i << "] = " << pArray[i] << std::endl;
    }

    return 0;
}
