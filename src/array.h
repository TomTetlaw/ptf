#ifndef __ARRAY_H__
#define __ARRAY_H__

template<typename T>
struct Array {
	T *data = nullptr;
	int size = 0;
	int num = 0;

	~Array();
	void ensure_size(int new_size);
	void append(const T &value);
	T &operator[](int index) { return data[index]; }

	T *first() { return &data[0]; }
};

template<typename T>
Array<T>::~Array() {
	delete[] data;
}

template<typename T>
void Array<T>::ensure_size(int new_size) {
	if (size >= new_size) {
		return;
	}

	T *old_data = data;
	data = new T[new_size];
	memset(data, 0, sizeof(T) * new_size);
	for (int i = 0; i < num; i++) {
		data[i] = old_data[i];
	}
	delete[] old_data;
	size = new_size;
}

template<typename T>
void Array<T>::append(const T &value) {
	ensure_size(num + 1);

	data[num] = value;
	num += 1;
}

#define For(x) \
	auto it = x.first(); \
	for(int it_index = 0; it_index < x.num; it_index++, it = &x[it_index]) \

#endif