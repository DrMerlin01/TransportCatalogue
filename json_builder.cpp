#include "json_builder.h"
#include <stdexcept>
#include <utility>
#include <iostream>
namespace json {
	using namespace std;

	Builder::Builder() {
		nodes_stack_.push_back(&root_);
	}

	KeyContext Builder::Key(string key) {
		if (root_.IsNull() || nodes_stack_.empty()) {
			throw logic_error("Вызов любого метода, кроме Build, при готовом объекте."s);
		}
		if (key_ || !nodes_stack_.back()->IsDict()) {
			throw logic_error("Вызов метода Key снаружи словаря или сразу после другого Key."s);
		}
		key_ = move(key);

		return KeyContext(*this);
	}

	Builder& Builder::Value(Node::Value value) {
		if (nodes_stack_.empty()) {
			throw logic_error("Вызов любого метода, кроме Build, при готовом объекте."s);
		}

		auto item = nodes_stack_.back();
		if (item->IsNull()) {
			item->GetValue() = move(value);
			nodes_stack_.pop_back();
		} else if (item->IsDict()) {
			if (!key_) {
				throw logic_error("Попытка добавить элемент в словарь без ключа."s);
			}

			Dict& dict = const_cast<Dict&>(item->AsDict());
			Node node;
			node.GetValue() = move(value);
			dict.emplace(key_.value(), node);
			key_ = nullopt;
		} else if (item->IsArray()) {
			if (key_) {
				throw logic_error("Попытка добавить ключ в массив."s);
			}

			Array& array = const_cast<Array&>(item->AsArray());
			Node node;
			node.GetValue() = move(value);
			array.emplace_back(node);
		}

		return *this;
	}

	DictItemContext Builder::StartDict() {
		if (nodes_stack_.empty()) {
			throw logic_error("Вызов любого метода, кроме Build, при готовом объекте."s);
		}

		auto item = nodes_stack_.back();
		if (item->IsNull()) {
			*item = move(Dict());
		} else {
			Dict& dict = const_cast<Dict&>(item->AsDict());
			const auto ptr = dict.emplace(key_.value(), Dict());
			nodes_stack_.push_back(&ptr.first->second);
		}

		return DictItemContext(*this);
	}

	ArrayItemContext Builder::StartArray() {
		if (nodes_stack_.empty()) {
			throw logic_error("Вызов любого метода, кроме Build, при готовом объекте."s);
		}

		auto item = nodes_stack_.back();
		if (item->IsNull()) {
			*item = move(Array());
		} else {
			Array& array = const_cast<Array&>(item->AsArray());
			array.emplace_back(Array());
			nodes_stack_.push_back(&array.back());
		}

		return ArrayItemContext(*this);
	}

	Builder& Builder::EndDict() {
		if (nodes_stack_.empty() || !nodes_stack_.back()->IsDict()) {
			throw logic_error("Инструкция EndDict закрывает не словарь."s);
		}
		nodes_stack_.pop_back();

		return *this;
	}

	Builder& Builder::EndArray() {
		if (nodes_stack_.empty() || !nodes_stack_.back()->IsArray()) {
			throw logic_error("Инструкция EndArray закрывает не массив."s);
		}
		nodes_stack_.pop_back();

		return *this;
	}

	Node Builder::Build() {
		if (!nodes_stack_.empty() || root_.IsNull()) {
			throw logic_error("Вызов метода Build при неготовом описываемом объекте."s);
		}

		return root_;
	}
	
	ArrayItemContext::ArrayItemContext(Builder& builder)
		: builder_(builder) {
	}

	ArrayItemContext& ArrayItemContext::Value(Node::Value value) {
		builder_.Value(value);
		return *this;
	}

	DictItemContext ArrayItemContext::StartDict() {
		return builder_.StartDict();
	}

	ArrayItemContext ArrayItemContext::StartArray() {
		return builder_.StartArray();
	}

	Builder& ArrayItemContext::EndArray() {
		return builder_.EndArray();
	}

	DictItemContext::DictItemContext(Builder& builder) 
		: builder_(builder) {
	}

	KeyContext DictItemContext::Key(string str) {
		return builder_.Key(str);
	}

	Builder& DictItemContext::EndDict() {
		return builder_.EndDict();
	}

	KeyContext::KeyContext(Builder& builder) 
		: builder_(builder) {
	}

	DictItemContext KeyContext::Value(Node::Value value) {
		return DictItemContext(builder_.Value(value));
	}

	DictItemContext KeyContext::StartDict() {
		return builder_.StartDict();
	}

	ArrayItemContext KeyContext::StartArray() {
		return builder_.StartArray();
	}
}