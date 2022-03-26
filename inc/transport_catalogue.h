#pragma once

#include "geo.h"
#include "domain.h"

#include <string>
#include <string_view>
#include <deque>
#include <unordered_map>
#include <vector>
#include <set>
#include <tuple>

namespace transport_catalogue {
	class TransportCatalogue final {
	private:
		std::deque<domain::Bus> buses_;
		std::deque<domain::Stop> bus_stops_;
		std::unordered_map<std::string_view, const domain::Stop*> stop_by_name_;
		std::unordered_map<std::string_view, const domain::Bus*> bus_by_name_;
		std::unordered_map<std::string_view, std::set<std::string_view>> buses_by_stop_name_;
		std::unordered_map<std::pair<const domain::Stop*, const domain::Stop*>, double, domain::StopsPairHasher> stops_to_dist_;

	public:
		void AddBus(const std::string& name, const std::vector<std::string_view>& bus_stops, bool is_roundtrip);

		void AddStop(const std::string& name, const geo::Coordinates coords);

		void SetDistanceBetweenStops(std::string_view from, std::string_view to, const int distance);

		const domain::Stop* GetStop(std::string_view name) const;

		const domain::Bus* GetBus(std::string_view name) const;

		const std::deque<domain::Stop> GetStops() const;

		const std::deque<domain::Bus> GetBuses() const;

		size_t GetStopsCount() const;

		size_t GetBusesCount() const;

		const std::unordered_map<std::pair<const domain::Stop*, const domain::Stop*>, double, domain::StopsPairHasher>& GetStopsDistance() const;

		std::set<std::string_view> GetBusesThroughStop(std::string_view stop_name) const;

		double GetActualDistanceBetweenStops(std::string_view from, std::string_view to) const;

		double GetGeographicDistanceBetweenStops(std::string_view from, std::string_view to) const;

		domain::InfoOnRoute GetInfoOnRoute(std::string_view name) const;
	};
}