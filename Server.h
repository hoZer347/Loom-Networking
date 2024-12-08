#pragma once

#include "Utils.h"

#include <boost/asio.hpp>

#include <iostream>
#include <string>
#include <queue>
#include <mutex>

#ifndef TCP_HOST_IP_PORT
#define TCP_HOST_IP_PORT 80
#endif


namespace Loom
{
	struct Server : public RAIIPersistentNetworkObject
	{
	protected:
		Server(const std::string& project_directory) :
			project_directory(project_directory)
		{ };

		const std::string project_directory;

	private:
		std::recursive_mutex mutex;
	};

	struct TCPServer final : public Server
	{
		TCPServer(const std::string& project_directory) :
			Server(project_directory),
			acceptor(
				io_context,
				boost::asio::ip::tcp::endpoint(
					boost::asio::ip::tcp::v4(),
					TCP_HOST_IP_PORT))
		{ };

		void Update();

	private:
		boost::asio::ip::tcp::acceptor acceptor;
	};

	struct UDPServer final : public Server
	{
		UDPServer(const std::string& project_directory) :
			Server(project_directory)
		{ };

		void Update();
	};
};
