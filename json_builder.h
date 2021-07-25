#pragma once

#include "json.h"
#include <vector>
#include <string>
#include <optional>

namespace json {
	class Builder {
	public:
		class ArrayItemContext;
		class DictItemContext;
		class KeyContext;
		class ValueContext;

		class ArrayItemContext {
		public:
			ArrayItemContext(Builder& builder);

			ArrayItemContext& Value(Node::Value value);

			DictItemContext StartDict();

			ArrayItemContext StartArray();

			Builder& EndArray();

		private:
			friend class Builder;
			Builder& builder_;
		};

		class DictItemContext {
		public:
			DictItemContext(Builder& builder);

			KeyContext Key(std::string str);

			Builder& EndDict();

		private:
			friend class Builder;
			Builder& builder_;
		};

		class KeyContext {
		public:
			KeyContext(Builder& builder);

			DictItemContext Value(Node::Value value);

			DictItemContext StartDict();

			ArrayItemContext StartArray();

		private:
			friend class Builder;
			Builder& builder_;
		};

		Builder();

		Node Build();

		KeyContext Key(std::string key);

		Node* AddItem(Node::Value value);

		Builder& Value(Node::Value value);

		DictItemContext StartDict();

		ArrayItemContext StartArray();

		Builder& EndDict();

		Builder& EndArray();

	private:
		Node root_;
		std::optional<std::string> key_;
		std::vector<Node*> nodes_stack_;
	};
}