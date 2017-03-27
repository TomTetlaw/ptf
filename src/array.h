#ifndef __ARRAY_H__
#define __ARRAY_H__

struct Default_Allocator {
	void *alloc(int n) { 
		return new char[n];
	}

	void dealloc(void *ptr) {
		delete[] ptr;
	}
};

template<typename T, typename A = Default_Allocator>
struct Array {
	A allocator;
	T *data = nullptr;
	int size = 0;
	int num = 0;

	~Array();
	void ensure_size(int new_size);
	void append(const T &value);
	T *alloc();
	T &operator[](int index) { return data[index]; }

	T *first() { return &data[0]; }
};

template<typename T, typename A>
Array<T, A>::~Array() {
	allocator.dealloc(data);
}

template<typename T, typename A>
void Array<T, A>::ensure_size(int new_size) {
	if (size >= new_size) {
		return;
	}

	T *old_data = data;
	data = (T *)allocator.alloc(new_size * sizeof(T));
	memset(data, 0, sizeof(T) * new_size);
	for (int i = 0; i < num; i++) {
		data[i] = old_data[i];
	}
	allocator.dealloc(old_data);
	size = new_size;
}

template<typename T, typename A>
void Array<T, A>::append(const T &value) {
	ensure_size(num + 1);

	data[num] = value;
	num += 1;
}

template<typename T, typename A>
T *Array<T, A>::alloc() {
	ensure_size(num + 1);
	T *out = &data[num];
	num += 1;
	return out;
}

#define For(x) \
	auto it = x.first(); \
	for(int it_index = 0; it_index < x.num; it_index++, it = &x[it_index]) \

#define For2(x, name) \
	auto name = x.first(); \
	for(int name##_index = 0; name##_index < x.num; name##_index++, name = &x[name##_index]) \


#endif