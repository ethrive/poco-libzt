//
// httpget.cpp
//
// This sample demonstrates the HTTPClientSession and the HTTPCredentials classes.
//
// Copyright (c) 2005-2012, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPCredentials.h"
#include "Poco/StreamCopier.h"
#include "Poco/NullStream.h"
#include "Poco/Path.h"
#include "Poco/URI.h"
#include "Poco/Exception.h"
#include <iostream>


using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPMessage;
using Poco::StreamCopier;
using Poco::Path;
using Poco::URI;
using Poco::Exception;


bool doRequest(Poco::Net::HTTPClientSession& session, Poco::Net::HTTPRequest& request, Poco::Net::HTTPResponse& response)
{
	session.sendRequest(request);
	std::istream& rs = session.receiveResponse(response);
	std::cout << response.getStatus() << " " << response.getReason() << std::endl;
	if (response.getStatus() != Poco::Net::HTTPResponse::HTTP_UNAUTHORIZED)
	{
		StreamCopier::copyStream(rs, std::cout);
		return true;
	}
	else
	{
		Poco::NullOutputStream null;
		StreamCopier::copyStream(rs, null);
		return false;
	}
}

#if USE_LIBZT
#include <ZeroTierSockets.h>
#endif

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		Path p(argv[0]);
		std::cout << "usage: " << p.getBaseName() << " <uri>" << std::endl;
		std::cout << "       fetches the resource identified by <uri> and print it to the standard output" << std::endl;
		return 1;
	}

#if USE_LIBZT
		char storage_path[] = "0";
		long long unsigned net_id = 0x35c192ce9b366ea2LL;
		char local_addr[] = "0.0.0.0";
		unsigned int local_port = 8080;
		int fd, accfd;
		int err = ZTS_ERR_OK;
		// Initialize node
		if ((err = zts_init_from_storage(storage_path)) != ZTS_ERR_OK)
		{
			printf("Unable to start service, error = %d. Exiting.\n", err);
			exit(1);
		}

		// Start node
		if ((err = zts_node_start()) != ZTS_ERR_OK)
		{
			printf("Unable to start service, error = %d. Exiting.\n", err);
			exit(1);
		}
		printf("Waiting for node to come online\n");
		while (!zts_node_is_online())
		{
			zts_util_delay(50);
		}
		printf("Public identity (node ID) is %llx\n", zts_node_get_id());
		// Join network
		printf("Joining network %llx\n", net_id);
		if (zts_net_join(net_id) != ZTS_ERR_OK)
		{
			printf("Unable to join network. Exiting.\n");
			exit(1);
		}
		printf("Don't forget to authorize this device in my.zerotier.com or the web API!\n");
		printf("Waiting for join to complete\n");
		while (!zts_net_transport_is_ready(net_id))
		{
			zts_util_delay(50);
		}
		// Get assigned address (of the family type we care about)
		int family = zts_util_get_ip_family(local_addr);
		printf("Waiting for address assignment from network\n");
		while (!(err = zts_addr_is_assigned(net_id, family)))
		{
                  zts_util_delay(50);
		}
		char ipstr[ZTS_IP_MAX_STR_LEN] = {0};
		zts_addr_get_str(net_id, family, ipstr, ZTS_IP_MAX_STR_LEN);
		printf("IP address on network %llx is %s\n", net_id, ipstr);
		zts_util_delay(5000);
#endif

	try
	{
		while (1) {
			try {
				URI uri(argv[1]);
				std::string path(uri.getPathAndQuery());
				if (path.empty())
					path = "/";

				std::string username;
				std::string password;
				Poco::Net::HTTPCredentials::extractCredentials(uri, username,
															 password);
				Poco::Net::HTTPCredentials credentials(username, password);

				HTTPClientSession session(uri.getHost(), uri.getPort());
				HTTPRequest request(HTTPRequest::HTTP_GET, path,
									HTTPMessage::HTTP_1_1);
				HTTPResponse response;
				if (!doRequest(session, request, response)) {
				credentials.authenticate(request, response);
				if (!doRequest(session, request, response)) {
					std::cerr << "Invalid username or password" << std::endl;
					return 1;
				}
				}
			} catch (std::exception& e) {
				std::cout << "exception: " << e.what() << std::endl;
			}
			Poco::Thread::sleep(1000);
			}
	}
	catch (Exception& exc)
	{
		std::cerr << exc.displayText() << std::endl;
		return 1;
	}
	return 0;
}
