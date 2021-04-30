#include "transport_catalogue.h"
#include <algorithm>
#include <utility>
#include <iostream>

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
	if (stop_by_name_.count(name)) {
		return stop_by_name_.at(name);
	} else {
		return nullptr;
	}
}

const TransportCatalogue::Bus* TransportCatalogue::GetBus(string_view name) const {
    if (bus_by_name_.count(name)) {
		return bus_by_name_.at(name);
	} else {
		return nullptr;
	}
}

InfoOnRoute TransportCatalogue::GetInfoOnRoute(string_view name) const {
	const auto bus = GetBus(name);
	if (bus == nullptr) {
		return {};
	}

	double lenght_route = 0.;
	size_t count_uniq_stops = bus->route.size() - 1;
	if (bus->is_circular) {
		count_uniq_stops = bus->route.size() / 2 + 1;
	}

	for (size_t i = 0; i < bus->route.size() - 1; ++i) {
		lenght_route += GetDistanceBetweenStops(bus->route[i], bus->route[i + 1]);  
	}

	return InfoOnRoute{ bus->route.size(), count_uniq_stops, lenght_route };
}