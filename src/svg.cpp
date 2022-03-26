#include "../inc/svg.h"

#include <sstream>

namespace svg {
	using namespace std::literals;
	using namespace std;

	Rgb::Rgb(uint8_t red, uint8_t green, uint8_t blue) 
		: red(red)
		, green(green)
		, blue(blue) {
	}

	Rgba::Rgba(uint8_t red, uint8_t green, uint8_t blue, double opacity) 
		: Rgb(red, green, blue)
		, opacity(opacity) {
	}

	ostream& operator<<(ostream& out, const StrokeLineCap& line_cap) {
		switch (line_cap) {
			case StrokeLineCap::BUTT:
				out << "butt"sv;
				break;
			case StrokeLineCap::ROUND:
				out << "round"sv;
				break;
			case StrokeLineCap::SQUARE:
				out << "square"sv;
				break;
			default:
				break;
		}

		return out;
	}

	ostream& operator<<(ostream& out, const StrokeLineJoin line_join) {
		switch (line_join) {
			case StrokeLineJoin::ARCS:
				out << "arcs"sv;
				break;
			case StrokeLineJoin::BEVEL:
				out << "bevel"sv;
				break;
			case StrokeLineJoin::MITER:
				out << "miter"sv;
				break;
			case StrokeLineJoin::MITER_CLIP:
				out << "miter-clip"sv;
				break;
			case StrokeLineJoin::ROUND:
				out << "round"sv;
				break;
			default:
				break;
		}

		return out;
	}

	string ColorPrinter::operator()(monostate) const {
		return "none"s;
	}

	string ColorPrinter::operator()(string color) const {
		return color;
	}

	string ColorPrinter::operator()(Rgb rgb) const {
		stringstream out;

		out << "rgb("sv;
		out << to_string(rgb.red);
		out << ","sv;
		out << to_string(rgb.green);
		out << ","sv;
		out << to_string(rgb.blue);
		out << ")"sv;

		return out.str();
	}

	string ColorPrinter::operator()(Rgba rgba) const {
		stringstream out;

		out << "rgba("sv;
		out << to_string(rgba.red);
		out << ","sv;
		out << to_string(rgba.green);
		out << ","sv;
		out << to_string(rgba.blue);
		out << ","sv;
		out << (rgba.opacity);
		out << ")"sv;

		return out.str();
	}

	Point::Point(double x, double y)
		: x(x)
		, y(y) {
	}

	RenderContext::RenderContext(ostream& out)
		: out(out) {
	}

	RenderContext::RenderContext(ostream& out, int indent_step, int indent)
		: out(out)
		, indent_step(indent_step)
		, indent(indent) {
	}

	RenderContext RenderContext::Indented() const {
		return {out, indent_step, indent + indent_step};
	}

	void RenderContext::RenderIndent() const {
		for (int i = 0; i < indent; ++i) {
			out.put(' ');
		}
	}

	void Object::Render(const RenderContext& context) const {
		context.RenderIndent();

		// Делегируем вывод тега своим подклассам
		RenderObject(context);

		context.out << endl;
	}

	// ---------- Circle ------------------
	Circle& Circle::SetCenter(Point center)  {
		center_ = center;
		return *this;
	}

	Circle& Circle::SetRadius(double radius)  {
		radius_ = radius;
		return *this;
	}

	void Circle::RenderObject(const RenderContext& context) const {
		auto& out = context.out;
		out << "<circle "sv;
		out << "cx=\""sv << center_.x << "\" "sv;
		out << "cy=\""sv << center_.y << "\" "sv;
		out << "r=\""sv << radius_ << "\" "sv;
		RenderAttrs(out);
		out << "/>"sv;
	}

	// ---------- Polyline ------------------
	Polyline& Polyline::AddPoint(Point point) {
		points_.push_back(point);
		return *this;
	}

	void Polyline::RenderObject(const RenderContext& context) const {
		auto& out = context.out;
		out << "<polyline "sv;
		out << "points=\""sv;
		bool flag = true;
		for(const auto point : points_) {
			if(flag) {
				flag = false;
			} else {
				out << " "sv;
			}
			out << point.x << ","sv << point.y;
		}
		out << "\""sv;
		RenderAttrs(out);
		out << "/>"sv;
	}

	// ---------- Text ------------------
	Text& Text::SetPosition(Point pos) {
		point_ = pos;
		return *this;
	}

	Text& Text::SetOffset(Point offset) {
		offset_ = offset;
		return *this;
	}

	Text& Text::SetFontSize(uint32_t size) {
		font_size_ = size;
		return *this;
	}

	Text& Text::SetFontFamily(string font_family) {
		font_family_ = move(font_family);
		return *this;
	}

	Text& Text::SetFontWeight(string font_weight) {
		font_weight_ = move(font_weight);
		return *this;
	}

	Text& Text::SetData(string data) {
		content_ = move(data);
		return *this;
	}

	void Text::RenderObject(const RenderContext& context) const {
		auto& out = context.out;
		out << "<text"sv;
		RenderAttrs(out);
		out << " x=\""sv << point_.x << "\""sv;
		out << " y=\""sv << point_.y << "\""sv;
		out << " dx=\""sv << offset_.x << "\"";
		out << " dy=\""sv << offset_.y << "\""sv;
		out << " font-size=\""sv << font_size_ << "\""sv;
		if(!font_family_.empty()) {
			out << " font-family=\""sv << font_family_ << "\""sv;    
		}
		if(!font_weight_.empty()) {
			out << " font-weight=\""sv << font_weight_ << "\""sv;   
		}
		out << ">"sv;
		out << content_;
		out << "</text>"sv;
	}

	// ---------- Document ------------------    
	void Document::AddPtr(unique_ptr<Object>&& obj) {
		objects_.push_back(move(obj));
	}

	void Document::Render(ostream& out) const {
		out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << endl;
		out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << endl;
		for(const auto& object : objects_) {
			object->Render(RenderContext{out, 2, 2});
		}
		out << "</svg>"sv << endl;
	}
} // namespace svg