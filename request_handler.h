#pragma once

#include "transport_catalogue.h"
#include "map_renderer.h"

namespace transport_catalogue {
	class RequestHandler {
	public:
		RequestHandler(TransportCatalogue& db);

		void FillingCatalogue(const ContentRequests& base_requests);

		std::optional<response::Stop> GetStopResponse(const std::string_view& stop_name) const;

		std::optional<response::Bus> GetBusResponse(const std::string_view& bus_name) const;

		svg::Document RenderMap(const renderer::MapRenderer& renderer) const;

		void RenderRoute(svg::Document& document, const std::map<domain::Bus, std::vector<svg::Point>>& route_to_projectored_points, const std::vector<svg::Color>& colors, const renderer::MapRenderer& renderer) const;

		void RenderRouteName(svg::Document& document, const std::map<domain::Bus, std::vector<svg::Point>>& route_to_projectored_points, const std::vector<svg::Color>& colors, const renderer::MapRenderer& renderer) const;

		void RenderStopCircles(svg::Document& document, const std::vector<domain::Stop>& stops, const renderer::SphereProjector& projector, const renderer::MapRenderer& renderer) const;

		void RenderStopNames(svg::Document& document, const std::vector<domain::Stop>& stops, const renderer::SphereProjector& projector, const renderer::MapRenderer& renderer) const;

	private:
		// RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
		TransportCatalogue& db_;

		void AddStop(const request::Stop& stop);

		void AddBus(const request::Bus& bus);

		void SetDistanceBetweenStops(const request::Stop& stop);

		std::vector<domain::Bus> GetNonEmptyBuses() const;

		std::vector<domain::Stop> GetStopsOnRoutes(const std::vector<domain::Bus>& buses) const;

		std::vector<geo::Coordinates> GetStopsCoordsOnRoutes(const std::vector<domain::Stop>& stops) const;

		std::map<domain::Bus, std::vector<svg::Point>> GetRouteToProjectoredPoints(const std::vector<domain::Bus>& buses, const renderer::SphereProjector& projector) const;
	};
}