#pragma once

#include "json.h"
#include <vector>
#include <string>
#include <optional>

namespace json {
	class ArrayItemContext;
	class DictItemContext;
	class KeyContext;
	class ValueContext;

	class Builder {
	public:
		Builder();

		KeyContext Key(std::string key);

		Builder& Value(Node::Value value);

		DictItemContext StartDict();

		ArrayItemContext StartArray();

		Builder& EndDict();

		Builder& EndArray();

		Node Build();

	private:
		Node root_;
		std::optional<std::string> key_;
		std::vector<Node*> nodes_stack_;
	};

	class ArrayItemContext {
	public:
		ArrayItemContext(Builder& builder);

		ArrayItemContext& Value(Node::Value value);

		DictItemContext StartDict();

		ArrayItemContext StartArray();

		Builder& EndArray();

	private:
		Builder& builder_;
	};

	class DictItemContext {
	public:
		DictItemContext(Builder& builder);

		KeyContext Key(std::string str);

		Builder& EndDict();

	private:
		Builder& builder_;
	};

	class KeyContext {
	public:
		KeyContext(Builder& builder);

		DictItemContext Value(Node::Value value);

		DictItemContext StartDict();

		ArrayItemContext StartArray();

	private:
		Builder& builder_;
	};
}