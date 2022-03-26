#include "../inc/transport_router.h"

using namespace std;

namespace transport_router {
	TransportRouter::TransportRouter(const transport_catalogue::TransportCatalogue& db)
		: db_(db) {
	}

	void TransportRouter::FillingGraphWithStops() {
		for (const auto& stop : db_.GetStops()) {
			AddEdgeGraph(stop);
		}
	}

	void TransportRouter::FillingGraphWithBuses() {
		for (const auto& bus : db_.GetBuses()) {
			const auto& stops = bus.stops;
			const string_view name = bus.name;
			if (bus.is_roundtrip) {
				AddEdgesGraph(stops.begin(), stops.end(), name);
			} else {
				AddEdgesGraph(stops.begin(), next(stops.begin(), stops.size() / 2 + 1), name);
				AddEdgesGraph(next(stops.begin(), stops.size() / 2), stops.end(), name);
			}
		}
	}

	void TransportRouter::CreateRouter() {
		FillingGraphWithStops();
		FillingGraphWithBuses();

		static graph::Router<transport_catalogue::route::Road> router(graph_);
		router_ = make_unique<graph::Router<transport_catalogue::route::Road>>(router);
	}

	void TransportRouter::SetVertexCount(const size_t vertex_count) {
		if (vertex_count > 0) {
			graph_ = graph::DirectedWeightedGraph<transport_catalogue::route::Road>(vertex_count);
		}
	}

	size_t TransportRouter::GetVertexCount() const {
		return graph_.GetVertexCount();
	}

	void TransportRouter::SetSettings(const RoutingSettings& settings) {
		settings_.bus_wait_time = settings.bus_wait_time;
		settings_.bus_velocity = settings.bus_velocity;
	}

	RoutingSettings TransportRouter::GetSettings() const {
		return settings_;
	}

	std::optional<transport_catalogue::response::Route> TransportRouter::GetRouteResponse(const string_view from, const string_view to) const {
		transport_catalogue::response::Route route_response;
		const graph::VertexId start = name_id_.at(from);
		const graph::VertexId finish = name_id_.at(to);

		const auto& info = router_->BuildRoute(start, finish);
		if (!info) {
			return nullopt;
		}

		route_response.total_time = info->weight.minutes;
		const auto& edges = info->edges;
		for (auto it = edges.begin(); it != edges.end(); ++it) {
			graph::Edge<transport_catalogue::route::Road> edge = graph_.GetEdge(*it);

			transport_catalogue::response::Wait wait;
			wait.type = "Wait"s;
			wait.stop_name = string(id_name_.at(edge.from));
			wait.time = settings_.bus_wait_time;
			route_response.items.push_back(wait);

			transport_catalogue::response::Travel travel;
			travel.type = "Bus"s;
			travel.bus = edge.weight.name;
			travel.span_count = edge.weight.span_count;
			travel.time = edge.weight.minutes - settings_.bus_wait_time;
			route_response.items.push_back(travel);
		}

		return route_response;
	}

	void TransportRouter::AddEdgeGraph(const transport_catalogue::domain::Stop& stop) {
		graph::VertexId from, to;
		from = to = CreateVertexId(stop.name);
		graph_.AddEdge({ from, to, transport_catalogue::route::Road{} });
	}

	graph::VertexId TransportRouter::CreateVertexId(const string& stop) {
		if (name_id_.count(stop) != 0) {
			return name_id_.at(stop);
		}
		const auto& [it, _] = names_.emplace(stop);
		const graph::VertexId id = name_id_[*it] = name_id_.size();
		id_name_[id] = *it;
		return id;
	}

	double TransportRouter::CalculateWeightEdge(const string_view from, const string_view to) const {
		return 60.0 * db_.GetActualDistanceBetweenStops(from, to) / (1000.0 * settings_.bus_velocity);
	}
}