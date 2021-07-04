#pragma once

#include "domain.h"
#include "json.h"
#include "map_renderer.h"
#include <istream>
#include <optional>

namespace transport_catalogue {
	class JsonReader {
	public:
		JsonReader(std::istream& in);

		json::Document GetDocument(const json::Array& responses) const;

		json::Node CreateStopNode(const std::optional<response::Stop> stop, const int id) const;

		json::Node CreateBusNode(const std::optional<response::Bus> bus, const int id) const;

		json::Node CreateMapNode(const svg::Document& document, const int id) const;

		Requests GetInfoRequests() const;

		ContentRequests GetContentRequests() const;

		renderer::RenderSettings GetRendererSettings() const;

	private:
		json::Document document_;

		request::Stop ReadStopFromJson(const json::Dict& content_stop) const;

		request::Bus ReadBusFromJson(const json::Dict& content_bus) const;

		svg::Point ParseOffset(const json::Node& node) const;

		svg::Color ParseColor(const json::Node& node) const;
	};
}