#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"
#include <iostream>

using namespace std;
using namespace transport_catalogue;
using namespace transport_catalogue::input_reader;
using namespace transport_catalogue::stat_reader;

int main() {
	TransportCatalogue catalogue;
	
	ReadLineAndParseData(catalogue, cin);
	GetInfoOnRoutes(catalogue, cin, cout);
	
	return 0;
}