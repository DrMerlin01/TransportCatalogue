#pragma once

#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

#include "graph.h"
#include "router.h"
#include "domain.h"
#include "transport_catalogue.h"

namespace transport_router {
	struct RoutingSettings final {
		int bus_wait_time;
		double bus_velocity;
	};

	class TransportRouter {
	public:
		TransportRouter(const transport_catalogue::TransportCatalogue& db);

		void SetSettings(const RoutingSettings& settings);

		void SetVertexCount(const size_t vertex_count);

		void CreateRouter();

		RoutingSettings GetSettings() const;

		size_t GetVertexCount() const;

		std::optional<transport_catalogue::response::Route> GetRouteResponse(const std::string_view from, const std::string_view to) const;

	private:
		const transport_catalogue::TransportCatalogue& db_;
		graph::DirectedWeightedGraph<transport_catalogue::route::Road> graph_;
		RoutingSettings settings_;
		std::unique_ptr<graph::Router<transport_catalogue::route::Road>> router_;
		std::unordered_map<std::string_view, graph::VertexId> name_id_;
		std::unordered_map<graph::VertexId, std::string_view> id_name_;
		std::unordered_set<std::string> names_;

		void FillingGraphWithStops();

		void FillingGraphWithBuses();

		template<typename Iterator>
		inline void AddEdgesGraph(Iterator begin, Iterator end, const std::string_view name) {
			for (auto from_id = begin; from_id != end; ++from_id) {
				const graph::VertexId from = CreateVertexId(std::string{*from_id});
				for (auto to_id = std::next(from_id); to_id != end; ++to_id) {
					const graph::VertexId to = CreateVertexId(std::string{*to_id});
					double sum_minutes = 0;
					int span_count = 0;
					for (auto it = from_id; it != to_id; ++it) {
						sum_minutes += CalculateWeightEdge(*it, *(it + 1));
						++span_count;
					}
					sum_minutes += settings_.bus_wait_time;
					graph_.AddEdge({ from, to, transport_catalogue::route::Road{std::string(name), span_count, sum_minutes} });
				}
			}
		}

		void AddEdgeGraph(const transport_catalogue::domain::Stop& stop);

		graph::VertexId CreateVertexId(const std::string& stop);

		double CalculateWeightEdge(const std::string_view from, const std::string_view to) const;
	};
}