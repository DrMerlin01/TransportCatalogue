#include "transport_catalogue.h"
#include "transport_router.h"
#include "json_reader.h"
#include "request_handler.h"
#include "json.h"
#include <iostream>

using namespace std;
using namespace transport_catalogue;
using namespace json;
using namespace renderer;
using namespace transport_router;

int main() {
	TransportCatalogue catalogue;

	JsonReader json_reader(catalogue, cin);
	json_reader.FillingCatalogue();

	TransportRouter router(catalogue, json_reader.GetRoutingSettings(), catalogue.GetStopsCount() * 2);

	MapRenderer renderer(catalogue, json_reader.GetRendererSettings());

	RequestHandler request_handler(catalogue, renderer, router);

	Requests requests = json_reader.GetInfoRequests();

	Array responses;

	for (const auto& request : requests) {
		if (request.type == "Stop"s) {
			responses.push_back(json_reader.CreateStopNode(request_handler.GetStopResponse(request.name), request.id));
		} else if (request.type == "Bus"s) {
			responses.push_back(json_reader.CreateBusNode(request_handler.GetBusResponse(request.name), request.id));
		} else if (request.type == "Map"s) {
			responses.push_back(json_reader.CreateMapNode(request_handler.RenderMap(), request.id));
		} else if (request.type == "Route"s) {
			responses.push_back(json_reader.CreateRouteNode(request_handler.GetRouteResponse(request.from, request.to), request.id));
		} else {
			// unknown request
		}
	}

	Document document = json_reader.GetDocument(responses);
	Print(document, cout);

	return 0;
}