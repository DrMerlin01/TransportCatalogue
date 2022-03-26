#include "../inc/map_renderer.h"

namespace renderer {
	using namespace std;

	inline const double EPSILON = 1e-6;

	bool IsZero(double value) {
		return abs(value) < EPSILON;
	}

	svg::Point SphereProjector::operator()(geo::Coordinates coords) const {
		return { (coords.lng - min_lon_) * zoom_coeff_ + padding_, (max_lat_ - coords.lat) * zoom_coeff_ + padding_ };
	}

	MapRenderer::MapRenderer(const transport_catalogue::TransportCatalogue& db) 
		: db_(db) {
	}

	void MapRenderer::SetRenderSettings(const RenderSettings& settings) {
		settings_ = settings;
	}

	const RenderSettings& MapRenderer::GetRenderSettings() const {
		return settings_;
	}

	svg::Document MapRenderer::RenderMap() {
		svg::Document document;

		const vector<transport_catalogue::domain::Bus> buses = GetNonEmptyBuses();
		const vector<transport_catalogue::domain::Stop> stops = GetStopsOnRoutes(buses);
		const vector<geo::Coordinates> stop_coords = GetStopsCoordsOnRoutes(stops);

		const RenderSettings& settings = GetRenderSettings();
		const SphereProjector projector{ stop_coords.begin(), stop_coords.end(), settings.width, settings.height, settings.padding };
		const vector<svg::Color>& colors = settings.color_palette;

		const auto route_to_projectored_points = GetRouteToProjectoredPoints(buses, projector);

		RenderRoutes(document, route_to_projectored_points, colors);
		RenderRouteNames(document, route_to_projectored_points, colors);
		RenderStopCircles(document, stops, projector);
		RenderStopNames(document, stops, projector);

		return document;
	}

	vector<transport_catalogue::domain::Bus> MapRenderer::GetNonEmptyBuses() const {
		const deque<transport_catalogue::domain::Bus>& buses = db_.GetBuses();
		vector<transport_catalogue::domain::Bus> non_empty_buses;
		non_empty_buses.reserve(buses.size());

		for (const transport_catalogue::domain::Bus& bus : buses) {
			if (!bus.stops.empty()) {
				non_empty_buses.push_back(bus);
			}
		}
		sort(non_empty_buses.begin(), non_empty_buses.end());

		return non_empty_buses;
	}

	vector<transport_catalogue::domain::Stop> MapRenderer::GetStopsOnRoutes(const vector<transport_catalogue::domain::Bus>& buses) const {
		vector<transport_catalogue::domain::Stop> unique_stops;

		for (const transport_catalogue::domain::Bus& bus : buses) {
			for (const string_view stop : bus.stops) {
				unique_stops.push_back(*db_.GetStop(stop));
			}
		}
		sort(unique_stops.begin(), unique_stops.end());
		const auto last = unique(unique_stops.begin(), unique_stops.end());
		unique_stops.erase(last, unique_stops.end());

		return unique_stops;
	}

	vector<geo::Coordinates> MapRenderer::GetStopsCoordsOnRoutes(const vector<transport_catalogue::domain::Stop>& stops) const {
		vector<geo::Coordinates> coordinates;

		for (const transport_catalogue::domain::Stop& stop : stops) {
			coordinates.push_back(stop.coordinates);
		}

		return coordinates;
	}

	map<transport_catalogue::domain::Bus, vector<svg::Point>> MapRenderer::GetRouteToProjectoredPoints(const vector<transport_catalogue::domain::Bus>& buses, const SphereProjector& projector) const {
		map<transport_catalogue::domain::Bus, vector<svg::Point>> projectored_points;

		for (const transport_catalogue::domain::Bus& bus : buses) {
			vector<svg::Point> points;
			for (const string_view stop_name : bus.stops) {
				const transport_catalogue::domain::Stop stop = *db_.GetStop(stop_name);
				points.push_back(projector(stop.coordinates));
			}
			projectored_points[bus] = points;
		}

		return projectored_points;
	}

