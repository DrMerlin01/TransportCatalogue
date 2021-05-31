#include "domain.h"

namespace transport_catalogue {
	using namespace std;

	namespace domain {
		size_t StopsPairHasher::operator()(const pair<const Stop*, const Stop*>& stops_pair) const {
			return hasher(stops_pair.first) + hasher(stops_pair.second) * 7;
		} 
	}
}