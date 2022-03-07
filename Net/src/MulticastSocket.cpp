//
// MulticastSocket.cpp
//
// Library: Net
// Package: Sockets
// Module:  MulticastSocket
//
// Copyright (c) 2005-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#include "Poco/Net/MulticastSocket.h"


#ifdef POCO_NET_HAS_INTERFACE


#include "Poco/Net/NetException.h"
#include <cstring>


#if defined(hpux) && defined(_XOPEN_SOURCE_EXTENDED) && defined(POCO_HPUX_IP_MREQ_HACK)
// netinet/in.h does not define struct ip_mreq if
// _XOPEN_SOURCE_EXTENDED is #define'd in HP-UX 11.x 
// versions prior to 11.30. Compile with -DPOCO_HPUX_IP_MREQ_HACK
// if you experience problems.
struct ip_mreq 
{
	struct in_addr imr_multiaddr;
	struct in_addr imr_interface;
};
#endif


// some Unix variants don't have IPV6_ADD_MEMBERSHIP/IPV6_DROP_MEMBERSHIP
#if defined(IPV6_JOIN_GROUP) && !defined(IPV6_ADD_MEMBERSHIP)
#define IPV6_ADD_MEMBERSHIP  IPV6_JOIN_GROUP
#define IPV6_DROP_MEMBERSHIP IPV6_LEAVE_GROUP
#endif