	void MapRenderer::RenderRoutes(svg::Document& document, const map<transport_catalogue::domain::Bus, vector<svg::Point>>& route_to_projectored_points, const vector<svg::Color>& colors) const {
		size_t i = 0;

		for (const auto& [bus, stop_points] : route_to_projectored_points) {			
			svg::Polyline polyline;
			polyline.SetFillColor(svg::NoneColor)
				.SetStrokeColor(move(colors[i % colors.size()]))
				.SetStrokeWidth(settings_.line_width)
				.SetStrokeLineCap(svg::StrokeLineCap::ROUND)
				.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
			for (const svg::Point point : stop_points) {
				polyline.AddPoint(point);
			}
			document.Add(move(polyline));

			++i;
		}
	}

	void MapRenderer::RenderRouteName(svg::Document& document, const svg::Point point, const svg::Color& color, const string& route_name) const {
		document.Add(svg::Text()
					 .SetData(route_name)
					 .SetPosition(point)
					 .SetOffset(settings_.bus_label_offset)
					 .SetFontSize(settings_.bus_label_font_size)
					 .SetFontFamily("Verdana"s)
					 .SetFontWeight("bold"s)
					 .SetFillColor(settings_.underlayer_color)
					 .SetStrokeColor(settings_.underlayer_color)
					 .SetStrokeWidth(settings_.underlayer_width)
					 .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
					 .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND));
		document.Add(svg::Text()
					 .SetData(route_name)
					 .SetPosition(point)
					 .SetOffset(settings_.bus_label_offset)
					 .SetFontSize(settings_.bus_label_font_size)
					 .SetFontFamily("Verdana"s)
					 .SetFontWeight("bold"s)
					 .SetFillColor(color));
	}

	void MapRenderer::RenderRouteNames(svg::Document& document, const std::map<transport_catalogue::domain::Bus, std::vector<svg::Point>>& route_to_projectored_points, const std::vector<svg::Color>& colors) const {
		size_t i = 0;

		for (const auto& [bus, stops_points] : route_to_projectored_points) {
			RenderRouteName(document, stops_points.front(), colors[i % colors.size()], bus.name);
			if (!bus.is_roundtrip && bus.stops.front() != bus.stops[bus.stops.size() / 2]) {
				RenderRouteName(document, stops_points[bus.stops.size() / 2], colors[i % colors.size()], bus.name);
			}
			++i;
		}
	}

	void MapRenderer::RenderStopCircles(svg::Document& document, const std::vector<transport_catalogue::domain::Stop>& stops, const SphereProjector& projector) const {
		for (const transport_catalogue::domain::Stop& stop : stops) {
			const svg::Point projected_point = projector(stop.coordinates);
			document.Add(svg::Circle()
					 .SetCenter(projected_point)
					 .SetRadius(settings_.stop_radius)
					 .SetFillColor("white"s));
		}
	}

	void MapRenderer::RenderStopNames(svg::Document& document, const std::vector<transport_catalogue::domain::Stop>& stops, const SphereProjector& projector) const {
		for (const transport_catalogue::domain::Stop& stop : stops) {
			const svg::Point projected_point = projector(stop.coordinates);
			document.Add(svg::Text()
					 .SetPosition(projected_point)
					 .SetOffset(settings_.stop_label_offset)
					 .SetFontSize(settings_.stop_label_font_size)
					 .SetFontFamily("Verdana"s)
					 .SetData(stop.name)
					 .SetFillColor(settings_.underlayer_color)
					 .SetStrokeColor(settings_.underlayer_color)
					 .SetStrokeWidth(settings_.underlayer_width)
					 .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
					 .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND));
			document.Add(svg::Text()
					 .SetPosition(projected_point)
					 .SetOffset(settings_.stop_label_offset)
					 .SetFontSize(settings_.stop_label_font_size)
					 .SetFontFamily("Verdana"s)
					 .SetData(stop.name)
					 .SetFillColor("black"s));
		}
	}
} // namespace renderer