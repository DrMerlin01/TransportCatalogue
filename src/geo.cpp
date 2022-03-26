#define _USE_MATH_DEFINES

#include "../inc/geo.h"

#include <cmath>

namespace geo {
	using namespace std;

	double ComputeDistance(Coordinates from, Coordinates to) {
		const double dr = M_PI / 180.0;

		return acos(sin(from.lat * dr) * sin(to.lat * dr)
			   + cos(from.lat * dr) * cos(to.lat * dr) * cos(abs(from.lng - to.lng) * dr))
			   * 6371000;
	}
} // namespace geo