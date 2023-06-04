#pragma once

namespace linalg {
	template <typename T = double> class Matrix {
	public:
		Matrix() noexcept = default;
		Matrix(const size_t& rows, const size_t& columns = 1);
		Matrix(const Matrix& obj);
		Matrix(Matrix&& obj) noexcept;
		template <typename T1> Matrix(const size_t& rows, const size_t& columns, const T1& def_value);
		template <typename T1> Matrix(const Matrix<T1>& obj);
		template <typename T1> Matrix(std::initializer_list<T1> lst);
		template <typename T1> Matrix(std::initializer_list<std::initializer_list <T1>> lst);

		Matrix& operator=(const Matrix& obj);
		Matrix& operator=(Matrix&& obj) noexcept;
		template <typename T1> Matrix& operator=(const Matrix<T1>& obj);

		T& operator()(const size_t& nmb_r, const size_t& nmb_c) noexcept;
		const T& operator()(const size_t& nmb_r, const size_t& nmb_c) const noexcept;
		T& at(const size_t& nmb_r, const size_t& nmb_c);
		const T& at(const size_t& nmb_r, const size_t& nmb_c) const;

		template <typename T1> Matrix& operator+=(const Matrix<T1>& obj);
		template <typename T1> Matrix& operator-=(const Matrix<T1>& obj);
		template <typename T1> Matrix& operator*=(const Matrix<T1>& obj);
		template <typename T1> Matrix& operator*=(const T1& value);

		size_t rows() const noexcept { return m_rows; }
		size_t columns() const noexcept { return m_columns; }
		size_t capacity() const noexcept { return m_capacity; }

		bool empty() const noexcept { return (m_rows || m_columns) == 0; }
		void reshape(const size_t& rows, const size_t& columns);
		void reserve(const size_t& n);
		void shrink_to_fit();
		void clear() noexcept;
		void swap(Matrix& matrix) noexcept;

		~Matrix();

	private:
		T* m_ptr = nullptr;
		size_t m_rows = 0;
		size_t m_columns = 0;
		size_t m_capacity = 0;
	private:
		template <typename T1> void copy_HIDE(const Matrix<T1>& obj);
	};

	template <typename T, typename T1> auto operator+(const Matrix<T>& obj1, const Matrix<T1>& obj2)->Matrix<decltype(T() + T1())>;
	template <typename T, typename T1> auto operator-(const Matrix<T>& obj1, const Matrix<T1>& obj2)->Matrix<decltype(T() - T1())>;
	template <typename T, typename T1> auto operator*(const Matrix<T>& obj1, const Matrix<T1>& obj2)->Matrix<decltype(T() * T1())>;
	template <typename T, typename T1> auto operator*(const T1& value, const Matrix<T>& obj)->Matrix<decltype(T() * T1())>;
	template <typename T, typename T1> auto operator*(const Matrix<T>& obj, const T1& value)->Matrix<decltype(T() * T1())>;

	struct MatrixException: public std::exception {
		MatrixException(const char* str): std::exception(str) {}
	};
	struct MemError : public MatrixException {
		MemError(): MatrixException("Memory allocation error!") {}
	};
	struct SizeError : public MatrixException {
		SizeError(): MatrixException("Incorrect sizes of objects for action!") {}
	};
	struct RangeError : public MatrixException {
		RangeError() : MatrixException("Out of range error!") {}
	};
	struct EmptyError : public MatrixException {
		EmptyError() : MatrixException("One of the objects is empty!") {}
	};
}

#include "matrix.hpp"