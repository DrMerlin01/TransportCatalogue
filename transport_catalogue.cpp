#include "transport_catalogue.h"
#include <algorithm>
#include <utility>
#include <iostream>
#include <unordered_set>

using namespace std;

void TransportCatalogue::AddBus(const string& name, const std::vector<string_view>& bus_stops, bool is_circular) {
	buses_.push_back({ move(name), is_circular, bus_stops });
	bus_by_name_[buses_.back().name] = &buses_.back();
}

void TransportCatalogue::AddStop(const std::string& name, const Coordinates coords) {
	bus_stops_.push_back({ move(name), coords });
	stop_by_name_[bus_stops_.back().name] = &bus_stops_.back();
}

double TransportCatalogue::GetDistanceBetweenStops(std::string_view from, std::string_view to) const {
	const auto from_stop = GetStop(from);
	const auto to_stop = GetStop(to);
	if (from_stop == nullptr || to_stop == nullptr) {
		return 0.;
	}

	return ComputeDistance(from_stop->coordinates, to_stop->coordinates);
}

const TransportCatalogue::Stop* TransportCatalogue::GetStop(string_view name) const {
	const auto stop = stop_by_name_.find(name);
    if (stop != stop_by_name_.end()) {
		return stop->second;
	} else {
		return nullptr;
	}
}

const TransportCatalogue::Bus* TransportCatalogue::GetBus(string_view name) const {
    const auto bus = bus_by_name_.find(name);
    if (bus != bus_by_name_.end()) {
		return bus->second;
	} else {
		return nullptr;
	}
}

InfoOnRoute TransportCatalogue::GetInfoOnRoute(string_view name) const {
	const auto bus = GetBus(name);
	if (bus == nullptr) {
		return {};
	}

    unordered_set<string_view> seen_stops;
	size_t count_uniq_stops = 0;
    double lenght_route = 0.;

	for (size_t i = 0; i < bus->route.size() - 1; ++i) {
		lenght_route += GetDistanceBetweenStops(bus->route[i], bus->route[i + 1]);  
        if (seen_stops.count(bus->route[i]) == 0) {
            ++count_uniq_stops;
            seen_stops.insert(bus->route[i]);
        }
	}

	return InfoOnRoute{ bus->route.size(), count_uniq_stops, lenght_route };
}