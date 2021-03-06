#include "../inc/transport_catalogue.h"
#include "../inc/transport_router.h"
#include "../inc/json_reader.h"
#include "../inc/request_handler.h"
#include "../inc/json.h"

#include <iostream>
#include <fstream>
#include <string_view>

using namespace std;
using namespace transport_catalogue;
using namespace json;
using namespace renderer;
using namespace transport_router;
using namespace serialization;

void PrintUsage(std::ostream& stream = std::cerr) {
	stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		PrintUsage();

		return 1;
	}

	const std::string_view mode(argv[1]);

	TransportCatalogue catalogue;
	MapRenderer renderer(catalogue);
	TransportRouter router(catalogue);
	JsonReader json_reader(catalogue, cin);
	Serialization serialization(catalogue, renderer, router, json_reader.GetSerializationSettings());
	
	if (mode == "make_base"sv) {
		json_reader.FillingCatalogue();
		renderer.SetRenderSettings(json_reader.GetRendererSettings());
		router.SetVertexCount(catalogue.GetStopsCount() * 2);
		router.SetSettings(json_reader.GetRoutingSettings());
		serialization.SerializeBase();
	} else if (mode == "process_requests"sv) {
		serialization.DeserializeBase();
		
		RequestHandler request_handler(catalogue, renderer, router);
		Requests requests = json_reader.GetInfoRequests();
		Array responses;
		
		for (const auto& request : requests) {
			if (request.type == "Stop"s) {
				responses.push_back(json_reader.CreateStopNode(request_handler.GetStopResponse(request.name), request.id));
			} else if (request.type == "Bus"s) {
				responses.push_back(json_reader.CreateBusNode(request_handler.GetBusResponse(request.name), request.id));
			} else if (request.type == "Map"s) {
				responses.push_back(json_reader.CreateMapNode(serialization.DeserializeMapRender(), request.id));
			} else if (request.type == "Route"s) {
				responses.push_back(json_reader.CreateRouteNode(request_handler.GetRouteResponse(request.from, request.to), request.id));
			} else {
				// unknown request
			}
		}

		Document document = json_reader.GetDocument(responses);
		Print(document, cout);
	} else {
		PrintUsage();

		return 1;
	}

	return 0;
}