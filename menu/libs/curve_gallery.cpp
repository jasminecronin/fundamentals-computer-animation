#include "curve_gallery.h"
//#include <algorithm>

#include "imgui/curve.h"

namespace io {

using values_t = MemoizeFunction::values_t;

MemoizeFunction::MemoizeFunction(values_t values) : m_values(values) {
  m_delta = 1.f / m_values.size();
}

float MemoizeFunction::evaluate(float t) const {
  int index = std::floor(t * m_values.size());
  if (index >= m_values.size())
    return m_values.back();

  return m_values[index];
}

MemoizeFunction memoizeFunction(int steps,
                                std::function<float(float)> const &func) {
  values_t values(steps, 0.f);
  float delta = 1.f / steps;

  for (int i = 0; i < steps; ++i) {
    values[i] = func(i * delta);
  }

  return {values};
}

float const *MemoizeFunction::data() const { return m_values.data(); }

int MemoizeFunction::bucketCount() const { return m_values.size(); }

using points_t = Curve::points_t;

Curve::Curve() : m_points(4) {}

Curve::Curve(points_t points) : m_points(points) {}

void Curve::load(points_t points) { m_points = points; }

ImVec2 *Curve::data() { return m_points.data(); }

float Curve::evaluate(float t) const {
  return ImGui::CurveValue(t, size(), data());
}

float Curve::evaluateSmooth(float t) const {
  return ImGui::CurveValueSmooth(t, size(), data());
}

ImVec2 const *Curve::data() const { return m_points.data(); }

int Curve::size() const { return m_points.size(); }

using curves_data_t = CurveGallery::curves_data_t;

int CurveGallery::create(int numberOfPoints) {
  CurveData data;

  auto points = points_t(numberOfPoints);
  float const delta = 1.f / (numberOfPoints - 1);
  int counter = 0;
  using namespace std;
  // studip
  std::transform(begin(points), end(points), begin(points), [&](auto) {
    auto pos = counter * delta;
    return ImVec2(pos, pos);
  });

  data.curve = Curve(points);

  data.memoized = io::memoizeFunction(numberOfPoints, [&](float t) {
    // return curve.evaluate(t);
    return data.curve.evaluateSmooth(t);
  });

  int index = m_curvesData.size();
  m_curvesData.push_back(data);

  return index;
}

int CurveGallery::create(std::string title, int numberOfPoints) {
  CurveData data;
  data.title = title;

  auto points = points_t(numberOfPoints);
  float const delta = 1.f / (numberOfPoints - 1);
  int counter = 0;
  using namespace std;
  // studip, need to initalize here, because ImGui::Curve won't initalize
  // without viewing the frame and extra input to the function. Idea:change code
  // to make better
  std::transform(begin(points), end(points), begin(points), [&](auto) {
    auto pos = (counter++) * delta;
    return ImVec2(pos, pos);
  });

  data.curve = Curve(points);

  data.memoized = io::memoizeFunction(numberOfPoints, [&](float t) {
    // return curve.evaluate(t);
    return data.curve.evaluateSmooth(t);
  });

  int index = m_curvesData.size();
  m_curvesData.push_back(data);

  return index;
}

float CurveGallery::evaluate(int index, float t) const {
  if (!isValidIndex(index))
    return -1.f;

  return m_curvesData[index].curve.evaluate(t);
}

float CurveGallery::evaluateSmooth(int index, float t) const {
  if (!isValidIndex(index))
    return -1.f;

  return m_curvesData[index].curve.evaluateSmooth(t);
}

float CurveGallery::evaluateFast(int index, float t) const {
  if (!isValidIndex(index))
    return -1.f;

  return m_curvesData[index].memoized.evaluate(t);
}

bool CurveGallery::isValidIndex(int index) const {
  return index >= 0 && index < size();
}

int CurveGallery::size() const { return m_curvesData.size(); }

curves_data_t &CurveGallery::curvesData() { return m_curvesData; }

curves_data_t const &CurveGallery::curvesData() const { return m_curvesData; }

} // namespace io

namespace ImGui {
void Gallery(io::CurveGallery &gallery) {
  for (auto &data : gallery.curvesData()) {
    ImGui::PushID(data.title.c_str());

    ImGui::Separator();
    bool changed = false;
    if (ImGui::Curve(data.title.c_str(), data.size, data.item, data.curve)) {
      changed = true;
    }

    int steps = data.memoized.bucketCount();
    if (ImGui::SliderInt("buckets", &steps, 2, 1000)) {
      changed = true;
    }

    if (changed) {
      // update memoized
      auto &curve = data.curve;
      data.memoized = io::memoizeFunction(steps, [&](float t) {
        // return curve.evaluate(t);
        return curve.evaluateSmooth(t);
      });
    }

    ImGui::PlotHistogram(data.title.c_str(), data.memoized.data(),
                         data.memoized.bucketCount(), 0, NULL, 0.0f, 1.0f,
                         data.size);

    ImGui::PopID();
  }
}

int Curve(char const *title, ImVec2 size, int &item, io::Curve &curve) {
  return ImGui::Curve(title, size, item, curve.size(), curve.data());
}
} // namespace ImGui
