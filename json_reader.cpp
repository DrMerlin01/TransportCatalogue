#include "json_reader.h"
#include <sstream>
#include <utility>
#include <vector>

namespace transport_catalogue {
	using namespace std;
	using namespace std::literals;

	JsonReader::JsonReader(istream& in) 
		: document_(json::Load(in)) {
	}

	json::Document JsonReader::GetDocument(const json::Array& responses) const {
		return json::Document(responses);
	}

	Requests JsonReader::GetInfoRequests() const {
		Requests ids;
		const json::Array& stat_requests = document_.GetRoot().AsMap().at("stat_requests"s).AsArray();
		ids.reserve(stat_requests.size());

		for(const json::Node& request : stat_requests) {
			const json::Dict& content = request.AsMap();

			request::Identification identification;
			identification.id = content.at("id"s).AsInt();
			identification.type = content.at("type"s).AsString();
			if(content.find("name"s) != content.end()) {
				identification.name = content.at("name"s).AsString();
			}

			ids.push_back(move(identification));
		}

		return ids;
	}

	json::Node JsonReader::CreateStopNode(const optional<response::Stop> stop, const int id) const {
		if(stop.has_value()) {
			json::Array buses_on_stop;
			for(const auto& stop : stop.value().buses_on_stop) {
				buses_on_stop.push_back(json::Node{string{stop}});
			}

			return { json::Dict{{ "request_id"s, id }, { "buses"s, buses_on_stop }} };
		} else {
			return { json::Dict{{ "request_id"s, id }, { "error_message"s, json::Node{string("not found"s)} }} };
		}
	}

	json::Node JsonReader::CreateBusNode(const optional<response::Bus> bus, const int id) const {
		if(bus.has_value()) {
			return { json::Dict{{ "request_id"s, id },
								{ "curvature"s, bus.value().info.curvature },
								{ "route_length"s, bus.value().info.lenght_route },
								{ "stop_count"s, static_cast<int>(bus.value().info.total_bus_stops)},
								{ "unique_stop_count"s, static_cast<int>(bus.value().info.unique_bus_stops) }}};
		} else {
			return { json::Dict{{ "request_id"s, id }, { "error_message"s, json::Node{string("not found"s)} }} };
		}
	}

	ContentRequests JsonReader::GetContentRequests() const {
		vector<request::Stop> stops;
		vector<request::Bus> buses;
		const json::Array& base_requests = document_.GetRoot().AsMap().at("base_requests"s).AsArray();

		for (const json::Node& request : base_requests) {
			const json::Dict& content = request.AsMap();
			if (content.at("type"s).AsString() == "Stop"s) {
				stops.push_back(move(ReadStopFromJson(content)));
			} else {
				buses.push_back(move(ReadBusFromJson(content)));
			}
		}

		return { move(stops), move(buses) };
	}

	svg::Point JsonReader::ParseOffset(const json::Node& node) const {
		const json::Array& array = node.AsArray();
		const json::Node& dx = array[0];
		const json::Node& dy = array[1];

		return { dx.AsDouble(), dy.AsDouble() };
	}

	svg::Color JsonReader::ParseColor(const json::Node& node) const {
		if (node.IsArray()) {
			const json::Array& array = node.AsArray();

			const json::Node& red_node = array[0];
			const uint8_t red = static_cast<uint8_t>(red_node.AsInt());

			const json::Node& green_node = array[1];
			const uint8_t green = static_cast<uint8_t>(green_node.AsInt());

			const json::Node& blue_node = array[2];
			const uint8_t blue = static_cast<uint8_t>(blue_node.AsInt());

			if (array.size() == 3) {
				return svg::Rgb{ red, green, blue };
			} else if (array.size() == 4) {
				const json::Node& opacity = array[3];
				return svg::Rgba{ red, green, blue, opacity.AsDouble() };
			}
		} else {
			return node.AsString();
		}
		
		return {};
	}

	json::Node JsonReader::CreateMapNode(const svg::Document& document, const int id) const {
		stringstream out;
		document.Render(out);
		return { json::Dict{{ "request_id"s, id }, { "map"s, out.str() }} };
	}

	renderer::RenderSettings JsonReader::GetRendererSettings() const {
		const json::Dict& render_settings = document_.GetRoot().AsMap().at("render_settings"s).AsMap();

		const json::Node& width_node = render_settings.at("width"s);
		const double width = width_node.AsDouble();

		const json::Node& height_node = render_settings.at("height"s);
		const double height = height_node.AsDouble();

		const json::Node& padding_node = render_settings.at("padding"s);
		const double padding = padding_node.AsDouble();

		const json::Node& line_width_node = render_settings.at("line_width"s);
		const double line_width = line_width_node.AsDouble();

		const json::Node& stop_radius_node = render_settings.at("stop_radius"s);
		const double stop_radius = stop_radius_node.AsDouble();

		const json::Node& bus_label_font_size_node = render_settings.at("bus_label_font_size"s);
		const uint32_t bus_label_font_size = bus_label_font_size_node.AsInt();

		const svg::Point bus_label_offset = ParseOffset(render_settings.at("bus_label_offset"s));

		const json::Node& stop_label_font_size_node = render_settings.at("stop_label_font_size"s);
		const uint32_t stop_label_font_size = stop_label_font_size_node.AsInt();

		const svg::Point stop_label_offset = ParseOffset(render_settings.at("stop_label_offset"));

		const svg::Color underlayer_color = ParseColor(render_settings.at("underlayer_color"s));

		const json::Node& underlayer_width_node = render_settings.at("underlayer_width"s);
		const double underlayer_width = underlayer_width_node.AsDouble();

		const json::Node& color_palette_node = render_settings.at("color_palette"s);
		vector<svg::Color> color_palette;
		color_palette.reserve(color_palette_node.AsArray().size());
		for (const auto& node : color_palette_node.AsArray()) {
			color_palette.push_back(ParseColor(node));
		}

		return { width, 
				 height, 
				 padding, 
				 line_width, 
				 stop_radius, 
				 bus_label_font_size, 
				 bus_label_offset, 
				 stop_label_font_size, 
				 stop_label_offset, 
				 underlayer_color, 
				 underlayer_width, 
				 color_palette
				};
	}

	request::Stop JsonReader::ReadStopFromJson(const json::Dict& content_stop) const {
		request::Stop stop;
		stop.type = "Stop"s;
		stop.name = content_stop.at("name"s).AsString();
		stop.coordinates = {content_stop.at("latitude"s).AsDouble(), content_stop.at("longitude"s).AsDouble()};
		if(content_stop.find("road_distances"s) != content_stop.end()) {
			for(const auto& [stop_to, distance] : content_stop.at("road_distances"s).AsMap()) {
				stop.stop_to_dist.insert({ stop_to, distance.AsInt() });
			}
		}

		return stop;
	}

	request::Bus JsonReader::ReadBusFromJson(const json::Dict& content_bus) const {
		request::Bus bus;
		bus.type = "Bus"s;
		bus.name = content_bus.at("name"s).AsString();
		bus.is_roundtrip = content_bus.at("is_roundtrip"s).AsBool();
		for(const json::Node& stop : content_bus.at("stops"s).AsArray()) {
			bus.bus_stops.push_back(stop.AsString());
		}
		if(!bus.is_roundtrip) {
			for (int i = bus.bus_stops.size() - 2; i >= 0; --i) {
				bus.bus_stops.push_back(bus.bus_stops[i]);
			}
		}
		
		return bus;
	}
}