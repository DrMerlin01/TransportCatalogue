#include "json_builder.h"
#include <stdexcept>
#include <utility>
#include <iostream>
namespace json {
	using namespace std;

	Builder::Builder() {
		nodes_stack_.push_back(&root_);
	}

	Node Builder::Build() {
		if (!nodes_stack_.empty() || root_.IsNull()) {
			throw logic_error("Вызов метода Build при неготовом описываемом объекте."s);
		}

		return root_;
	}

	Builder::KeyContext Builder::Key(string key) {
		if (root_.IsNull() || nodes_stack_.empty()) {
			throw logic_error("Вызов любого метода, кроме Build, при готовом объекте."s);
		}
		if (key_ || !nodes_stack_.back()->IsDict()) {
			throw logic_error("Вызов метода Key снаружи словаря или сразу после другого Key."s);
		}
		key_ = move(key);

		return KeyContext(*this);
	}

	Node* Builder::AddItem(Node::Value value) {
		auto item = nodes_stack_.back();
		Node node;
		node.GetValue() = move(value);

		if (item->IsNull()) {
			*item = move(node);
			nodes_stack_.pop_back();

			return &root_;
		} else if (item->IsDict()) {
			if (!key_) {
				throw logic_error("Попытка добавить элемент в словарь без ключа."s);
			}

			Dict& dict = const_cast<Dict&>(item->AsDict());
			const auto ptr = dict.emplace(key_.value(), node);
			key_ = nullopt;

			return &ptr.first->second;
		} else if (item->IsArray()) {
			if (key_) {
				throw logic_error("Попытка добавить ключ в массив."s);
			}

			Array& array = const_cast<Array&>(item->AsArray());
			array.emplace_back(node);

			return &array.back();
		}

		return nullptr;
	}

	Builder& Builder::Value(Node::Value value) {
		if (nodes_stack_.empty()) {
			throw logic_error("Вызов любого метода, кроме Build, при готовом объекте."s);
		}

		AddItem(value);

		return *this;
	}

	Builder::DictItemContext Builder::StartDict() {
		if (nodes_stack_.empty()) {
			throw logic_error("Вызов любого метода, кроме Build, при готовом объекте."s);
		}

		nodes_stack_.push_back(AddItem(Dict()));

		return DictItemContext(*this);
	}

	Builder::ArrayItemContext Builder::StartArray() {
		if (nodes_stack_.empty()) {
			throw logic_error("Вызов любого метода, кроме Build, при готовом объекте."s);
		}

		nodes_stack_.push_back(AddItem(Array()));

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

	Builder::ArrayItemContext::ArrayItemContext(Builder& builder)
		: builder_(builder) {
	}

	Builder::ArrayItemContext& Builder::ArrayItemContext::Value(Node::Value value) {
		builder_.Value(value);
		return *this;
	}

	Builder::DictItemContext Builder::ArrayItemContext::StartDict() {
		return builder_.StartDict();
	}

	Builder::ArrayItemContext Builder::ArrayItemContext::StartArray() {
		return builder_.StartArray();
	}

	Builder& Builder::ArrayItemContext::EndArray() {
		return builder_.EndArray();
	}

	Builder::DictItemContext::DictItemContext(Builder& builder) 
		: builder_(builder) {
	}

	Builder::KeyContext Builder::DictItemContext::Key(string str) {
		return builder_.Key(str);
	}

	Builder& Builder::DictItemContext::EndDict() {
		return builder_.EndDict();
	}

	Builder::KeyContext::KeyContext(Builder& builder) 
		: builder_(builder) {
	}

	Builder::DictItemContext Builder::KeyContext::Value(Node::Value value) {
		return DictItemContext(builder_.Value(value));
	}

	Builder::DictItemContext Builder::KeyContext::StartDict() {
		return builder_.StartDict();
	}

	Builder::ArrayItemContext Builder::KeyContext::StartArray() {
		return builder_.StartArray();
	}
}