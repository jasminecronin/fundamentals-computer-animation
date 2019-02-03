#pragma once

#include <iosfwd>
#include <vector>

#include "vec3f.h"

namespace math {
namespace geometry {

using Points = std::vector<Vec3f>;

class Curve {
public:
  Curve();
  Curve(Points points);
  Curve(Points points, bool isClosedCurve);

  Vec3f operator[](int idx) const;
  Vec3f &operator[](int idx);
  Vec3f front() const;
  Vec3f back() const;

  void setPoint(int idx, Vec3f point);
  void addMidpointToSegment(int idx);
  void removePoint(int idx);

  size_t pointCount() const;
  Vec3f const *data() const;
  Points const &points() const;

  bool isClosed() const;
  void setClosed(bool);

private:
  Points m_points;
  bool m_isClosedCurve = false;
};

// Free functions
float length(Curve const &curve);

Curve cubicSubdivideCurve(Curve curve, int numberOfSubdivisionSteps);
Curve repeatedAveraging(Curve const &curve, int numberOfAveragingSteps);
Points repeatedAveraging(Points points, int numberOfAveragingSteps);

} // namespace geometry
} // namespace math
