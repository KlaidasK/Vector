#pragma once
#ifndef VHEADER_H
#define VHEADER_H

#include <vector>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <string>

using std::cout;
using std::endl;
using std::max;


class Timer {
private:
	using hrClock = std::chrono::high_resolution_clock;
	std::chrono::time_point<hrClock> start;
public:
	Timer() :start{ hrClock::now() } {}
	void reset() {
		start = hrClock::now();
	}
	double elapsed()const {
		std::chrono::duration<double> diffs = hrClock::now() - start;
		return diffs.count();
	}
};




template <class T> class Vector{
public: // interfeisas
	typedef T* iterator;
	typedef const T* const_iterator;
	typedef size_t size_type;
	typedef T value_type;

	// rule of three
	//Member functions
	Vector() { create(); } //Default konstruktorius
	explicit Vector(size_type n, const T& t = T{}) { create(n, t); }
	Vector(const Vector& v) { create(v.begin(), v.end()); }
	Vector& operator=(const Vector& t) { //Kopijavimo
		if (this != &t) {
			this->data = t.data;
			this->avail = t.avail;
			this->limit = t.limit;
		}
		return *this;
	};
	~Vector() { data = avail = limit = nullptr; } //Destruktorius

	//Element access
	T& operator[](size_type i) { if (i < 0 || size() <= i) throw std::out_of_range{ "Vector::operator[]" }; return data[i]; } //Operatoriaus [] perdengimas
	const T& operator[](size_type i) const { if (i < 0 || size() <= i) throw std::out_of_range{ "Vector::operator[]" }; return data[i]; }
	inline T& at(int i) {
		if (i < 0 || size() <= i) throw std::out_of_range{ "Vector::operator[]" };
		return data[i];
	}
	inline T& at(int& i) {
		if (i < 0 || size() <= i) throw std::out_of_range{ "Vector::operator[]" };
		return data[i];
	}
	T& front() { return data[0]; }
	T& back() { return data[size()-1]; }
	T* Data() { if (size() == 0) return nullptr; else return data; }

	//Iterators
	iterator begin() { return data; }
	const_iterator begin() const { return data; }
	iterator end() { return data + size(); }
	const_iterator end() const { return data + size(); }

	//Capacity
	size_type size() const { return avail - data; }
	bool empty()const { return size()==0; }
	size_type capacity() const { return limit - data; }
	void shrink_to_fit() { limit=avail ; }

	void reserve(size_type n) {
		if (n > capacity()) {
			if (empty() == 1) {
				data = alloc.allocate(n); // grąžina ptr į array pirmą elementą
				limit = data + n;
				avail = data;
			}
			else {
				limit = data + n;
			}
		}
		else if(n<capacity())
			if (empty() == 1) {
				size_type new_size = 2 + size();
				data = alloc.allocate(n); // grąžina ptr į array pirmą elementą
				limit = avail = data + n; // sustato rodykles į vietas
				std::uninitialized_fill(data, limit, T{}); // inicializuoja elementus val reikšme
			}
			else {
				size_type new_size = 2 + size();
				limit = data + new_size;
			}

		}

	//Modifiers

	void clear() { if (empty()!=1) data=avail=limit=nullptr; }
	void erase(iterator it) {
		int index = it - data;
		if (index < 0 || index >= size()) {
			throw std::exception("Erase - Index out of range!");
		}
		for (int i = index; i < size() - 1; ++i)
		{
			data[i] = data[i + 1];
		}
		--limit;
		--avail;
	}
	
	void erase(iterator first, iterator last) {
		size_type diff = last - first;
		iterator it = first;
		iterator end = last-1;
		if (diff==1) erase(it);
		else {
			for (int i = 0; i < diff; i++) {
				erase(it);
			}
		}
	};

	void push_back(const T& t) {
		if (avail == limit)
			grow();
		unchecked_append(t);
	}
	void pop_back() {
		iterator it = avail;
		traits_t::destroy(alloc, --it);
		avail = limit = it;
	};

	void resize(size_type n) {
		if (n < size()) {
			size_type d = size() - n;
			iterator it = avail;
			for (int i = size(); i > n; i--) {
				traits_t::destroy(alloc, --it);
				avail = limit = it;
			}
		}
		else if (n > size()) {
			
			if (empty() == 1) { reserve(n); }
			else {


				//std::cout << "even" << std::endl;
				size_type new_size = n;
				// išskirti naują vietą ir perkopijuoti egzistuojančius elementus
				iterator new_data = alloc.allocate(new_size);

				//iterator new_avail = std::uninitialized_copy(data, avail, new_data);
				for (int i = 0; i < size(); i++) new_data[i] = data[i];
				for (int i = size(); i < n; i++) new_data[i] = T{};
				uncreate();
				//avail = new_avail;

				// reset'int rodykles į naujai išskirtą vietą
				data = new_data;
				avail = data+new_size;
				limit = data + new_size;
			}

		}
		else if (n == 0) { return; }
		else if (n < 0) { throw std::exception("Erase - Index out of range!"); }

	};


	void insert(iterator it, T val) {
		int index = it - data;
		if (index < 0 || index >= size()) {
			throw std::exception("Insert - Index out of range!");
		}
		for (int i = size() - 1; i >= index; i--) {
			data[i + 1] = data[i];
		}

		// Insert the element at the specified index
		data[index] = val;

		// Increment the size of the array
		++limit;
		++avail;

	}

	void swap(Vector& other) {
		std::swap(data, other.data);
		std::swap(limit, other.limit);
		std::swap(avail, other.avail);
	}





	void print()
	{
		std::cout << "[ ";
		for (int i = 0; i<size(); i++) {
			std::cout << data[i] << ' ';
		}
		std::cout << "]\n";
	}

private:
	iterator data; // kaip buvo anksčiau
	iterator avail; // pirmasis po paskutiniojo sukonstruoto Vector elementas
	iterator limit; // pirmasis po paskutiniojo Vector elementas
	// atminties išskyrimo valdymui
	std::allocator<T> alloc; // objektas atminties valdymui
	using traits_t = std::allocator_traits<decltype(alloc)>;
	// išskirti atmintį (array) ir ją inicializuoti
	void create() {
		data = avail = limit = nullptr;
	};
	void create(size_type n, const T& val) {
		data = alloc.allocate(n); // grąžina ptr į array pirmą elementą
		limit = avail = data + n; // sustato rodykles į vietas
		std::uninitialized_fill(data, limit, val); // inicializuoja elementus val reikšme
	};
	void create(const_iterator i, const_iterator j) {
		data = alloc.allocate(j - i); // išskirti vietos j-i elementams
		limit = avail = std::uninitialized_copy(i, j, data); // nukopijuoja elementus iš intervalo
	};
	// sunaikinti elementus array ir atlaisvinti atmintį
	void uncreate() {
		if (data) {
			// sunaikinti (atbuline tvarka) sukonstruotus elementus
			iterator it = avail;
			while (it != data)
				traits_t::destroy(alloc, --it);
			// atlaisvinti išskirtą atmintį. Turi būti data != nullptr
			alloc.deallocate(data, limit - data);
		}
		// reset'inam pointer'iuss - Vector'ius tuščias
		data = limit = avail = nullptr;
	};
	// pagalbinės funkcijos push_back realizacijai
	
	void unchecked_append(const T& val) {
		//data[size()-1] = val;
		//avail++;
		traits_t::construct(alloc, avail++, val);
	};

	void grow() {
		// dvigubai daugiau vietos, nei buvo
		size_type new_size = max(2 * (limit - data), ptrdiff_t(1));
		// išskirti naują vietą ir perkopijuoti egzistuojančius elementus
		iterator new_data = alloc.allocate(new_size);
		iterator new_avail = std::uninitialized_copy(data, avail, new_data);
		// atlaisvinti seną vietą
		uncreate();
		// reset'int rodykles į naujai išskirtą vietą
		data = new_data;
		avail = new_avail;
		limit = data + new_size;
	};
	




};

