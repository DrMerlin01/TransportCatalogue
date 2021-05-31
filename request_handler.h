#pragma once

#include "transport_catalogue.h"

namespace transport_catalogue {
	class RequestHandler {
	public:
		// MapRenderer понадобится в следующей части итогового проекта
		RequestHandler(TransportCatalogue& db/*, const renderer::MapRenderer& renderer*/);

		void FillingCatalogue(const ContentRequests& base_requests);

		std::optional<response::Stop> GetStopResponse(const std::string_view& stop_name) const;

		// Возвращает информацию о маршруте (запрос Bus)
		std::optional<response::Bus> GetBusResponse(const std::string_view& bus_name) const;

		// Этот метод будет нужен в следующей части итогового проекта
		//svg::Document RenderMap() const;

	private:
		// RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
		TransportCatalogue& db_;
		//const renderer::MapRenderer& renderer_;

		void AddStop(const request::Stop& stop);

		void AddBus(const request::Bus& bus);

		void SetDistanceBetweenStops(const request::Stop& stop);
	};
}