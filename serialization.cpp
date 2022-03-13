#include "serialization.h"
#include <cstdint>
#include <fstream>
#include <sstream>
#include <utility>
#include <variant>

using namespace std;

namespace serialization {
	Serialization::Serialization(transport_catalogue::TransportCatalogue& transport_catalogue, renderer::MapRenderer& map_renderer, transport_router::TransportRouter& transport_router, const Path& path_to_base) 
		: transport_catalogue_(transport_catalogue)
		, map_renderer_(map_renderer)
		, transport_router_(transport_router)
		, path_to_base_(path_to_base) { 
	}

	void Serialization::SetSetting(const Path& path_to_base) {
		path_to_base_ = path_to_base;
	}

	void Serialization::SerializeBase() {
		std::ofstream out_file(path_to_base_, std::ios::binary);
		FillingBase();
		base_.SerializeToOstream(&out_file);
	}

	void Serialization::DeserializeBase() {
		std::ifstream in_file(path_to_base_, std::ios::binary);
		base_.ParseFromIstream(&in_file);
		DeserializeTransportCatalogue();
		DeserializeParameters();
	}

	void Serialization::FillingBase() {
		SerializeTransportCatalogue();
		SerializeMapRender();
		SerializeParameters();
	}

	//----------------------------------------TransportCatalogue----------------------------------------------------    
	proto_transport_catalogue::Stop Serialization::SerializeStop(const transport_catalogue::domain::Stop& stop) const {
		proto_transport_catalogue::Stop result;
		result.set_name(stop.name);
		result.mutable_coordinates()->set_lat(stop.coordinates.lat);
		result.mutable_coordinates()->set_lng(stop.coordinates.lng);

		return result;
	}

	proto_transport_catalogue::FromToDistance Serialization::SerializeFromToDistance(const transport_catalogue::domain::Stop* from, const transport_catalogue::domain::Stop* to, uint64_t distance) const {
		proto_transport_catalogue::FromToDistance result;
		result.set_from(from->name);
		result.set_to(to->name);
		result.set_distance(distance);

		return result;
	}

	proto_transport_catalogue::Bus Serialization::SerializeBus(const transport_catalogue::domain::Bus& bus) const {
		proto_transport_catalogue::Bus result;
		result.set_is_roundtrip(bus.is_roundtrip);
		result.set_name(bus.name);
		for (const auto& stop : bus.stops) {
			result.add_stop_names(string{stop});
		}

		return result;
	}

	void Serialization::SerializeStops() {
		for (const auto& stop : transport_catalogue_.GetStops()) {
			*base_.mutable_transport_catalogue()->add_stops() = move(SerializeStop(stop));
		}
	}

	void Serialization::SerializeFromToDistance() {
		for (const auto& [from_to, distance] : transport_catalogue_.GetStopsDistance()) {
			*base_.mutable_transport_catalogue()->add_stops_distance() = move(SerializeFromToDistance(from_to.first, from_to.second, distance));
		}
	}

	void Serialization::SerializeBuses() {
		for (const auto& bus : transport_catalogue_.GetBuses()) {
			*base_.mutable_transport_catalogue()->add_buses() = move(SerializeBus(bus));
		}
	}

	void Serialization::SerializeTransportCatalogue() {
		SerializeStops();
		SerializeFromToDistance();
		SerializeBuses();
	}

	void Serialization::DeserializeStop(const proto_transport_catalogue::Stop& stop) {
		transport_catalogue::domain::Stop result;
		result.name = stop.name();
		result.coordinates.lat = stop.coordinates().lat();
		result.coordinates.lng = stop.coordinates().lng();
		transport_catalogue_.AddStop(result.name, result.coordinates);
	}

	void Serialization::DeserializeFromToDistance(const proto_transport_catalogue::FromToDistance& from_to_distance) {
		const auto from = transport_catalogue_.GetStop(from_to_distance.from());
		const auto to = transport_catalogue_.GetStop(from_to_distance.to());
		transport_catalogue_.SetDistanceBetweenStops(from->name, to->name, from_to_distance.distance());
	}

	void Serialization::DeserializeBus(const proto_transport_catalogue::Bus& bus) {
		transport_catalogue::domain::Bus result;
		result.is_roundtrip = bus.is_roundtrip();
		result.name = bus.name();
		for (int i = 0; i < bus.stop_names_size(); ++i) {
			result.stops.push_back(transport_catalogue_.GetStop(bus.stop_names(i))->name);
		}
		transport_catalogue_.AddBus(result.name, result.stops, result.is_roundtrip);
	}

	void Serialization::DeserializeStops() {
		for (int i = 0; i < base_.transport_catalogue().stops_size(); ++i) {
			DeserializeStop(base_.transport_catalogue().stops(i));
		}
	}

	void Serialization::DeserializeFromToDistances() {
		for (int i = 0; i < base_.transport_catalogue().stops_distance_size(); ++i) {
			DeserializeFromToDistance(base_.transport_catalogue().stops_distance(i));
		}
	}

	void Serialization::DeserializeBuses() {
		for (int i = 0; i < base_.transport_catalogue().buses_size(); ++i) {
			DeserializeBus(base_.transport_catalogue().buses(i));
		}
	}

	void Serialization::DeserializeTransportCatalogue() {
		DeserializeStops();
		DeserializeFromToDistances();
		DeserializeBuses();
	}
	
	//----------------------------------------MapRenderer----------------------------------------------------------
	void Serialization::SerializeMapRender() const{
		ostringstream out;
		map_renderer_.RenderMap().Render(out);
		base_.mutable_map()->set_description(out.str());
	}

	const string& Serialization::DeserializeMapRender() const {
		return base_.map().description();
	}

	//----------------------------------------TransportRouter----------------------------------------------------------
	void Serialization::SerializeParameters() const {
		transport_router::RoutingSettings settings = transport_router_.GetSettings();
		(*base_.mutable_routing_settings()).set_bus_wait_time(settings.bus_wait_time);
		(*base_.mutable_routing_settings()).set_bus_velocity(settings.bus_velocity);

		size_t vertex_count = transport_router_.GetVertexCount();
		(*base_.mutable_vertex_count()).set_count(vertex_count);
	}

	void Serialization::DeserializeParameters() {
		transport_router::RoutingSettings result;
		result.bus_wait_time = base_.routing_settings().bus_wait_time();
		result.bus_velocity = base_.routing_settings().bus_velocity();
		transport_router_.SetSettings(result);

		const size_t vertex_count = base_.vertex_count().count();
		transport_router_.SetVertexCount(vertex_count);

		transport_router_.CreateRouter();
	}
}
