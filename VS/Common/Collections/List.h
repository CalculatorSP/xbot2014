template <typename T>
class List
{
public:
    int capacity;
    int size;
    T* data;
    
    List()
    :capacity(10), size(0)
    {
        data = new T[capacity];
    }
    
    ~List()
    {
        delete[] data;
    }
    
    T operator[](int i)
    {
        return data[i];
    }
    
    const T& operator[](int i) const
    {
        return data[i];
    }
    
    void add(T val)
    {
        if (size == capacity)
        {
            capacity *= 2;
            T* newData = new T[capacity];
            for (int i = 0; i < size; i++)
                newData[i] = data[i];
            delete[] data;
            data = newData;
        }
        data[size++] = val;
    }
    
    void remove(int index)
    {
        for (int i = index; i < size; i++)
        {
            data[i] = data[i+1];
        }
        size--;
    }
};