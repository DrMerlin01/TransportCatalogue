#pragma once

#include "geo.h"
#include "svg.h"
#include "transport_catalogue.h"

#include <string>
#include <algorithm>
#include <map>

namespace renderer {
	bool IsZero(double value);

	struct RenderSettings {
		double width = 0.;
		double height = 0.;
		double padding = 0.;
		double line_width = 0.;
		double stop_radius = 0.;
		uint32_t bus_label_font_size = 0;
		svg::Point bus_label_offset{0., 0.};
		uint32_t stop_label_font_size = 0;
		svg::Point stop_label_offset{0., 0.};
		svg::Color underlayer_color;
		double underlayer_width = 0.;
		std::vector<svg::Color> color_palette;

		RenderSettings() = default;
	};

	class SphereProjector {
	public:
		template <typename PointInputIt>
		SphereProjector(PointInputIt points_begin, PointInputIt points_end, double max_width, double max_height, double padding) 
			: padding_(padding) {
			if (points_begin == points_end) {
				return;
			}

			const auto [left_it, right_it] 
				= std::minmax_element(points_begin, points_end, [](auto lhs, auto rhs) {
					return lhs.lng < rhs.lng;
				});

			min_lon_ = left_it->lng;
			const double max_lon = right_it->lng;

			const auto [bottom_it, top_it]
				= std::minmax_element(points_begin, points_end, [](auto lhs, auto rhs) {
					return lhs.lat < rhs.lat;
				});

			const double min_lat = bottom_it->lat;
			max_lat_ = top_it->lat;

			std::optional<double> width_zoom;
			if (!IsZero(max_lon - min_lon_)) {
				width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
			}

			std::optional<double> height_zoom;
			if (!IsZero(max_lat_ - min_lat)) {
				height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
			}

			if (width_zoom && height_zoom) {
				zoom_coeff_ = std::min(*width_zoom, *height_zoom);
			} else if (width_zoom) {
				zoom_coeff_ = *width_zoom;
			} else if (height_zoom) {
				zoom_coeff_ = *height_zoom;
			}
		}

		svg::Point operator()(geo::Coordinates coords) const;

	private:
		double padding_;
		double min_lon_ = 0;
		double max_lat_ = 0;
		double zoom_coeff_ = 0;
	}; 

	class MapRenderer {
	public:
		explicit MapRenderer() = default;

		MapRenderer(const transport_catalogue::TransportCatalogue& db);

		void SetRenderSettings(const RenderSettings& settings);

		const RenderSettings& GetRenderSettings() const;

		svg::Document RenderMap();

		void RenderMap(svg::Document& document, const std::vector<svg::Point>& route_points, const svg::Color& color, const std::string& route_name, const svg::Point stop_point, const std::string& stop_name);

	private:
		const transport_catalogue::TransportCatalogue& db_;
		RenderSettings settings_;

		std::vector<transport_catalogue::domain::Bus> GetNonEmptyBuses() const;

		std::vector<transport_catalogue::domain::Stop> GetStopsOnRoutes(const std::vector<transport_catalogue::domain::Bus>& buses) const;

		std::vector<geo::Coordinates> GetStopsCoordsOnRoutes(const std::vector<transport_catalogue::domain::Stop>& stops) const;

		std::map<transport_catalogue::domain::Bus, std::vector<svg::Point>> GetRouteToProjectoredPoints(const std::vector<transport_catalogue::domain::Bus>& buses, const SphereProjector& projector) const;

		void RenderRoutes(svg::Document& document, const std::map<transport_catalogue::domain::Bus, std::vector<svg::Point>>& route_to_projectored_points, const std::vector<svg::Color>& colors) const;

		void RenderRouteNames(svg::Document& document, const std::map<transport_catalogue::domain::Bus, std::vector<svg::Point>>& route_to_projectored_points, const std::vector<svg::Color>& colors) const;

		void RenderRouteName(svg::Document& document, const svg::Point point, const svg::Color& color, const std::string& route_name) const;

		void RenderStopCircles(svg::Document& document, const std::vector<transport_catalogue::domain::Stop>& stops, const SphereProjector& projector) const;

		void RenderStopNames(svg::Document& document, const std::vector<transport_catalogue::domain::Stop>& stops, const SphereProjector& projector) const;
	};
} // namespace renderer