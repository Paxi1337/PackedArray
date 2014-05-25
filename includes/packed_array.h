#ifndef PACKED_ARRAY_H
#define PACKED_ARRAY_H

#ifndef size_t
#define size_t unsigned int
#endif

#include <cstdlib>

template<typename T>
class PackedArray {
public:
	PackedArray();
	explicit PackedArray(size_t arr);
	~PackedArray();

	void wrapArr(T* arr, size_t size);
	//void provideArr(size_t size);

	void add(const T& element);

private:
	PackedArray(const PackedArray& other);
	PackedArray& operator=(const PackedArray& other);

	size_t mSize;
	T* mArr;
	bool mAllocated;
};


template <typename T>
PackedArray<T>::PackedArray() : mSize(0), mArr(nullptr), mAllocated(false) {}

template <typename T>
PackedArray<T>::PackedArray(size_t size) : mSize(size), mArr(new T[size]), mAllocated(true) {}

template <typename T>
PackedArray<T>::~PackedArray() {
	if(mAllocated)
		delete[] mArr;

	mArr = nullptr;
}

template <typename T>
void PackedArray<T>::wrapArr(T* arr, size_t size) {

	mArr = arr;
	mSize = size;

	mAllocated = false;
}

template <typename T>
void add(const T& element) {

}




#endif
