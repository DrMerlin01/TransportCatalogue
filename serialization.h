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

		Serialization(transport_catalogue::TransportCatalogue& transport_catalogue, renderer::MapRenderer& map_renderer, transport_router::TransportRouter& transport_router, const Path& path_to_base);

		void SetSetting(const Path& path_to_base);

		void SerializeBase();

		void DeserializeBase();
	private:
		void FillingBase();
		//----------------------------------------TransportCatalogue----------------------------------------------------
		proto_transport_catalogue::Stop SerializeStop(const transport_catalogue::domain::Stop& stop) const;

		proto_transport_catalogue::FromToDistance SerializeFromToDistance(const transport_catalogue::domain::Stop* from, const transport_catalogue::domain::Stop* to, uint64_t distance) const;

		proto_transport_catalogue::Bus SerializeBus(const transport_catalogue::domain::Bus& bus) const;

		void SerializeStops();

		void SerializeFromToDistance();

		void SerializeBuses();

		void SerializeTransportCatalogue();

		void DeserializeStop(const proto_transport_catalogue::Stop& stop);

		void DeserializeFromToDistance(const proto_transport_catalogue::FromToDistance& from_to_distance);

		void DeserializeBus(const proto_transport_catalogue::Bus& bus);

		void DeserializeStops();

		void DeserializeFromToDistances();

		void DeserializeBuses();

		void DeserializeTransportCatalogue();

		//----------------------------------------MapRenderer----------------------------------------------------------
	public:
		const std::string& DeserializeMapRender() const;

	private:
		void SerializeMapRender() const;

		//----------------------------------------TransportRouter----------------------------------------------------------
		void SerializeParameters() const;

		void DeserializeParameters();

		Path path_to_base_;
		transport_catalogue::TransportCatalogue& transport_catalogue_;
		renderer::MapRenderer& map_renderer_;
		transport_router::TransportRouter& transport_router_;
		mutable proto_transport_catalogue::DataBaseCatalogue base_;
	};
}