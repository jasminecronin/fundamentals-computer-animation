#pragma once

#include <string>

#include "curve.h"

namespace math {
namespace geometry {

void saveCurveToFile(math::geometry::Curve const &curve,
                     std::string const &filePath);

math::geometry::Curve loadCurveFromFile(std::string const &filePath);

math::geometry::Curve loadCurveFrom_OBJ_File(std::string const &filePath);

} // namespace geometry
} // namespace math
