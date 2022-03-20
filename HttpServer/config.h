#pragma once

#include <unordered_map>
#include <strutil/cpp_string_util.h>

using namespace strutil;

class ConfigFile
{
public:
	struct Value
	{
		std::string stringValue;
		int intValue;
		bool boolValue;

		inline Value() : stringValue(), intValue(0), boolValue(false) { }
		inline Value(const std::string &string, int intValue, bool boolValue) :
			stringValue(string), intValue(intValue), boolValue(boolValue) { }
		inline Value(const Value &other)
		{
			operator=(other);
		}

		inline Value &operator=(Value &&other) noexcept
		{
			if (this != &other)
			{
				stringValue = std::move(other.stringValue);
				intValue = other.intValue;
				boolValue = other.boolValue;
			}
			return *this;
		}

		inline Value &operator=(const Value &other)
		{
			if (this != &other)
			{
				stringValue = other.stringValue;
				intValue = other.intValue;
				boolValue = other.boolValue;
			}
			return *this;
		}
	};

	class Section
	{
	private:
		std::unordered_map<CaseInsensitiveString, Value> m_values;
	public:
		inline Section() { }

		inline Section(const Section &other)
		{
			operator=(other);
		}

		inline Section &operator=(Section &&other) noexcept
		{
			if (this != &other)
				m_values = std::move(other.m_values);
			return *this;
		}

		inline Section &operator=(const Section &other)
		{
			if (this != &other)
				m_values = other.m_values;
			return *this;
		}

		void AddValue(const CaseInsensitiveString &key, const char *value, int valuelen);

		inline const Value *FindValue(const CaseInsensitiveString &key) const
		{
			auto it = m_values.find(key);
			return it == m_values.end() ? nullptr : &it->second;
		}

		constexpr const std::unordered_map<CaseInsensitiveString, Value> &GetValues() const
		{
			return m_values;
		}
	};
private:
	std::unordered_map<CaseInsensitiveString, Section> m_sections;
public:
	ConfigFile(const char *file);
	inline ConfigFile(const ConfigFile &other)
	{
		operator=(other);
	}

	inline const Section *FindSection(const CaseInsensitiveString &name) const
	{
		auto it = m_sections.find(name);
		return it == m_sections.end() ? nullptr : &it->second;
	}

	inline ConfigFile &operator=(ConfigFile &&other) noexcept
	{
		if (this != &other)
			m_sections = std::move(other.m_sections);
		return *this;
	}

	inline ConfigFile &operator=(const ConfigFile &other)
	{
		if (this != &other)
			m_sections = other.m_sections;
		return *this;
	}
};