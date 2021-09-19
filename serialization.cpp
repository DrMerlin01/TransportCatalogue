#include "serialization.h"
#include <cstdint>
#include <fstream>
#include <sstream>
#include <utility>
#include <variant>

using namespace std;

namespace serialization {
	Serialization::Serialization(transport_catalogue::TransportCatalogue& transport_catalogue, renderer::MapRenderer& map_renderer, transport_router::TransportRouter& transport_router) 
		: transport_catalogue_(transport_catalogue)
		, map_renderer_(map_renderer)
		, transport_router_(transport_router) { 
	}

	void Serialization::SetSetting(const Path& path_to_base) {
		path_to_base_ = path_to_base;
	}

	void Serialization::CreateBase() {
		std::ofstream out_file(path_to_base_, std::ios::binary);
		SaveStops();
		SaveFromToDistance();
		SaveBuses();
		SaveMap();
		SaveParameters();
		base_.SerializeToOstream(&out_file);
	}

	void Serialization::AccessBase() {
		std::ifstream in_file(path_to_base_, std::ios::binary);
		base_.ParseFromIstream(&in_file);
		LoadStops();
		LoadFromToDistances();
		LoadBuses();
		LoadParameters();
	}

	//----------------------------------------TransportCatalogue----------------------------------------------------    
	proto_transport_catalogue::Stop Serialization::SaveStop(const transport_catalogue::domain::Stop& stop) const {
		proto_transport_catalogue::Stop result;
		result.set_name(stop.name);
		result.mutable_coordinates()->set_lat(stop.coordinates.lat);
		result.mutable_coordinates()->set_lng(stop.coordinates.lng);

		return result;
	}

	proto_transport_catalogue::FromToDistance Serialization::SaveFromToDistance(const transport_catalogue::domain::Stop* from, const transport_catalogue::domain::Stop* to, uint64_t distance) const {
		proto_transport_catalogue::FromToDistance result;
		result.set_from(from->name);
		result.set_to(to->name);
		result.set_distance(distance);

		return result;
	}

	proto_transport_catalogue::Bus Serialization::SaveBus(const transport_catalogue::domain::Bus& bus) const {
		proto_transport_catalogue::Bus result;
		result.set_is_roundtrip(bus.is_roundtrip);
		result.set_name(bus.name);
		for (const auto& stop : bus.stops) {
			result.add_stop_names(string{stop});
		}

		return result;
	}

	void Serialization::SaveStops() {
		for (const auto& stop : transport_catalogue_.GetStops()) {
			*base_.add_stops() = move(SaveStop(stop));
		}
	}

	void Serialization::SaveFromToDistance() {
		for (const auto& [from_to, distance] : transport_catalogue_.GetStopsDistance()) {
			*base_.add_stops_distance() = move(SaveFromToDistance(from_to.first, from_to.second, distance));
		}
	}

	void Serialization::SaveBuses() {
		for (const auto& bus : transport_catalogue_.GetBuses()) {
			*base_.add_buses() = move(SaveBus(bus));
		}
	}

	void Serialization::LoadStop(const proto_transport_catalogue::Stop& stop) {
		transport_catalogue::domain::Stop result;
		result.name = stop.name();
		result.coordinates.lat = stop.coordinates().lat();
		result.coordinates.lng = stop.coordinates().lng();
		transport_catalogue_.AddStop(result.name, result.coordinates);
	}

	void Serialization::LoadFromToDistance(const proto_transport_catalogue::FromToDistance& from_to_distance) {
		const auto from = transport_catalogue_.GetStop(from_to_distance.from());
		const auto to = transport_catalogue_.GetStop(from_to_distance.to());
		transport_catalogue_.SetDistanceBetweenStops(from->name, to->name, from_to_distance.distance());
	}

	void Serialization::LoadBus(const proto_transport_catalogue::Bus& bus) {
		transport_catalogue::domain::Bus result;
		result.is_roundtrip = bus.is_roundtrip();
		result.name = bus.name();
		for (int i = 0; i < bus.stop_names_size(); ++i) {
			result.stops.push_back(transport_catalogue_.GetStop(bus.stop_names(i))->name);
		}
		transport_catalogue_.AddBus(result.name, result.stops, result.is_roundtrip);
	}

	void Serialization::LoadStops() {
		for (int i = 0; i < base_.stops_size(); ++i) {
			LoadStop(base_.stops(i));
		}
	}

	void Serialization::LoadFromToDistances() {
		for (int i = 0; i < base_.stops_distance_size(); ++i) {
			LoadFromToDistance(base_.stops_distance(i));
		}
	}

	void Serialization::LoadBuses() {
		for (int i = 0; i < base_.buses_size(); ++i) {
			LoadBus(base_.buses(i));
		}
	}

	//----------------------------------------MapRenderer----------------------------------------------------------
	void Serialization::SaveMap() const{
		ostringstream out;
		map_renderer_.RenderMap().Render(out);
		base_.mutable_map()->set_description(out.str());
	}

	const string& Serialization::LoadMap() const {
		return base_.map().description();
	}

	//----------------------------------------TransportRouter----------------------------------------------------------
	void Serialization::SaveParameters() const {
		transport_router::RoutingSettings settings = transport_router_.GetSettings();
		(*base_.mutable_settings()).set_bus_wait_time(settings.bus_wait_time);
		(*base_.mutable_settings()).set_bus_velocity(settings.bus_velocity);
	}

	void Serialization::LoadParameters() {
		transport_router::RoutingSettings result;
		result.bus_wait_time = base_.settings().bus_wait_time();
		result.bus_velocity = base_.settings().bus_velocity();
		transport_router_.SetSettings(result);
	}
}
