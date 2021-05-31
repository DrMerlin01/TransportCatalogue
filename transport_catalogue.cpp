#include "transport_catalogue.h"
#include <algorithm>
#include <utility>
#include <unordered_set>

using namespace std;

namespace transport_catalogue {
	void TransportCatalogue::AddBus(const string& name, const vector<string_view>& bus_stops, bool is_roundtrip) {
		buses_.push_back({ move(name), is_roundtrip, bus_stops });
		bus_by_name_[buses_.back().name] = &buses_.back();

		for (const string_view stop : bus_stops) {
			buses_by_stop_name_[stop].insert(buses_.back().name);
		}
	}

	void TransportCatalogue::AddStop(const string& name, const geo::Coordinates coords) {
		bus_stops_.push_back({ move(name), coords });
		stop_by_name_[bus_stops_.back().name] = &bus_stops_.back();
	}

	void TransportCatalogue::SetDistanceBetweenStops(string_view from, string_view to, const int distance) {
		const auto from_stop = GetStop(from);
		const auto to_stop = GetStop(to);
		if (from_stop == nullptr || to_stop == nullptr) {
			return;
		}

		stops_to_dist_[{from_stop, to_stop}] = distance;
	}

	double TransportCatalogue::GetActualDistanceBetweenStops(string_view from, string_view to) const {
		const auto from_stop = GetStop(from);
		const auto to_stop = GetStop(to);
		if (from_stop == nullptr || to_stop == nullptr) {
			return 0.;
		}

		auto stops_pair = stops_to_dist_.find({from_stop, to_stop});
		if (stops_pair != stops_to_dist_.end()) {
			return stops_pair->second;
		} else {
			stops_pair = stops_to_dist_.find({to_stop, from_stop});
			if (stops_pair != stops_to_dist_.end()) {
				return stops_pair->second;
			} else {
				return 0.;
			}
		}
	}

	double TransportCatalogue::GetGeographicDistanceBetweenStops(string_view from, string_view to) const {
		const auto from_stop = GetStop(from);
		const auto to_stop = GetStop(to);
		if (from_stop == nullptr || to_stop == nullptr) {
			return 0.;
		}

		return geo::ComputeDistance(from_stop->coordinates, to_stop->coordinates);
	}

	const domain::Stop* TransportCatalogue::GetStop(string_view name) const {
		const auto stop = stop_by_name_.find(name);
		if (stop != stop_by_name_.end()) {
			return stop->second;
		} else {
			return nullptr;
		}
	}

	const domain::Bus* TransportCatalogue::GetBus(string_view name) const {
		const auto bus = bus_by_name_.find(name);
		if (bus != bus_by_name_.end()) {
			return bus->second;
		} else {
			return nullptr;
		}
	}

	set<string_view> TransportCatalogue::GetBusesThroughStop(string_view stop_name) const {
		const auto it = buses_by_stop_name_.find(stop_name);
		if (it != buses_by_stop_name_.end()) {
			return it->second;
		} else {
			return {};
		}
	}

	domain::InfoOnRoute TransportCatalogue::GetInfoOnRoute(string_view name) const {
		const auto bus = GetBus(name);
		if (bus == nullptr) {
			return {};
		}

		unordered_set<string_view> seen_stops;
		size_t count_uniq_stops = 0;
		double actual_lenght_route = 0.;
		double geographic_lenght_route = 0.;

		for (size_t i = 0; i < bus->stops.size() - 1; ++i) {
			actual_lenght_route += GetActualDistanceBetweenStops(bus->stops[i], bus->stops[i + 1]);
			geographic_lenght_route += GetGeographicDistanceBetweenStops(bus->stops[i], bus->stops[i + 1]);
			if (seen_stops.count(bus->stops[i]) == 0) {
				++count_uniq_stops;
				seen_stops.insert(bus->stops[i]);
			}
		}
		const double curvature = actual_lenght_route / geographic_lenght_route;

		return { bus->stops.size(), count_uniq_stops, actual_lenght_route, curvature };
	}
}