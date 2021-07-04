#include "map_renderer.h"

namespace renderer {
	using namespace std;

	inline const double EPSILON = 1e-6;

	bool IsZero(double value) {
		return abs(value) < EPSILON;
	}

	svg::Point SphereProjector::operator()(geo::Coordinates coords) const {
		return { (coords.lng - min_lon_) * zoom_coeff_ + padding_, (max_lat_ - coords.lat) * zoom_coeff_ + padding_ };
	}

	MapRenderer::MapRenderer(const RenderSettings& settings) 
		: settings_(settings) {
	}

	const RenderSettings& MapRenderer::GetRenderSettings() const {
		return settings_;
	}

	void MapRenderer::RenderMap(svg::Document& document, const vector<svg::Point>& route_points, const svg::Color& color, const string& route_name, const svg::Point stop_point, const string& stop_name) {
		RenderRoute(document, route_points, color);
		RenderRouteName(document, stop_point, color, route_name);
		RenderStopCircle(document, stop_point);
		RenderStopName(document, stop_point, stop_name);
	}

	void MapRenderer::RenderRoute(svg::Document& document, const vector<svg::Point>& points, const svg::Color& color) const {
		svg::Polyline polyline;
		polyline.SetFillColor(svg::NoneColor)
			.SetStrokeColor(move(color))
			.SetStrokeWidth(settings_.line_width)
			.SetStrokeLineCap(svg::StrokeLineCap::ROUND)
			.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
		for (const svg::Point point : points) {
			polyline.AddPoint(point);
		}
		document.Add(move(polyline));
	}

	void MapRenderer::RenderRouteName(svg::Document& document, const svg::Point point, const svg::Color& color, const string& route_name) const {
		document.Add(svg::Text()
					 .SetData(route_name)
					 .SetPosition(point)
					 .SetOffset(settings_.bus_label_offset)
					 .SetFontSize(settings_.bus_label_font_size)
					 .SetFontFamily("Verdana"s)
					 .SetFontWeight("bold"s)
					 .SetFillColor(settings_.underlayer_color)
					 .SetStrokeColor(settings_.underlayer_color)
					 .SetStrokeWidth(settings_.underlayer_width)
					 .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
					 .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND));
		document.Add(svg::Text()
					 .SetData(route_name)
					 .SetPosition(point)
					 .SetOffset(settings_.bus_label_offset)
					 .SetFontSize(settings_.bus_label_font_size)
					 .SetFontFamily("Verdana"s)
					 .SetFontWeight("bold"s)
					 .SetFillColor(color));
	}

	void MapRenderer::RenderStopCircle(svg::Document& document, const svg::Point point) const {
		document.Add(svg::Circle()
					 .SetCenter(point)
					 .SetRadius(settings_.stop_radius)
					 .SetFillColor("white"s));
	}

	void MapRenderer::RenderStopName(svg::Document& document, const svg::Point point, const string& stop_name) const {
		document.Add(svg::Text()
					 .SetPosition(point)
					 .SetOffset(settings_.stop_label_offset)
					 .SetFontSize(settings_.stop_label_font_size)
					 .SetFontFamily("Verdana"s)
					 .SetData(stop_name)
					 .SetFillColor(settings_.underlayer_color)
					 .SetStrokeColor(settings_.underlayer_color)
					 .SetStrokeWidth(settings_.underlayer_width)
					 .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
					 .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND));
		document.Add(svg::Text()
					 .SetPosition(point)
					 .SetOffset(settings_.stop_label_offset)
					 .SetFontSize(settings_.stop_label_font_size)
					 .SetFontFamily("Verdana"s)
					 .SetData(stop_name)
					 .SetFillColor("black"s));
	}
} // namespace renderer