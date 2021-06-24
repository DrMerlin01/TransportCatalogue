#include "json_reader.h"
#include <sstream>
#include <utility>

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