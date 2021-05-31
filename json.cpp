#include "json.h"
#include <sstream>
#include <utility>

namespace json {
	using namespace std;
	
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
			out << " "sv;
		}
	}

	namespace {
		using namespace literals;

		Node LoadNode(istream& input);

		string GetValueType(istream& input) {
			string str;
			while (isalpha(input.peek())) {
				str.push_back(static_cast<char>(input.get()));
			}
			return str;
		}

		Node LoadNull(istream& input) {
			const auto value = GetValueType(input);
			if (value != "null"sv) {
				throw ParsingError("Failed to parse '"s + value + "' as null."s);
			}

			return Node(nullptr);
		}

		Node LoadBool(istream& input) {
			const auto value = GetValueType(input);
			if (value == "true"sv) {
				return Node(true);
			} else if (value == "false"sv) {
				return Node(false);
			} else {
				throw ParsingError("Failed to parse '"s + value + "' as bool."s);
			}
		}

		Node LoadArray(istream& input) {
			Array result;

			for (char c; input >> c && c != ']';) {
				if (c != ',') {
					input.putback(c);
				}
				result.push_back(LoadNode(input));
			}

			if (!input) {
				throw ParsingError("Array parsing error."s);
			}

			return Node(move(result));
		}

		Node LoadString(istream& input) {
			string str;
			while (input.peek()) {
				if (!input) {
					throw ParsingError("String parsing error."s);
				}

				char symbol = static_cast<char>(input.get());
				if(symbol == '"') {
					break;
				} else if(symbol == '\\') {
					input >> symbol;
					switch (symbol) {
						case 'n':
							str.push_back('\n');
							break;
						case 't':
							str.push_back('\t');
							break;
						case 'r':
							str.push_back('\r');
							break;
						case '"':
							str.push_back('"');
							break;
						case '\\':
							str.push_back('\\');
							break;
						default:
							throw ParsingError("Unrecognized escape sequence \\"s + symbol + "."s);
					}
				} else if(symbol == '\n' || symbol == '\r') {
					throw ParsingError("Unexpected end of line."s);
				} else {
					str.push_back(symbol);
				}
			}

			return Node(move(str));
		}

		Node LoadDict(std::istream& input) {
			Dict result;

			for (char c; input >> c && c != '}';) {
				if (c == ',') {
					input >> c;
				}
				string key = LoadString(input).AsString();
				input >> c;
				result.insert({move(key), LoadNode(input)});
			}

			if (!input) {
				throw ParsingError("Dictionary parsing error."s);
			}

			return Node(move(result));
		}

		Node LoadNumber(istream& input) {
			string parsed_num;

			// Считывает в parsed_num очередной символ из input
			auto read_char = [&parsed_num, &input] {
				parsed_num += static_cast<char>(input.get());
				if (!input) {
					throw ParsingError("Failed to read number from stream"s);
				}
			};

			// Считывает одну или более цифр в parsed_num из input
			auto read_digits = [&input, read_char] {
				if (!isdigit(input.peek())) {
					throw ParsingError("A digit is expected"s);
				}
				while (isdigit(input.peek())) {
					read_char();
				}
			};

			if (input.peek() == '-') {
				read_char();
			}
			// Парсим целую часть числа
			if (input.peek() == '0') {
				read_char();
				// После 0 в JSON не могут идти другие цифры
			} else {
				read_digits();
			}

			bool is_int = true;
			// Парсим дробную часть числа
			if (input.peek() == '.') {
				read_char();
				read_digits();
				is_int = false;
			}

			// Парсим экспоненциальную часть числа
			if (int ch = input.peek(); ch == 'e' || ch == 'E') {
				read_char();
				if (ch = input.peek(); ch == '+' || ch == '-') {
					read_char();
				}
				read_digits();
				is_int = false;
			}

			try {
				if (is_int) {
					// Сначала пробуем преобразовать строку в int
					try {
						return stoi(parsed_num);
					} catch (...) {
						// В случае неудачи, например, при переполнении
						// код ниже попробует преобразовать строку в double
					}
				}
				return stod(parsed_num);
			} catch (...) {
				throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
			}
		}

		Node LoadNode(istream& input) {
			char c;
			if (!(input >> c)) {
				throw ParsingError("Unexpected EOF."s);
			}

			if (c == '[') {
				return LoadArray(input);
			} else if (c == '{') {
				return LoadDict(input);
			} else if (c == '"') {
				return LoadString(input);
			} else if (c == 't' || c == 'f') {
				input.putback(c);
				return LoadBool(input);
			} else if (c == 'n') {
				input.putback(c);
				return LoadNull(input);
			} else {
				input.putback(c);
				return LoadNumber(input);
			}
		}

		void PrintNode(ostream& output, const Node& node);

		void PrintValue(ostream& out, nullptr_t) {
			out << "null"sv;
		}

		void PrintValue(ostream& out, const bool& value) {
			out << (value ? "true"sv : "false"sv);
		}

		void PrintValue(ostream& out, const int& value) {
			out << value;
		}

		void PrintValue(ostream& out, const double& value) {
			out << value;
		}

		void PrintValue(ostream& out, const string& str) {
			out << '"';
			for (const char c : str) {
				switch (c) {
					case '\r':
						out << "\\r"sv;
						break;
					case '\t':
						out << "\\t"sv;
						break;
					case '\n':
						out << "\\n"sv;
						break;
					case '"':
						[[fallthrough]];
					case '\\':
						out << "\\"sv;
						[[fallthrough]];
					default:
						out << c;
						break;
				}
			}
			out << '"';
		}

		void PrintValue(ostream& out, const Array& array) {
			RenderContext context{out, 2, 2};
			bool flag = true;

			out << "[\n"sv;
			for (const Node& node : array) {
				if (flag) {
					flag = false;
				} else {
					out << ",\n"sv;
				}
				context.RenderIndent();
				PrintNode(out, node);
			}
			out << "\n]"sv;
		}

		void PrintValue(ostream& out, const Dict& map) {
			RenderContext context{out, 2, 2};
			bool flag = true;
			
			out << "{\n"sv;
			for (const auto& [key, node] : map) {
				if (flag) {
					flag = false;
				} else {
					out << ",\n"sv;
				}
				context.RenderIndent();
				PrintValue(out, key);
				out << ": "sv;
				PrintNode(out, node);
			}
			out << "\n}"sv;
		}

		void PrintNode(ostream& output, const Node& node) {
			ostringstream strm;

			visit(
				[&strm](auto value) {
					PrintValue(strm, value);
				},
				node.GetValue()
			);

			output << strm.str();
		}
	} // namespace

	bool Node::IsNull() const {
		return holds_alternative<nullptr_t>(*this);
	}

	bool Node::IsArray() const {
		return holds_alternative<Array>(*this);
	}

	const Array& Node::AsArray() const {
		using namespace literals;

		if (!IsArray()) {
			throw logic_error("Not an array."s);
		}

		return get<Array>(*this);
	}

	bool Node::IsMap() const {
		return holds_alternative<Dict>(*this);
	}

	const Dict& Node::AsMap() const {
		using namespace literals;

		if (!IsMap()) {
			throw logic_error("Not a dictionary."s);
		}

		return get<Dict>(*this);
	}

	bool Node::IsBool() const {
		return holds_alternative<bool>(*this);
	}

	bool Node::AsBool() const {
		using namespace literals;

		if (!IsBool()) {
			throw logic_error("Not a bool."s);
		}

		return get<bool>(*this);
	}

	bool Node::IsInt() const {
		return holds_alternative<int>(*this);
	}

	int Node::AsInt() const {
		using namespace literals;

		if (!IsInt()) {
			throw logic_error("Not an int."s);
		}
		return get<int>(*this);
	}

	bool Node::IsPureDouble() const {
		return holds_alternative<double>(*this);
	}

	bool Node::IsDouble() const {
		return IsInt() || IsPureDouble();
	}

	double Node::AsDouble() const {
		using namespace literals;

		if (!IsDouble()) {
			throw logic_error("Not a double."s);
		}
		return IsPureDouble() ? get<double>(*this) : AsInt();
	}

	bool Node::IsString() const {
		return holds_alternative<string>(*this);
	}

	const string& Node::AsString() const {
		using namespace literals;

		if (!IsString()) {
			throw logic_error("Not a string."s);
		}

		return get<string>(*this);
	}

	bool Node::operator==(const Node& rhs) const {
		return GetValue() == rhs.GetValue();
	}

	const Node::Value& Node::GetValue() const {
		return *this;
	}

	bool Node::operator!=(const Node& rhs) const {
		return !(GetValue() == rhs.GetValue());
	}

	Document::Document(Node root) 
		: root_(move(root)) {
	}

	const Node& Document::GetRoot() const {
		return root_;
	}

	bool Document::operator==(const Document& rhs) const {
		return GetRoot() == rhs.GetRoot();
	}

	bool Document::operator!=(const Document& rhs) const {
		return !(GetRoot() == rhs.GetRoot());
	}

	Document Load(istream& input) {
		return Document{LoadNode(input)};
	}

	void Print(const Document& doc, ostream& output) {
		PrintNode(output, doc.GetRoot());
	}
} // namespace json