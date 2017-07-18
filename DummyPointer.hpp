#pragma once

namespace Linqpp
{
	namespace Detail
	{
		template <class T>
		class DummyPointer
		{
		private:
			T _value;

		public:
			explicit DummyPointer(T&& value) : _value(std::move(value)) { }

			auto operator->() const { return std::addressof(_value); }
			auto operator->() { return std::addressof(_value); }
		};

		template <class T>
		auto CreateDummyPointer(T&& value) { return DummyPointer<T>(std::move(value)); }
	}
}
