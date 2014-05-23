#ifndef PACKED_ARRAY_H
#define PACKED_ARRAY_H

#include <array>

template<typename T, int n>
class PackedArray {
public:
	PackedArray();
	~PackedArray();

private:
	PackedArray(const PackedArray& other);
	PackedArray& operator=(const PackedArray& other);

	std::array<T, n> mArray;
};



#endif
