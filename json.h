#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

namespace json {
	class Node;
	using Dict = std::map<std::string, Node>;
	using Array = std::vector<Node>;

	struct RenderContext final {
		RenderContext(std::ostream& out);

		RenderContext(std::ostream& out, int indent_step, int indent = 0);

		RenderContext Indented() const;

		void RenderIndent() const;

		std::ostream& out;
		int indent_step = 0;
		int indent = 0;
	};

	// Эта ошибка должна выбрасываться при ошибках парсинга JSON
	class ParsingError : public std::runtime_error {
	public:
		using runtime_error::runtime_error;
	};

	class Node final : private std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string> {
	public:
		using variant::variant;
		using Value = variant;

		bool IsNull() const;

		bool IsArray() const;

		const Array& AsArray() const;

		bool IsMap() const;

		const Dict& AsMap() const;

		bool IsBool() const;

		bool AsBool() const;

		bool IsInt() const;

		int AsInt() const;

		bool IsPureDouble() const;

		bool IsDouble() const;

		double AsDouble() const;

		bool IsString() const;

		const std::string& AsString() const;

		const Value& GetValue() const;

		bool operator==(const Node& rhs) const;

		bool operator!=(const Node& rhs) const;
	};

	class Document {
	public:
		explicit Document(Node root);

		const Node& GetRoot() const;

		bool operator==(const Document& rhs) const;

		bool operator!=(const Document& rhs) const;

	private:
		Node root_;
	};

	Document Load(std::istream& input);

	void Print(const Document& doc, std::ostream& output);
} // namespace json