#ifndef SAFE_VECTOR_H
#define SAFE_VECTOR_H

#include <vector>
#include <mutex>

template <typename T>
class SafeVector
{
public:
    void push(const T& val){
        mut.lock();
        safe_vector.push_back(val);
        mut.unlock();
    }

    T back(){
        mut.lock();
        auto data = safe_vector.back();
        mut.unlock();
        return data;
    }

    void pop(){
        mut.lock();
        safe_vector.pop_back();
        mut.unlock();
    }

    size_t len(void){
        mut.lock();
        size_t size = safe_vector.size();
        mut.unlock();
        return size;
    }

    T& operator[](std::size_t idx){
        return safe_vector[idx];
    }
private:
    std::vector<T> safe_vector;
    mutable std::mutex mut;
};

#endif