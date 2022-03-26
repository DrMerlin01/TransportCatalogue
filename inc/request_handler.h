#pragma once

#include "transport_catalogue.h"
#include "transport_router.h"
#include "map_renderer.h"

namespace transport_catalogue {
	class RequestHandler {
	public:
		RequestHandler(TransportCatalogue& db, renderer::MapRenderer& renderer, transport_router::TransportRouter& router);

		std::optional<response::Stop> GetStopResponse(const std::string_view stop_name) const;

		std::optional<response::Bus> GetBusResponse(const std::string_view bus_name) const;

		svg::Document RenderMap() const;

		std::optional<response::Route> GetRouteResponse(const std::string_view from, const std::string_view to) const;

	private:
		// RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
		TransportCatalogue& db_;
		renderer::MapRenderer& renderer_;
		transport_router::TransportRouter& router_;
	};
}