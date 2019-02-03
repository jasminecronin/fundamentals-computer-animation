#pragma once

#include <array>
#include <initializer_list>
#include <iosfwd>
#include <memory>

namespace math {

class Mat4f {
public:
  enum Size { DIM = 4, NUMBER_ELEMENTS = 16 };

  using array16f = std::array<float, NUMBER_ELEMENTS>;
  using handle = std::unique_ptr<array16f>;

  explicit Mat4f();
  explicit Mat4f(float fillValue);

  Mat4f(std::initializer_list<float> list);
  Mat4f(Mat4f &&);
  Mat4f(Mat4f const &);

  ~Mat4f();

  void fill(float t);

  Mat4f &operator=(Mat4f const &other);
  Mat4f &operator=(Mat4f &&other);

  float &operator()(int row, int column);
  float &operator[](int element);
  float operator()(int row, int column) const;
  float operator[](int element) const;

  float &at(int row, int column);
  float &at(int element);
  float at(int row, int column) const;
  float at(int element) const;

  bool isValidDim(int idx) const;
  bool isValidElement(int idx) const;

  float *data();
  float const *data() const;

  int rowMajorIndex(int row, int column) const;

  array16f::iterator begin();
  array16f::iterator end();
  array16f::const_iterator begin() const;
  array16f::const_iterator end() const;

private:
  handle m_ptr = nullptr;
};

Mat4f identity();
Mat4f transposed(Mat4f mat);
Mat4f operator+(Mat4f const &lhs, Mat4f const &rhs);
Mat4f operator-(Mat4f const &lhs, Mat4f const &rhs);
Mat4f operator*(Mat4f const &lhs, Mat4f const &rhs);
Mat4f operator*(float s, Mat4f const &rhs);
Mat4f operator*(Mat4f const &lhs, float s);

std::ostream &operator<<(std::ostream &out, Mat4f const &mat);

} // namespace math
