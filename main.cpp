#include "transport_catalogue.h"
#include "json_reader.h"
#include "request_handler.h"
#include "json.h"
#include <iostream>

using namespace std;
using namespace transport_catalogue;
using namespace json;

int main() {
	TransportCatalogue catalogue;

	JsonReader json_reader(cin);

	RequestHandler request_handler(catalogue);
	request_handler.FillingCatalogue(json_reader.GetContentRequests());

	Requests requests = json_reader.GetInfoRequests();

	Array responses;

	for (const auto& request : requests) {
		if (request.type == "Stop"s) {
			responses.push_back(json_reader.CreateStopNode(request_handler.GetStopResponse(request.name), request.id));
		} else if (request.type == "Bus"s) {
			responses.push_back(json_reader.CreateBusNode(request_handler.GetBusResponse(request.name), request.id));
		} else if (request.type == "Map"s) {
			renderer::RenderSettings render_settings = json_reader.GetRendererSettings();
			renderer::MapRenderer renderer(render_settings);
			responses.push_back(json_reader.CreateMapNode(request_handler.RenderMap(renderer), request.id));
		} else {
			// unknown request
		}
	}

	Document document = json_reader.GetDocument(responses);
	Print(document, cout);

	return 0;
}