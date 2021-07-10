#include "request_handler.h"

namespace transport_catalogue {
	using namespace std;

	RequestHandler::RequestHandler(TransportCatalogue& db, renderer::MapRenderer& renderer) 
		: db_(db)
		, renderer_(renderer) {
	}

	optional<response::Stop> RequestHandler::GetStopResponse(const string_view& stop_name) const {
		const domain::Stop* stop_from_db = db_.GetStop(stop_name);
		if (stop_from_db != nullptr) {
			return response::Stop{stop_from_db->name, db_.GetBusesThroughStop(stop_name)};
		} else {
			return nullopt;
		}
	}

	optional<response::Bus> RequestHandler::GetBusResponse(const string_view& bus_name) const {
		const domain::Bus* bus_from_db = db_.GetBus(bus_name);
		if (bus_from_db != nullptr) {
			return response::Bus{bus_from_db->name, db_.GetInfoOnRoute(bus_name)};
		} else {
			return nullopt;
		}
	}

	svg::Document RequestHandler::RenderMap() const {
		svg::Document document;

		const vector<domain::Bus> buses = GetNonEmptyBuses();
		const vector<domain::Stop> stops = GetStopsOnRoutes(buses);
		const vector<geo::Coordinates> stop_coords = GetStopsCoordsOnRoutes(stops);

		const renderer::RenderSettings& settings = renderer_.GetRenderSettings();
		const renderer::SphereProjector projector{ stop_coords.begin(), stop_coords.end(), settings.width, settings.height, settings.padding };
		const vector<svg::Color>& colors = settings.color_palette;

		const auto route_to_projectored_points = GetRouteToProjectoredPoints(buses, projector);

		RenderRoute(document, route_to_projectored_points, colors);
		RenderRouteName(document, route_to_projectored_points, colors);
		RenderStopCircles(document, stops, projector);
		RenderStopNames(document, stops, projector);

		return document;
	}

	void RequestHandler::RenderRoute(svg::Document& document, const map<domain::Bus, vector<svg::Point>>& route_to_projectored_points, const vector<svg::Color>& colors) const {
		size_t i = 0;

		for (const auto& [bus, stop_points] : route_to_projectored_points) {
			renderer_.RenderRoute(document, stop_points, colors[i % colors.size()]);
			++i;
		}
	}

	void RequestHandler::RenderRouteName(svg::Document& document, const std::map<domain::Bus, std::vector<svg::Point>>& route_to_projectored_points, const std::vector<svg::Color>& colors) const {
		size_t i = 0;

		for (const auto& [bus, stops_points] : route_to_projectored_points) {
			renderer_.RenderRouteName(document, stops_points.front(), colors[i % colors.size()], bus.name);
			if (!bus.is_roundtrip && bus.stops.front() != bus.stops[bus.stops.size() / 2]) {
				renderer_.RenderRouteName(document, stops_points[bus.stops.size() / 2], colors[i % colors.size()], bus.name);
			}
			++i;
		}
	}

	void RequestHandler::RenderStopCircles(svg::Document& document, const std::vector<domain::Stop>& stops, const renderer::SphereProjector& projector) const {
		for (const domain::Stop& stop : stops) {
			const svg::Point projected_point = projector(stop.coordinates);
			renderer_.RenderStopCircle(document, projected_point);
		}
	}

	void RequestHandler::RenderStopNames(svg::Document& document, const std::vector<domain::Stop>& stops, const renderer::SphereProjector& projector) const {
		for (const domain::Stop& stop : stops) {
			const svg::Point projected_point = projector(stop.coordinates);
			renderer_.RenderStopName(document, projected_point, stop.name);
		}
	} 

	vector<domain::Bus> RequestHandler::GetNonEmptyBuses() const {
		const deque<domain::Bus>& buses = db_.GetBuses();
		vector<domain::Bus> non_empty_buses;
		non_empty_buses.reserve(buses.size());

		for (const domain::Bus& bus : buses) {
			if (!bus.stops.empty()) {
				non_empty_buses.push_back(bus);
			}
		}
		sort(non_empty_buses.begin(), non_empty_buses.end());

		return non_empty_buses;
	}

	vector<domain::Stop> RequestHandler::GetStopsOnRoutes(const vector<domain::Bus>& buses) const {
		vector<domain::Stop> unique_stops;

		for (const domain::Bus& bus : buses) {
			for (const string_view stop : bus.stops) {
				unique_stops.push_back(*db_.GetStop(stop));
			}
		}
		sort(unique_stops.begin(), unique_stops.end());
		const auto last = unique(unique_stops.begin(), unique_stops.end());
		unique_stops.erase(last, unique_stops.end());

		return unique_stops;
	}

	vector<geo::Coordinates> RequestHandler::GetStopsCoordsOnRoutes(const vector<domain::Stop>& stops) const {
		vector<geo::Coordinates> coordinates;

		for (const domain::Stop& stop : stops) {
			coordinates.push_back(stop.coordinates);
		}

		return coordinates;
	}

	map<domain::Bus, vector<svg::Point>> RequestHandler::GetRouteToProjectoredPoints(const vector<domain::Bus>& buses, const renderer::SphereProjector& projector) const {
		map<domain::Bus, vector<svg::Point>> projectored_points;

		for (const domain::Bus& bus : buses) {
			vector<svg::Point> points;
			for (const string_view stop_name : bus.stops) {
				const domain::Stop stop = *db_.GetStop(stop_name);
				points.push_back(projector(stop.coordinates));
			}
			projectored_points[bus] = points;
		}

		return projectored_points;
	}
}