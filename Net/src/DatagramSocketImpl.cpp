//
// DatagramSocketImpl.cpp
//
// Library: Net
// Package: Sockets
// Module:  DatagramSocketImpl
//
// Copyright (c) 2005-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#include "Poco/Net/DatagramSocketImpl.h"
#include "Poco/Net/NetException.h"


using Poco::InvalidArgumentException;


namespace Poco {
namespace Net {


DatagramSocketImpl::DatagramSocketImpl()
{
}


DatagramSocketImpl::DatagramSocketImpl(SocketAddress::Family family)
{
	if (family == SocketAddress::IPv4)
#ifdef USE_LIBZT
		init(ZTS_AF_INET);
#else
		init(AF_INET);
#endif
#if defined(POCO_HAVE_IPv6)
	else if (family == SocketAddress::IPv6)
#ifdef USE_LIBZT
		init(ZTS_AF_INET6);
#else
		init(AF_INET6);
#endif
#endif
#if defined(POCO_OS_FAMILY_UNIX)
	else if (family == SocketAddress::UNIX_LOCAL)
		init(AF_UNIX);
#endif
	else throw InvalidArgumentException("Invalid or unsupported address family passed to DatagramSocketImpl");
}


DatagramSocketImpl::DatagramSocketImpl(poco_socket_t sockfd): SocketImpl(sockfd)
{
}


DatagramSocketImpl::~DatagramSocketImpl()
{
}


void DatagramSocketImpl::init(int af)
{
	initSocket(af, SOCK_DGRAM);
}


} } // namespace Poco::Net
