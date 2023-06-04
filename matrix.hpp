#pragma once

#include <initializer_list>
#include <iomanip>
#include <sstream>
#include <ostream>
#include <vector>

template <typename T>
linalg::Matrix<T>::Matrix(const size_t& rows, const size_t& columns) {
	if ((rows == 0 && columns != 0) || (rows != 0 && columns == 0 )) {
		throw EmptyError();
	}

	T* tmp_ptr = reinterpret_cast<T*>(operator new(sizeof(T) * rows * columns));
	T* current_ptr = tmp_ptr;

	try {
		for (; current_ptr != tmp_ptr + rows * columns; ++current_ptr) {
			new(current_ptr) T();
		}
	}
	catch (...){
		for (; current_ptr >= tmp_ptr; --current_ptr) {
			current_ptr->~T();
		}
		delete reinterpret_cast<void*>(tmp_ptr);
		throw MemError();
	}

	m_ptr = tmp_ptr;
	m_rows = rows;
	m_columns = columns;
	m_capacity = rows * columns;
}

template <typename T>
linalg::Matrix<T>::Matrix(const Matrix<T>& obj) {
	linalg::Matrix<T>::copy_HIDE(obj);
}

template <typename T>
linalg::Matrix<T>::Matrix(Matrix<T>&& obj) noexcept {
	linalg::Matrix<T>::swap(obj);
}
template <typename T> template <typename T1>
linalg::Matrix<T>::Matrix(const Matrix<T1>& obj) {
	linalg::Matrix<T>::copy_HIDE(obj);
}
template <typename T> template <typename T1>
linalg::Matrix<T>::Matrix(const size_t& rows, const size_t& columns, const T1& def_val) {
	if ((rows == 0 && columns != 0) || (rows != 0 && columns == 0)) {
		throw EmptyError();
	}

	T* tmp_ptr = reinterpret_cast<T*>(operator new(sizeof(T) * rows * columns));
	T* current_ptr = tmp_ptr;

	try {
		for (; current_ptr != tmp_ptr + rows * columns; ++current_ptr) {
			new(current_ptr) T(def_val);
		}
	}
	catch (...) {
		for (; current_ptr >= tmp_ptr; --current_ptr) {
			current_ptr->~T();
		}
		delete reinterpret_cast<void*>(tmp_ptr);
		throw MemError();
	}

	m_ptr = tmp_ptr;
	m_rows = rows;
	m_columns = columns;
	m_capacity = rows * columns;
}

template <typename T> template <typename T1>
linalg::Matrix<T>::Matrix(std::initializer_list<T1> lst) {
	if (lst.size() == 0) {
		return;
	}
	T* tmp_ptr = reinterpret_cast<T*>(operator new(sizeof(T) * lst.size()));
	T* current_ptr = tmp_ptr;

	try {
		using iter_t = typename std::initializer_list<T1>::iterator;
		for (iter_t it = lst.begin(); it != lst.end(); ++it, ++current_ptr) {
			new(current_ptr) T(*it);
		}

	}
	catch(...) {
		for (; current_ptr >= tmp_ptr; --current_ptr) {
			current_ptr->~T();
		}
		delete reinterpret_cast<void*>(tmp_ptr);
		throw MemError();
	}

	m_ptr = tmp_ptr;
	m_capacity = lst.size();
	m_rows = lst.size();
	m_columns = 1;
}

template <typename T> template <typename T1>
linalg::Matrix<T>::Matrix(std::initializer_list<std::initializer_list <T1>> lst) {
	if (lst.size() == 0) {
		return;
	}
	//using iter_t = typename std::initializer_list<std::initializer_list <T1>>::iterator;
	size_t weight = 0;
	for (auto iter_lst : lst) {
		if (iter_lst.size() > weight) {
			weight = iter_lst.size();
		}
	}
	
	size_t i = 0, check;
	T* tmp_ptr = reinterpret_cast<T*>(operator new(sizeof(T) * lst.size() * weight));
	try {
		for (const auto& iter_lst : lst) {
			check = 0;
			for (const auto& elem : iter_lst) {
				new(tmp_ptr + i * weight + check) T(elem);
				++check; // Нужно чтобы доинициализорвали элементы, если длина вложенных листов разная.
			}

			// Доинициализируем дефолтом.
			for (; check < weight; ++check) {
				new(tmp_ptr + i * weight + check) T();
			}
			++i;
		}
	}
	catch (...) {
		for (i = i * weight + check; i >= 0; --i) {
			(tmp_ptr + i)->~T();
		}
		delete reinterpret_cast<void*>(tmp_ptr);
		throw MemError();
	}

	m_ptr = tmp_ptr;
	m_capacity = lst.size() * weight;
	m_rows = lst.size();
	m_columns = weight;
}

