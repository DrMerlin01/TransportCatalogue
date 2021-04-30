#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"
#include <iostream>

using namespace std;

int main() {
    TransportCatalogue cataloge;
    
    ReadLineWithData(cataloge);
    GetInfoOnRoutes(cataloge);
    
    return 0;
}