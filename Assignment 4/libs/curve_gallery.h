#pragma once

#include <functional>
#include <string>
#include <vector>

#include "imgui/imgui.h"

namespace io {

class MemoizeFunction {
public:
  using values_t = std::vector<float>;

public:
  MemoizeFunction() = default;
  MemoizeFunction(values_t values);

  float evaluate(float t) const;

  float const *data() const;
  int bucketCount() const;

private:
  float m_delta = 0.f;
  values_t m_values;
};

MemoizeFunction memoizeFunction(int steps,
                                std::function<float(float)> const &func);

class Curve {
public:
  using points_t = std::vector<ImVec2>;

public:
  Curve();
  Curve(points_t points);

  void load(points_t points);
  ImVec2 *data();

  float evaluate(float t) const;
  float evaluateSmooth(float t) const;

  ImVec2 const *data() const;
  int size() const;

private:
  points_t m_points;
};

class CurveGallery {
public:
  struct CurveData {
    std::string title = std::string("default");
    ImVec2 size = ImVec2(600, 200);
    int item = 0;
    Curve curve;
    MemoizeFunction memoized;
  };

  using curves_data_t = std::vector<CurveData>;

public:
  int create(int numberOfPoints);
  int create(std::string title, int numberOfPoints);

  float evaluate(int index, float t) const;
  float evaluateFast(int index, float t) const;
  float evaluateSmooth(int index, float t) const;

  bool isValidIndex(int) const;
  int size() const;

  curves_data_t &curvesData();
  curves_data_t const &curvesData() const;

private:
  curves_data_t m_curvesData;
};

} // namespace io

namespace ImGui {
void Gallery(io::CurveGallery &gallery);
int Curve(char const *title, ImVec2 size, int &item, io::Curve &curve);
} // namespace ImGui