class Test {
	std::vector<int> t1;
	Vector<int> t2;

public:

	void Push_back_Testavimas() {
		int numDashes = 32;
		std::string line(numDashes, '-');
		cout << "## Push_back testavimas ##" << endl;
		std::cout << line << std::endl;
		//std::vector<int> t1;
		for (int i = 0; i < 7; i++) t1.push_back(i * 2 + 1);
		//Vector<int> t2;
		for (int i = 0; i < 7; i++) t2.push_back(i * 2 + 1);


		cout << "std::vector" << endl;
		for (auto& i : t1) cout << i << " ";
		cout << endl;
		std::cout << line << std::endl;
		cout << "Vector klase" << endl;
		t2.print();
		std::cout << line << std::endl;
		cout << endl;
	}

	void Erase_Testavimas() {
		int numDashes = 32;
		std::string line(numDashes, '-');
		cout << "## erase testavimas ##" << endl;
		t1.erase(t1.begin() + 3);
		t2.erase(t2.begin() + 3);

		cout << "std::vector" << endl;
		for (auto& i : t1) cout << i << " ";
		cout << endl;
		std::cout << line << std::endl;
		cout << "Vector klase" << endl;
		t2.print();
		std::cout << line << std::endl;
		cout << endl;
	}

	void Erase_with_range_Testavimas() {
		int numDashes = 32;
		std::string line(numDashes, '-');
		cout << "## erase range testavimas ##" << endl;

		t1.erase(t1.begin() + 1, t1.begin() + 3);
		t2.erase(t2.begin() + 1, t2.begin() + 3);

		cout << "std::vector" << endl;
		for (auto& i : t1) cout << i << " ";
		cout << endl;
		std::cout << line << std::endl;
		cout << "Vector klase" << endl;
		t2.print();
		std::cout << line << std::endl;
		cout << endl;

	}

	void Insert_Testavimas() {
		int numDashes = 32;
		std::string line(numDashes, '-');
		cout << "## Insert testavimas ##" << endl;
		t1.insert(t1.begin() + 3,10);
		t2.insert(t2.begin() + 3,10);

		cout << "std::vector" << endl;
		for (auto& i : t1) cout << i << " ";
		cout << endl;
		std::cout << line << std::endl;
		cout << "Vector klase" << endl;
		t2.print();
		std::cout << line << std::endl;
		cout << endl;
	}

	void Reserve_Testavimas() {
		int numDashes = 32;
		std::string line(numDashes, '-');
		cout << "## Reserve testavimas ##" << endl;
		t1.clear();
		t2.clear();
		t1.reserve(10);
		t2.reserve(10);

		cout << "std::vector" << endl;
		for (auto& i : t1) cout << i << " ";
		cout << endl;
		cout << t1.size()<<" " << t1.capacity() << endl;
		std::cout << line << std::endl;
		cout << "Vector klase" << endl;
		t2.print();
		cout << t2.size()<<" " << t2.capacity() << endl;
		std::cout << line << std::endl;
		cout << endl;
	}









};

#endif