namespace Poco {
namespace Net {


MulticastSocket::MulticastSocket()
{
}


MulticastSocket::MulticastSocket(SocketAddress::Family family): DatagramSocket(family)
{
#if defined(POCO_OS_FAMILY_UNIX)
	if (family == SocketAddress::UNIX_LOCAL)
		throw Poco::InvalidArgumentException("Cannot create a MulticastSocket with UNIX_LOCAL socket");
#endif
}


MulticastSocket::MulticastSocket(const SocketAddress& address, bool reuseAddress): DatagramSocket(address, reuseAddress)
{
}


MulticastSocket::MulticastSocket(const Socket& socket): DatagramSocket(socket)
{
}


MulticastSocket::~MulticastSocket()
{
}


MulticastSocket& MulticastSocket::operator = (const Socket& socket)
{
	DatagramSocket::operator = (socket);
	return *this;
}


void MulticastSocket::setInterface(const NetworkInterface& interfc)
{
	if (address().family() == SocketAddress::IPv4)
	{
		impl()->setOption(IPPROTO_IP, IP_MULTICAST_IF, interfc.firstAddress(IPAddress::IPv4));
	}
#if defined(POCO_HAVE_IPv6)
	else if (address().family() == SocketAddress::IPv6)
	{
		impl()->setOption(IPPROTO_IPV6, IPV6_MULTICAST_IF, interfc.index());
	}
#endif
	else throw UnsupportedFamilyException("Unknown or unsupported socket family.");
}

	
NetworkInterface MulticastSocket::getInterface() const
{
	try
	{
		IPAddress addr;
		impl()->getOption(IPPROTO_IP, IP_MULTICAST_IF, addr);
		return NetworkInterface::forAddress(addr);
	}
	catch (Poco::Exception&)
	{
#if defined(POCO_HAVE_IPv6)
		int ix;
		impl()->getOption(IPPROTO_IPV6, IPV6_MULTICAST_IF, ix);
		return NetworkInterface::forIndex(ix);
#else
		throw;
#endif
	}
}

	
void MulticastSocket::setLoopback(bool flag)
{
#ifdef USE_LIBZT
	if (address().af() == ZTS_AF_INET)
#else
	if (address().af() == AF_INET)
#endif
	{
		unsigned char uflag = flag ? 1 : 0;
#ifdef USE_LIBZT
		impl()->setOption(ZTS_IPPROTO_IP, ZTS_IP_MULTICAST_LOOP, uflag);
#else
		impl()->setOption(IPPROTO_IP, IP_MULTICAST_LOOP, uflag);
#endif
	}
	else
	{
#if defined(POCO_HAVE_IPv6)
		unsigned uflag = flag ? 1 : 0;
#ifdef USE_LIBZT
		assert(false);
		// impl()->setOption(ZTS_IPPROTO_IPV6, ZTS_IPV6_MULTICAST_LOOP, uflag);
#else
		impl()->setOption(IPPROTO_IPV6, IPV6_MULTICAST_LOOP, uflag);
#endif
#endif
	}
}

	
bool MulticastSocket::getLoopback() const
{
	bool flag = false;
#ifdef USE_LIBZT
	if (address().af() == ZTS_AF_INET)
#else
	if (address().af() == AF_INET)
#endif
	{
		unsigned char uflag;
#ifdef USE_LIBZT
		impl()->getOption(ZTS_IPPROTO_IP, ZTS_IP_MULTICAST_LOOP, uflag);
#else
		impl()->getOption(IPPROTO_IP, IP_MULTICAST_LOOP, uflag);
#endif
		flag = uflag != 0;
	}
	else
	{
#if defined(POCO_HAVE_IPv6)
		unsigned uflag;
#ifdef USE_LIBZT
		assert(false);
		// impl()->getOption(ZTS_IPPROTO_IPV6, ZTS_IPV6_MULTICAST_LOOP, uflag);
#else
		impl()->getOption(IPPROTO_IPV6, IPV6_MULTICAST_LOOP, uflag);
#endif
		flag = uflag != 0;
#endif
	}
	return flag;
}


void MulticastSocket::setTimeToLive(unsigned value)
{
#ifdef USE_LIBZT
	if (address().af() == ZTS_AF_INET)
#else
	if (address().af() == AF_INET)
#endif
	{
		unsigned char ttl = (unsigned char) value;
#ifdef USE_LIBZT
		impl()->setOption(ZTS_IPPROTO_IP, ZTS_IP_MULTICAST_TTL, ttl);
#else
		impl()->setOption(IPPROTO_IP, IP_MULTICAST_TTL, ttl);
#endif
	}
	else
	{
#if defined(POCO_HAVE_IPv6)
#ifdef USE_LIBZT
		assert(false);
		// impl()->setOption(ZTS_IPPROTO_IPV6, ZTS_IPV6_MULTICAST_HOPS, value);
#else
		impl()->setOption(IPPROTO_IPV6, IPV6_MULTICAST_HOPS, value);
#endif
#endif
	}
}

	
unsigned MulticastSocket::getTimeToLive() const
{
	unsigned ttl(0);
#ifdef USE_LIBZT
	if (address().af() == ZTS_AF_INET)
#else
	if (address().af() == AF_INET)
#endif
	{
		unsigned char cttl;
#ifdef USE_LIBZT
		impl()->getOption(ZTS_IPPROTO_IP, ZTS_IP_MULTICAST_TTL, cttl);
#else
		impl()->getOption(IPPROTO_IP, IP_MULTICAST_TTL, cttl);
#endif
		ttl = cttl;
	}
	else
	{
#if defined(POCO_HAVE_IPv6)
#ifdef USE_LIBZT
		assert(false);
		// impl()->getOption(ZTS_IPPROTO_IPV6, ZTS_IPV6_MULTICAST_HOPS, ttl);
#else
		impl()->getOption(IPPROTO_IPV6, IPV6_MULTICAST_HOPS, ttl);
#endif
#endif
	}
	return ttl;
}

	
void MulticastSocket::joinGroup(const IPAddress& groupAddress)
{
	joinGroup(groupAddress, findFirstInterface(groupAddress));
}

	
void MulticastSocket::joinGroup(const IPAddress& groupAddress, const NetworkInterface& interfc)
{
#ifdef USE_LIBZT
	if (groupAddress.af() == ZTS_AF_INET)
#else
	if (groupAddress.af() == AF_INET)
#endif
	{
#ifdef USE_LIBZT
		struct zts_ip_mreq mr;
#else
		struct ip_mreq mr;
#endif
		std::memcpy(&mr.imr_multiaddr, groupAddress.addr(), groupAddress.length());
		std::memcpy(&mr.imr_interface, interfc.firstAddress(IPAddress::IPv4).addr(), interfc.firstAddress(IPAddress::IPv4).length());
#ifdef USE_LIBZT
		impl()->setRawOption(IPPROTO_IP, IP_ADD_MEMBERSHIP, &mr, sizeof(mr));
#else
		impl()->setRawOption(IPPROTO_IP, IP_ADD_MEMBERSHIP, &mr, sizeof(mr));
#endif
	}
	else
	{
#if defined(POCO_HAVE_IPv6)
#ifdef USE_LIBZT
		struct zts_ipv6_mreq mr;
#else
		struct ipv6_mreq mr;
#endif
		std::memcpy(&mr.ipv6mr_multiaddr, groupAddress.addr(), groupAddress.length());
		mr.ipv6mr_interface = interfc.index();
#ifdef USE_LIBZT
		impl()->setRawOption(ZTS_IPPROTO_IPV6, ZTS_IPV6_ADD_MEMBERSHIP, &mr, sizeof(mr));
#else
		impl()->setRawOption(IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &mr, sizeof(mr));
#endif
#endif
	}
}


NetworkInterface MulticastSocket::findFirstInterface(const IPAddress& groupAddress)
{
	NetworkInterface::Map m = NetworkInterface::map();
	if (groupAddress.family() == IPAddress::IPv4)
	{
		for (const auto& p: m)
		{
			if (p.second.supportsIPv4() &&
				p.second.firstAddress(IPAddress::IPv4).isUnicast() &&
				!p.second.isLoopback() &&
				!p.second.isPointToPoint())
			{
				return p.second;
			}
		}
	}
#ifdef POCO_HAVE_IPv6
	else if (groupAddress.family() == IPAddress::IPv6)
	{
		for (const auto& p: m)
		{
			if (p.second.supportsIPv6() &&
				p.second.firstAddress(IPAddress::IPv6).isUnicast() &&
				!p.second.isLoopback() &&
				!p.second.isPointToPoint())
			{
				return p.second;
			}
		}
	}
#endif // POCO_HAVE_IPv6

	throw NotFoundException("No multicast-eligible network interface found.");
}

	
void MulticastSocket::leaveGroup(const IPAddress& groupAddress)
{
	NetworkInterface intf;
	leaveGroup(groupAddress, intf);
}

	
void MulticastSocket::leaveGroup(const IPAddress& groupAddress, const NetworkInterface& interfc)
{
#ifdef USE_LIBZT
	if (groupAddress.af() == ZTS_AF_INET)
#else
	if (groupAddress.af() == AF_INET)
#endif
	{
#ifdef USE_LIBZT
		struct zts_ip_mreq mr;
#else
		struct ip_mreq mr;
#endif
		std::memcpy(&mr.imr_multiaddr, groupAddress.addr(), groupAddress.length());
		std::memcpy(&mr.imr_interface, interfc.firstAddress(IPAddress::IPv4).addr(), interfc.firstAddress(IPAddress::IPv4).length());
#ifdef USE_LIBZT
		impl()->setRawOption(ZTS_IPPROTO_IP, ZTS_IP_DROP_MEMBERSHIP, &mr, sizeof(mr));
#else
		impl()->setRawOption(IPPROTO_IP, IP_DROP_MEMBERSHIP, &mr, sizeof(mr));
#endif
	}
	else
	{
#if defined(POCO_HAVE_IPv6)
#ifdef USE_LIBZT
		struct zts_ipv6_mreq mr;
#else
		struct ipv6_mreq mr;
#endif
		std::memcpy(&mr.ipv6mr_multiaddr, groupAddress.addr(), groupAddress.length());
		mr.ipv6mr_interface = interfc.index();
#ifdef USE_LIBZT
		impl()->setRawOption(ZTS_IPPROTO_IPV6, ZTS_IPV6_DROP_MEMBERSHIP, &mr, sizeof(mr));
#else
		impl()->setRawOption(IPPROTO_IPV6, IPV6_DROP_MEMBERSHIP, &mr, sizeof(mr));
#endif
#endif
	}
}


} } // namespace Poco::Net


#endif // POCO_NET_HAS_INTERFACE
