#pragma once

#include <iostream>
#include "transport_catalogue.h"

std::ostream& operator<<(std::ostream& out, const InfoOnRoute& r);
void GetInfoOnRoutes(TransportCatalogue& cataloge);