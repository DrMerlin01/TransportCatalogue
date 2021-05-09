#pragma once

#include "geo.h"
#include <string>
#include <string_view>
#include <deque>
#include <unordered_map>
#include <vector>
#include <set>
#include <tuple>

namespace transport_catalogue {
	namespace content {
		struct InfoOnRoute {
			size_t count_bus_stop;
			size_t count_unique_bus_stop;
			double lenght_route;
			double curvature;
		};

		struct Bus {
			std::string name;
			bool is_circular;
			std::vector<std::string_view> route;
		};

		struct Stop {
			std::string name;
			geo::Coordinates coordinates;
		};

		struct StopsPairHasher {
			size_t operator()(const std::pair<const Stop*, const Stop*>& stops_pair) const {
				return hasher(stops_pair.first) + hasher(stops_pair.second) * 7;
			}

			std::hash<const void*> hasher;
		};
	}

	class TransportCatalogue {
	private:
		std::deque<content::Bus> buses_;
		std::deque<content::Stop> bus_stops_;
		std::unordered_map<std::string_view, const content::Stop*> stop_by_name_;
		std::unordered_map<std::string_view, const content::Bus*> bus_by_name_;
		std::unordered_map<std::string_view, std::set<std::string_view>> buses_by_stop_name_;
		std::unordered_map<std::pair<const content::Stop*, const content::Stop*>, double, content::StopsPairHasher> stops_to_dist_;

	public:
		void AddBus(const std::string& name, const std::vector<std::string_view>& bus_stops, bool is_circular);

		void AddStop(const std::string& name, const geo::Coordinates coords);

		void SetDistanceBetweenStops(std::string_view from, std::string_view to, const int distance);

		const content::Stop* GetStop(std::string_view name) const;

		const content::Bus* GetBus(std::string_view name) const;

		std::set<std::string_view> GetBusesThroughStop(std::string_view stop_name) const;

		double GetActualDistanceBetweenStops(std::string_view from, std::string_view to) const;

		double GetGeographicDistanceBetweenStops(std::string_view from, std::string_view to) const;

		content::InfoOnRoute GetInfoOnRoute(std::string_view name) const;
	};
}