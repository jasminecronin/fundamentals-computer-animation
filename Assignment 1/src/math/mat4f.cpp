#include "mat4f.h"

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <iterator>

namespace math {

Mat4f::Mat4f() {
  handle tmp(new array16f);
  m_ptr = std::move(tmp);
}

Mat4f::Mat4f(float fillValue) {
  handle tmp(new array16f);
  tmp->fill(fillValue);
  m_ptr = std::move(tmp);
}

Mat4f::Mat4f(std::initializer_list<float> list) {
  assert(list.size() == NUMBER_ELEMENTS);
  handle tmp(new array16f);
  std::copy_n(list.begin(), NUMBER_ELEMENTS, tmp->begin());
  m_ptr = std::move(tmp);
}

Mat4f::Mat4f(Mat4f &&other) : m_ptr(std::move(other.m_ptr)) {}

Mat4f::Mat4f(Mat4f const &other) {
  using std::begin;
  using std::end;
  handle tmp(new array16f);
  std::copy_n(begin(other), NUMBER_ELEMENTS, tmp->begin());
  m_ptr = std::move(tmp);
}

Mat4f::~Mat4f() {}

void Mat4f::fill(float t) { m_ptr->fill(t); }

Mat4f &Mat4f::operator=(Mat4f const &other) {
  if (this != &other) {
    Mat4f tmp(other);
    *this = std::move(tmp);
  }
  return *this;
}

Mat4f &Mat4f::operator=(Mat4f &&other) {
  m_ptr = std::move(other.m_ptr);
  return *this;
}

float &Mat4f::operator()(int row, int column) {
  return (*m_ptr)[rowMajorIndex(row, column)];
}

float &Mat4f::operator[](int element) { return (*m_ptr)[element]; }

float Mat4f::operator()(int row, int column) const {
  return (*m_ptr)[rowMajorIndex(row, column)];
}

float Mat4f::operator[](int element) const { return (*m_ptr)[element]; }

float &Mat4f::at(int row, int column) {
  return (*m_ptr).at(rowMajorIndex(row, column));
}

float &Mat4f::at(int element) { return (*m_ptr).at(element); }

float Mat4f::at(int row, int column) const {
  return (*m_ptr).at(rowMajorIndex(row, column));
}

float Mat4f::at(int element) const { return (*m_ptr).at(element); }

int Mat4f::rowMajorIndex(int row, int column) const {
  return row * DIM + column;
}

bool Mat4f::isValidDim(int idx) const { return idx >= 0 && idx < DIM; }

bool Mat4f::isValidElement(int idx) const {
  return idx >= 0 && idx < NUMBER_ELEMENTS;
}

float *Mat4f::data() { return m_ptr->data(); }

float const *Mat4f::data() const { return m_ptr->data(); }

Mat4f identity() {
  return {
      1.f, 0.f, 0.f, 0.f, // row 0
      0.f, 1.f, 0.f, 0.f, // row 1
      0.f, 0.f, 1.f, 0.f, // row 2
      0.f, 0.f, 0.f, 1.f  // row 3
  };
}

Mat4f transposed(Mat4f mat) {
  //	0	1	2	3
  // ----------------
  // 0|	0		1		2		3
  // 1| 4		5		6		7
  // 2| 8		9		10	11
  // 3| 12	13	14	15

  std::swap(mat[1], mat[4]);
  std::swap(mat[2], mat[8]);
  std::swap(mat[3], mat[12]);
  std::swap(mat[6], mat[9]);
  std::swap(mat[7], mat[13]);
  std::swap(mat[11], mat[14]);
  return mat;
}

Mat4f operator+(Mat4f const &lhs, Mat4f rhs) {
  using std::begin;
  using std::end;

  std::transform(begin(lhs), end(lhs), begin(rhs), begin(rhs),
                 std::plus<float>());
  return rhs;
}

Mat4f operator*(Mat4f const &lhs, Mat4f const &rhs) {
  Mat4f result;

  float element = 0.f;
  for (int i = 0; i < Mat4f::DIM; ++i) {
    for (int j = 0; j < Mat4f::DIM; ++j) {
      element = 0.f;
      for (int k = 0; k < Mat4f::DIM; ++k) {
        element += lhs(i, k) * rhs(k, j);
      }
      result(i, j) = element;
    }
  }

  return result;
}

Mat4f operator*(float s, Mat4f rhs) {
  using std::begin;
  using std::end;
  std::transform(begin(rhs), end(rhs), begin(rhs),
                 [=](float f) { return s * f; });
  return rhs;
}

Mat4f operator*(Mat4f lhs, float s) {
  using std::begin;
  using std::end;
  std::transform(begin(lhs), end(lhs), begin(lhs),
                 [=](float f) { return s * f; });
  return lhs;
}

Mat4f::array16f::iterator Mat4f::begin() { return m_ptr->begin(); }
Mat4f::array16f::iterator Mat4f::end() { return m_ptr->end(); }
Mat4f::array16f::const_iterator Mat4f::begin() const { return m_ptr->begin(); }
Mat4f::array16f::const_iterator Mat4f::end() const { return m_ptr->end(); }

std::ostream &operator<<(std::ostream &out, Mat4f const &mat) {
  using std::begin;
  using std::end;
  std::ostream_iterator<float> outIter(out, " ");
  std::copy(begin(mat), end(mat), outIter);
  return out;
}

} // namespace math
