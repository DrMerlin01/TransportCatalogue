#pragma once

#include <filesystem>
#include <memory>
#include <string>

#include "domain.h"
#include "map_renderer.h"
#include "transport_catalogue.h"
#include "transport_router.h"

#include <transport_catalogue.pb.h>

namespace serialization {
	class Serialization {
	public:
		using Path = std::filesystem::path;

		Serialization(transport_catalogue::TransportCatalogue& transport_catalogue, renderer::MapRenderer& map_renderer, transport_router::TransportRouter& transport_router);

		void SetSetting(const Path& path_to_base);

		void CreateBase();

		void AccessBase();
	private:
		//----------------------------------------TransportCatalogue----------------------------------------------------
		proto_transport_catalogue::Stop SaveStop(const transport_catalogue::domain::Stop& stop) const;

		proto_transport_catalogue::FromToDistance SaveFromToDistance(const transport_catalogue::domain::Stop* from, const transport_catalogue::domain::Stop* to, uint64_t distance) const;

		proto_transport_catalogue::Bus SaveBus(const transport_catalogue::domain::Bus& bus) const;

		void SaveStops();

		void SaveFromToDistance();

		void SaveBuses();

		void LoadStop(const proto_transport_catalogue::Stop& stop);

		void LoadFromToDistance(const proto_transport_catalogue::FromToDistance& from_to_distance);

		void LoadBus(const proto_transport_catalogue::Bus& bus);

		void LoadStops();

		void LoadFromToDistances();

		void LoadBuses();

		//----------------------------------------MapRenderer----------------------------------------------------------
	public:
		const std::string& LoadMap() const;

	private:
		void SaveMap() const;

		//----------------------------------------TransportRouter----------------------------------------------------------
		void SaveParameters() const;

		void LoadParameters();

		Path path_to_base_;
		transport_catalogue::TransportCatalogue& transport_catalogue_;
		renderer::MapRenderer& map_renderer_;
		transport_router::TransportRouter& transport_router_;
		mutable proto_transport_catalogue::TransportCatalogue base_;
	};
}