template <typename T>
linalg::Matrix<T>::~Matrix() {
	if (empty() != 1) {
		T* current_ptr = m_ptr + m_rows * m_columns - 1;
		for (; current_ptr >= m_ptr; --current_ptr) {
			current_ptr->~T();
		}
	}
	delete reinterpret_cast<void*>(m_ptr);
}

template <typename T>
void linalg::Matrix<T>::reshape(const size_t& rows, const size_t& columns) {
	if ((rows == 0 && columns != 0) || (rows != 0 && columns == 0)) {
		throw EmptyError();
	}
	if (m_rows * m_columns == rows * columns) {
		m_rows = rows;
		m_columns = columns;
	}
	else {
		throw SizeError();
	}
}

template <typename T>
void linalg::Matrix<T>::reserve(const size_t& capacity) {
	if (capacity <= m_capacity) {
		return;
	}

	T* tmp_ptr = new T[capacity];
	delete m_ptr;
	m_ptr = tmp_ptr;
	m_capacity = capacity;
}

template <typename T>
void linalg::Matrix<T>::shrink_to_fit() {
	if (m_capacity == m_rows * m_columns) return;
	*this = Matrix<T>(*this);
}

template <typename T>
void linalg::Matrix<T>::swap(Matrix<T>& obj) noexcept{
	std::swap(m_rows, obj.m_rows);
	std::swap(m_columns, obj.m_columns);
	std::swap(m_capacity, obj.m_capacity);
	std::swap(m_ptr, obj.m_ptr);
}

template <typename T>
void linalg::Matrix<T>::clear() noexcept{
	if (empty() != 1) {
		T* current_ptr = m_ptr + m_rows * m_columns - 1;
		for (; current_ptr >= m_ptr; --current_ptr) {
			current_ptr->~T();
		}
		m_rows = 0;
		m_columns = 0;
	}
}
template <typename T> template <typename T1>
void linalg::Matrix<T>::copy_HIDE(const Matrix<T1>& obj) {
	if (obj.empty() == 1) return;
	T* tmp_ptr = reinterpret_cast<T*>(operator new(sizeof(T) * obj.rows() * obj.columns()));
	size_t i=0, j;

	try {
		for (; i < obj.rows(); ++i) {
			for (j = 0; j < obj.columns(); ++j) {
				new(tmp_ptr + i * obj.columns() + j) T(obj(i, j));
			}
		}
	}
	catch (...) {
		T* current_ptr = tmp_ptr + i * obj.columns() + j;
		for (; current_ptr >= tmp_ptr; --current_ptr) {
			current_ptr->~T();
		}
		delete reinterpret_cast<void*>(tmp_ptr);
		throw MemError();
	}

	m_ptr = tmp_ptr;
	m_rows = obj.rows();
	m_columns = obj.columns();
	m_capacity = obj.rows() * obj.columns();
}

template <typename T>
linalg::Matrix<T>& linalg::Matrix<T>::operator=(const Matrix<T>& obj) {
	if (this == &obj) {
		return *this;
	}
	return operator=<T>(obj);
}

template <typename T> template <typename T1>
linalg::Matrix<T>& linalg::Matrix<T>::operator=(const Matrix<T1>& obj) {
	if (m_capacity < obj.rows() * obj.columns()) {
		return *this = Matrix(obj); 
	}
	for (size_t i = 0; i < obj.rows(); ++i) {
		for (size_t j = 0; j < obj.columns(); ++j) {
			(*this)(i, j) = T(obj(i, j));
		}
	}

	m_rows = obj.rows();
	m_columns = obj.columns();
	return *this;
}

template <typename T>
linalg::Matrix<T>& linalg::Matrix<T>::operator=(Matrix&& obj) noexcept{
	if (&obj == this) {
		return *this;
	}
	swap(obj);
	return *this;
}

template <typename T>
int length_obj(T& obj) {
	std::ostringstream str_out;
	str_out << obj;
	return str_out.str().size();
}

template <typename T>
std::ostream& operator<<(std::ostream& out, const linalg::Matrix<T>& obj) {
	if (obj.empty() == 1) {
		out << "Matrix is empty!";
	}
	std::vector <int> max_in_col(obj.columns(), 0);
	for (size_t i = 0; i < obj.rows(); ++i) {
		for (size_t j = 0; j < obj.columns(); ++j) {
			int lng_obj = length_obj(obj(i, j));
			if (max_in_col[j] < lng_obj) {
				max_in_col[j] = lng_obj;
			}
		}
	}

	for (size_t i = 0; i < obj.rows(); ++i) {
		for (size_t j = 0; j < obj.columns(); ++j) {	
			if (j == 0) {
				if (i == 0) {
					out << "|";
				}
				else {
					out << "\n|";					
				}
				out << std::setw(max_in_col[j]) << obj(i, j);
			}
			else if (j == obj.columns() - 1) {
				out << std::setw(max_in_col[j] + 2) << obj(i, j);
				out << "|";
			}
			else {
				out << std::setw(max_in_col[j] + 2) << obj(i, j);
			}
		}
	}


	return out;
}

