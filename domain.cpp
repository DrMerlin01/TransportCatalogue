#include "domain.h"

namespace transport_catalogue {
	using namespace std;

	namespace domain {
		size_t StopsPairHasher::operator()(const pair<const Stop*, const Stop*>& stops_pair) const {
			return hasher(stops_pair.first) + hasher(stops_pair.second) * 7;
		} 

		bool operator==(const Stop& lhs, const Stop& rhs) {
			return lhs.name == rhs.name;
		}

		bool operator!=(const Stop& lhs, const Stop& rhs) {
			return !(lhs == rhs);
		}

		bool operator<(const Stop& lhs, const Stop& rhs) {
			return lhs.name < rhs.name;
		}

		bool operator<=(const Stop& lhs, const Stop& rhs) {
			return !(lhs > rhs);
		}

		bool operator>(const Stop& lhs, const Stop& rhs) {
			return rhs < lhs;
		}

		bool operator>=(const Stop& lhs, const Stop& rhs) {
			return !(lhs < rhs);
		}

		bool operator<(const Bus& lhs, const Bus& rhs) {
			return lhs.name < rhs.name;
		}
	}
}