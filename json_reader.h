#pragma once

#include "json.h"
#include "domain.h"
#include "map_renderer.h"
#include "transport_catalogue.h"
#include <istream>
#include <optional>

namespace transport_catalogue {
	class JsonReader {
	public:
		JsonReader(TransportCatalogue& db, std::istream& in);

		void FillingCatalogue();

		renderer::RenderSettings GetRendererSettings() const;

		json::Document GetDocument(const json::Array& responses) const;

		Requests GetInfoRequests() const;

		json::Node CreateStopNode(const std::optional<response::Stop> stop, const int id) const;

		json::Node CreateBusNode(const std::optional<response::Bus> bus, const int id) const;

		json::Node CreateMapNode(const svg::Document& document, const int id) const;

	private:
		TransportCatalogue& db_;
		json::Document document_;

		void AddStop(const request::Stop& stop);

		void AddBus(const request::Bus& bus);

		void SetDistanceBetweenStops(const request::Stop& stop);

		request::Stop ReadStopFromJson(const json::Dict& content_stop) const;

		request::Bus ReadBusFromJson(const json::Dict& content_bus) const;

		svg::Point ParseOffset(const json::Node& node) const;

		svg::Color ParseColor(const json::Node& node) const;
	};
}