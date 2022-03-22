#include "http_cookie.h"

static constexpr char AttribSeparator[] = "; ";
static constexpr int AttribSeparatorLength = sizeof AttribSeparator - 1;

static inline StringBuilder &AppendSeparator(StringBuilder &builder)
{
	return builder.Append(AttribSeparator, AttribSeparatorLength);
}

StringBuilder &HTTPCookie::AppendToBuilder(StringBuilder &builder) const
{
//	StringBuilder actualValue;
/*	actualValue.Append('\"');
	for (size_t i = 0; i < m_value.size(); i++)
	{
		const char *escval = ToASCIIEscapeString(m_value[i]);
		if (escval)
			actualValue.Append(escval);
		else
			actualValue.Append(m_value[i]);
	}*/

	//	actualValue.Append('\"');

	builder.Append(m_name).Append('=');
	//builder.Append(actualValue.ToInPlaceString(), actualValue.Size());
	builder.Append(m_value);

	if (m_date.dayOfWeek != 0xffff)
	{
		AppendSeparator(builder);
		builder.Append("Expires=");
		AppendHTTPDate(builder, m_date);
	}

	if (m_maxage >= 0)
	{
		AppendSeparator(builder);
		builder.Append("Max-Age=").Append(std::to_string(m_maxage));
	}

	if (m_domain.length() > 0)
	{
		AppendSeparator(builder);
		builder.Append("Domain=").Append(m_domain);
	}

	if (m_path.length() > 0)
	{
		AppendSeparator(builder);
		builder.Append("Path=").Append(m_path);
	}

	if (m_secure == SECURE_TRUE)
	{
		AppendSeparator(builder);
		builder.Append("Secure");
	}

	if (m_httpOnly == HTTPONLY_TRUE)
	{
		AppendSeparator(builder);
		builder.Append("HttpOnly");
	}

	if (m_samesite != SAMESITE_UNSEPC)
	{
		AppendSeparator(builder);
		builder.Append("SameSite=");
		switch (m_samesite)
		{
		case SAMESITE_STRICT:
			builder.Append("Strict");
			break;
		case SAMESITE_LAX:
			builder.Append("Lax");
			break;
		case SAMESITE_NONE:
			builder.Append("None");
			break;
		}
	}

	return builder;
}