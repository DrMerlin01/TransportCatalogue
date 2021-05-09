#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"
#include <iostream>

using namespace std;
using namespace transport_catalogue;
using namespace transport_catalogue::detail;

int main() {
	TransportCatalogue cataloge;
	
	ReadLineWithData(cataloge);
	GetInfoOnRoutes(cataloge);
	
	return 0;
}