template <typename T>
T& linalg::Matrix<T>::operator()(const size_t& nmb_r, const size_t& nmb_c) noexcept {
	return m_ptr[nmb_r * m_columns + nmb_c];
}

template <typename T>
const T& linalg::Matrix<T>::operator()(const size_t& nmb_r, const size_t& nmb_c) const noexcept {
	return m_ptr[nmb_r * m_columns + nmb_c];
}

template <typename T>
T& linalg::Matrix<T>::at(const size_t& nmb_r, const size_t& nmb_c) {
	if (nmb_r * m_columns + nmb_c > m_rows * m_columns) {
		throw RangeError();
	}
	return (*this)(nmb_r, nmb_c);
}

template <typename T>
const T& linalg::Matrix<T>::at(const size_t& nmb_r, const size_t& nmb_c) const {
	if (nmb_r * m_columns + nmb_c > m_rows * m_columns) {
		throw RangeError();
	}
	return (*this)(nmb_r, nmb_c);
}

template <typename T> template <typename T1>
linalg::Matrix<T>& linalg::Matrix<T>::operator+=(const Matrix<T1>& obj) {
	if (empty() == 1 || obj.empty() == 1) {
		throw EmptyError();
	}
	if (m_rows != obj.rows() || m_columns != obj.columns()) {
		throw SizeError();
	}
	for (size_t i = 0; i < m_rows; ++i) {
		for (size_t j = 0; j < m_columns; ++j) {
			(*this)(i, j) += obj(i, j);
		}
	}
	return *this;
}

template <typename T> template <typename T1>
linalg::Matrix<T>& linalg::Matrix<T>::operator-=(const Matrix<T1>& obj) {
	if (empty() == 1 || obj.empty() == 1) {
		throw EmptyError();
	}
	if (m_rows != obj.rows() || m_columns != obj.columns()) {
		throw SizeError();
	}
	for (size_t i = 0; i < m_rows; ++i) {
		for (size_t j = 0; j < m_columns; ++j) {
			(*this)(i, j) -= obj(i, j);
		}
	}
	return *this;
}

template <typename T> template <typename T1>
linalg::Matrix<T>& linalg::Matrix<T>::operator*=(const Matrix<T1>& obj) {
	if (empty() == 1 || obj.empty() == 1) {
		throw EmptyError();
	}
	if (m_columns != obj.rows()) {
		throw SizeError();
	}
	Matrix tmp_m(m_rows, obj.columns());
	for (size_t i = 0; i < m_rows; ++i) {
		for (size_t j = 0; j < obj.columns(); ++j) {
			for (size_t nmb = 0; nmb < m_columns; ++nmb) {
				tmp_m(i, j) += (*this)(i, nmb) * obj(nmb, j);
			}
		}
	}
	*this = std::move(tmp_m);
	return *this;
}

template <typename T> template <typename T1>
linalg::Matrix<T>& linalg::Matrix<T>::operator*=(const T1& value) {
	if (empty() == 1) {
		throw EmptyError();
	}
	for (size_t i = 0; i < m_rows; ++i) {
		for (size_t j = 0; j < m_columns; ++j) {
			(*this)(i, j) *= value;
		}
	}
	return *this;
}

template <typename T, typename T1>
auto linalg::operator+(const linalg::Matrix<T>& obj1, const linalg::Matrix<T1>& obj2) -> linalg::Matrix<decltype(T() + T1())> {
	return linalg::Matrix<decltype(T() + T1())>(obj1) += obj2;
}

template<typename T, typename T1>
auto linalg::operator-(const linalg::Matrix<T>& obj1, const linalg::Matrix<T1>& obj2) -> linalg::Matrix<decltype(T() - T1())> {
	return linalg::Matrix<decltype(T() + T1())>(obj1) -= obj2;
}

template<typename T, typename T1>
auto linalg::operator*(const linalg::Matrix<T>& obj1, const linalg::Matrix<T1>& obj2)-> linalg::Matrix<decltype(T()* T1())> {
	return linalg::Matrix<decltype(T() + T1())>(obj1) *= obj2;
}

template<typename T, typename T1>
auto linalg::operator*(const T1& value, const linalg::Matrix<T>& obj) -> linalg::Matrix<decltype(T()* T1())> {
	return linalg::Matrix<decltype(T()* T1())>(obj) *= value;
}

template<typename T, typename T1>
auto linalg::operator*(const linalg::Matrix<T>& obj, const T1& value) -> linalg::Matrix<decltype(T()* T1())> {
	return linalg::Matrix<decltype(T()* T1())>(obj) *= value;
}