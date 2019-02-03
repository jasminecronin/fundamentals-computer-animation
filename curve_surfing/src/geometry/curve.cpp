#include "curve.h"

namespace math {
namespace geometry {

Curve::Curve() {}

Curve::Curve(std::vector<Vec3f> points) : m_points(points) {}
Curve::Curve(std::vector<Vec3f> points, bool isClosedCurve)
    : m_points(points), m_isClosedCurve(isClosedCurve) {}

Vec3f Curve::operator[](int idx) const { return m_points[idx]; }

Vec3f &Curve::operator[](int idx) { return m_points[idx]; }

Vec3f Curve::front() const { return m_points.front(); }

Vec3f Curve::back() const { return m_points.back(); }

void Curve::setPoint(int idx, Vec3f point) { m_points.at(idx) = point; }

void Curve::addMidpointToSegment(int idx) {
  Vec3f a = m_points[idx];
  size_t neighbourIdx = idx + 1;
  neighbourIdx = (neighbourIdx >= m_points.size() ? 0 : neighbourIdx);
  Vec3f b = m_points[neighbourIdx];

  Vec3f c = lerp(a, b, 0.5f);
  // TODO: std::advance wasn't working?
  m_points.insert(std::begin(m_points) + neighbourIdx, c);
}

void Curve::removePoint(int idx) { m_points.erase(std::begin(m_points) + idx); }

size_t Curve::pointCount() const { return m_points.size(); }

Vec3f const *Curve::data() const { return m_points.data(); }

std::vector<Vec3f> const &Curve::points() const { return m_points; }

bool Curve::isClosed() const { return m_isClosedCurve; }

void Curve::setClosed(bool closed) { m_isClosedCurve = closed; }

// Free functions
float length(Curve const &curve) {
  float l = 0.f;
  int numLineSegments = curve.pointCount() - 1;

  for (int i = 0; i < numLineSegments; ++i) {
    l += distance(curve[i], curve[i + 1]);
  }

  if (curve.isClosed()) // do wrap around segment
  {
    l += distance(curve.front(), curve.back());
  }

  return l;
}

Curve cubicSubdivideCurve(Curve curve, int numberOfSubdivisionSteps) {
  for (int iter = 0; iter < numberOfSubdivisionSteps; ++iter) {
    curve = repeatedAveraging(curve, 2);
  }
  return curve;
}

Curve repeatedAveraging(Curve const &curve, int numberOfAveragingSteps) {
  return {repeatedAveraging(curve.points(), numberOfAveragingSteps)};
}

Points repeatedAveraging(Points points, int numberOfAveragingSteps) {
  Points tmp;
  tmp.reserve(points.size() * 2);
  int numLineSegments = points.size() - 1;

  for (int i = 0; i < numLineSegments; ++i) {
    Vec3f mid = lerp(points[i], points[i + 1], 0.5f);
    tmp.push_back(points[i]);
    tmp.push_back(mid);
  }
  Vec3f mid = lerp(points.back(), points.front(), 0.5f);
  tmp.push_back(points.back());
  tmp.push_back(mid);

  points = tmp;

  numLineSegments = points.size() - 1;
  for (int avgItr = 0; avgItr < numberOfAveragingSteps; ++avgItr) {
    for (int i = 0; i < numLineSegments; ++i) {
      Vec3f mid = lerp(points[i], points[i + 1], 0.5);
      tmp[i] = mid;
    }
    Vec3f mid = lerp(points.back(), points.front(), 0.5f);
    tmp.back() = mid;

    points.swap(tmp);
  }

  return points;
}

} // namespace geometry
} // namespace math
