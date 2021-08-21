#pragma once

#include <tuple>
#include <string>
#include <vector>
#include <variant>
#include <set>
#include <string_view>
#include <functional>
#include "geo.h"

namespace transport_catalogue {
	namespace domain {
		struct InfoOnRoute final {
			size_t total_bus_stops;
			size_t unique_bus_stops;
			double lenght_route;
			double curvature;
		};

		struct Bus final {
			std::string name;
			bool is_roundtrip;
			std::vector<std::string_view> stops;
		};

		struct Stop final {
			std::string name;
			geo::Coordinates coordinates;
		};

		struct StopsPairHasher {
			size_t operator()(const std::pair<const Stop*, const Stop*>& stops_pair) const;

			std::hash<const void*> hasher;
		};

		bool operator==(const Stop& lhs, const Stop& rhs);

		bool operator!=(const Stop& lhs, const Stop& rhs);

		bool operator<(const Stop& lhs, const Stop& rhs);

		bool operator<=(const Stop& lhs, const Stop& rhs);

		bool operator>(const Stop& lhs, const Stop& rhs);

		bool operator>=(const Stop& lhs, const Stop& rhs);

		bool operator<(const Bus& lhs, const Bus& rhs);
	}

	namespace response {
		struct Stop final {
			std::string_view name;
			std::set<std::string_view> buses_on_stop;
		};

		struct Bus final {
			std::string_view name;
			domain::InfoOnRoute info;
		};

		struct Wait final {
			std::string type;
			std::string stop_name;
			int time;
		};

		struct Travel final {
			std::string type;
			std::string bus;
			int span_count;
			double time;
		};

		struct Route final {
			double total_time;
			std::vector<std::variant<Wait, Travel>> items;
		};
	}

	namespace request {
		struct Request {
			std::string type;
			std::string name;
			std::string from;
			std::string to;
		};

		struct Identification final : Request {
			int id;
		};

		struct Stop final : Request {
			geo::Coordinates coordinates;
			std::unordered_map<std::string, int> stop_to_dist;
		};

		struct Bus final : Request {
			bool is_roundtrip;
			std::vector<std::string> bus_stops;
		};
	}

	namespace route {
		struct Road {
			std::string name;
			int span_count;
			double minutes;
		};

		Road operator+(const Road& lhs, const Road& rhs);

		bool operator<(const Road& lhs, const Road& rhs);

		bool operator>(const Road& lhs, const Road& rhs);
	}

	using Requests = std::vector<request::Identification>;
}