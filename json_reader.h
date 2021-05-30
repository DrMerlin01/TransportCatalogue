#pragma once

#include "domain.h"
#include "json.h"
#include <istream>
#include <optional>

namespace transport_catalogue {
    class JsonReader {
    public:
        JsonReader(std::istream& in);

        json::Document GetDocument(const json::Array& responses) const;

        json::Node CreateStopNode(const std::optional<response::Stop> stop, const int id) const;

        json::Node CreateBusNode(const std::optional<response::Bus> bus, const int id) const;

        Requests GetInfoRequests() const;

        ContentRequests GetContentRequests() const;

    private:
        json::Document document_;

        request::Stop ReadStopFromJson(const json::Dict& content_stop) const;

        request::Bus ReadBusFromJson(const json::Dict& content_bus) const;
    };
}