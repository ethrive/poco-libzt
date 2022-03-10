//
// RegularExpression.h
//
// Library: Foundation
// Package: RegExp
// Module:  RegularExpression
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#include "Poco/RegularExpression.h"
#include "Poco/Exception.h"
#include <sstream>
#include <cassert>


namespace Poco {


const int RegularExpression::OVEC_SIZE = 63; // must be multiple of 3


RegularExpression::RegularExpression(const std::string& pattern, int options, bool study): _pcre(0), _extra(0)
{
	assert(false);
}


RegularExpression::~RegularExpression()
{
}


int RegularExpression::match(const std::string& subject, std::string::size_type offset, Match& mtch, int options) const
{
	poco_assert (offset <= subject.length());

	return 0;
}


int RegularExpression::match(const std::string& subject, std::string::size_type offset, MatchVec& matches, int options) const
{
	poco_assert (offset <= subject.length());

	return 0;
}


bool RegularExpression::match(const std::string& subject, std::string::size_type offset) const
{
	Match mtch;
	match(subject, offset, mtch, RE_ANCHORED | RE_NOTEMPTY);
	return mtch.offset == offset && mtch.length == subject.length() - offset;
}


bool RegularExpression::match(const std::string& subject, std::string::size_type offset, int options) const
{
	Match mtch;
	match(subject, offset, mtch, options);
	return mtch.offset == offset && mtch.length == subject.length() - offset;
}


int RegularExpression::extract(const std::string& subject, std::string& str, int options) const
{
	Match mtch;
	int rc = match(subject, 0, mtch, options);
	if (mtch.offset != std::string::npos)
		str.assign(subject, mtch.offset, mtch.length);
	else
		str.clear();
	return rc;
}


int RegularExpression::extract(const std::string& subject, std::string::size_type offset, std::string& str, int options) const
{
	Match mtch;
	int rc = match(subject, offset, mtch, options);
	if (mtch.offset != std::string::npos)
		str.assign(subject, mtch.offset, mtch.length);
	else
		str.clear();
	return rc;
}


int RegularExpression::split(const std::string& subject, std::string::size_type offset, std::vector<std::string>& strings, int options) const
{
	MatchVec matches;
	strings.clear();
	int rc = match(subject, offset, matches, options);
	strings.reserve(matches.size());
	for (const auto& m: matches)
	{
		if (m.offset != std::string::npos)
			strings.push_back(subject.substr(m.offset, m.length));
		else
			strings.push_back(std::string());
	}
	return rc;
}


int RegularExpression::subst(std::string& subject, std::string::size_type offset, const std::string& replacement, int options) const
{
	if (options & RE_GLOBAL)
	{
		int rc = 0;
		std::string::size_type pos = substOne(subject, offset, replacement, options);
		while (pos != std::string::npos)
		{
			++rc;
			pos = substOne(subject, pos, replacement, options);
		}
		return rc;
	}
	else
	{
		return substOne(subject, offset, replacement, options) != std::string::npos ? 1 : 0;
	}
}


std::string::size_type RegularExpression::substOne(std::string& subject, std::string::size_type offset, const std::string& replacement, int options) const
{
	if (offset >= subject.length()) return std::string::npos;

	return 0;
}


bool RegularExpression::match(const std::string& subject, const std::string& pattern, int options)
{
	int ctorOptions = options & (RE_CASELESS | RE_MULTILINE | RE_DOTALL | RE_EXTENDED | RE_ANCHORED | RE_DOLLAR_ENDONLY | RE_EXTRA | RE_UNGREEDY | RE_UTF8 | RE_NO_AUTO_CAPTURE);
	int mtchOptions = options & (RE_ANCHORED | RE_NOTBOL | RE_NOTEOL | RE_NOTEMPTY | RE_NO_AUTO_CAPTURE | RE_NO_UTF8_CHECK);
	RegularExpression re(pattern, ctorOptions, false);
	return re.match(subject, 0, mtchOptions);
}


} // namespace Poco
