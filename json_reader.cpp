#include "json_reader.h"
#include <sstream>
#include <utility>
#include <vector>

namespace transport_catalogue {
	using namespace std;
	using namespace std::literals;

	JsonReader::JsonReader(TransportCatalogue& db, istream& in) 
		: db_(db)
		, document_(json::Load(in)) {
	}

	void JsonReader::FillingCatalogue() {
		vector<request::Stop> stops;
		vector<request::Bus> buses;
		const json::Array& base_requests = document_.GetRoot().AsDict().at("base_requests"s).AsArray();

		for (const json::Node& request : base_requests) {
			const json::Dict& content = request.AsDict();
			if (content.at("type"s).AsString() == "Stop"s) {
				stops.push_back(move(ReadStopFromJson(content)));
				AddStop(stops.back());
			} else {
				buses.push_back(move(ReadBusFromJson(content)));
			}
		}
		for(const auto& stop : stops) {
			SetDistanceBetweenStops(stop);
		}
		for(const auto& bus : buses) {
			AddBus(bus);
		}
	}

	void JsonReader::AddStop(const request::Stop& stop) {
		db_.AddStop(stop.name, stop.coordinates);
	}

	void JsonReader::AddBus(const request::Bus& bus) {
		vector<string_view> bus_stops;
		
		for(const auto& stop_name: bus.bus_stops) {
			bus_stops.push_back(db_.GetStop(stop_name)->name);
		}
		db_.AddBus(bus.name, bus_stops, bus.is_roundtrip);
	}

	void JsonReader::SetDistanceBetweenStops(const request::Stop& stop) {
		for(const auto [stop_to, distance] : stop.stop_to_dist) {
			db_.SetDistanceBetweenStops(stop.name, stop_to, distance);
		}
	}

	json::Document JsonReader::GetDocument(const json::Array& responses) const {
		return json::Document(responses);
	}

	request::Stop JsonReader::ReadStopFromJson(const json::Dict& content_stop) const {
		request::Stop stop;
		
		stop.type = "Stop"s;
		stop.name = content_stop.at("name"s).AsString();
		stop.coordinates = {content_stop.at("latitude"s).AsDouble(), content_stop.at("longitude"s).AsDouble()};
		if(content_stop.find("road_distances"s) != content_stop.end()) {
			for(const auto& [stop_to, distance] : content_stop.at("road_distances"s).AsDict()) {
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

	renderer::RenderSettings JsonReader::GetRendererSettings() const {
		renderer::RenderSettings settings;
		const json::Dict& render_settings = document_.GetRoot().AsDict().at("render_settings"s).AsDict();

		settings.width = render_settings.at("width"s).AsDouble();
		settings.height = render_settings.at("height"s).AsDouble();
		settings.padding = render_settings.at("padding"s).AsDouble();
		settings.line_width = render_settings.at("line_width"s).AsDouble();
		settings.stop_radius = render_settings.at("stop_radius"s).AsDouble();
		settings.bus_label_font_size = render_settings.at("bus_label_font_size"s).AsInt();
		settings.bus_label_offset = ParseOffset(render_settings.at("bus_label_offset"s));
		settings.stop_label_font_size = render_settings.at("stop_label_font_size"s).AsInt();
		settings.stop_label_offset = ParseOffset(render_settings.at("stop_label_offset"));
		settings.underlayer_color = ParseColor(render_settings.at("underlayer_color"s));
		settings.underlayer_width = render_settings.at("underlayer_width"s).AsDouble();

		const json::Node& color_palette_node = render_settings.at("color_palette"s);
		settings.color_palette.reserve(color_palette_node.AsArray().size());
		for (const auto& node : color_palette_node.AsArray()) {
			settings.color_palette.push_back(ParseColor(node));
		}

		return settings;
	}

	Requests JsonReader::GetInfoRequests() const {
		Requests ids;
		const json::Array& stat_requests = document_.GetRoot().AsDict().at("stat_requests"s).AsArray();
		ids.reserve(stat_requests.size());

		for(const json::Node& request : stat_requests) {
			const json::Dict& content = request.AsDict();

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

			return json::Builder{}
						.StartDict()
							.Key("request_id"s).Value(id)
							.Key("buses"s).Value(buses_on_stop)
						.EndDict()
					.Build();
		} else {
			return json::Builder{}
						.StartDict()
							.Key("request_id"s).Value(id)
							.Key("error_message"s).Value("not found"s)
						.EndDict()
					.Build();
		}
	}

	json::Node JsonReader::CreateBusNode(const optional<response::Bus> bus, const int id) const {
		if(bus.has_value()) {
			return json::Builder{}
						.StartDict()
							.Key("request_id"s).Value(id)
							.Key("curvature"s).Value(bus.value().info.curvature)
							.Key("route_length"s).Value(bus.value().info.lenght_route)
							.Key("stop_count"s).Value(static_cast<int>(bus.value().info.total_bus_stops))
							.Key("unique_stop_count"s).Value(static_cast<int>(bus.value().info.unique_bus_stops))
						.EndDict()
					.Build();
		} else {
			return json::Builder{}
						.StartDict()
							.Key("request_id"s).Value(id)
							.Key("error_message"s).Value("not found"s)
						.EndDict()
					.Build();
		}
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

		return json::Builder{}
					.StartDict()
						.Key("request_id"s).Value(id)
						.Key("map"s).Value(out.str())
					.EndDict()
				.Build();
	}
}