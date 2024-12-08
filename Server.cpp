#include "Server.h"

#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>
#include <boost/regex.hpp>

#include <filesystem>
#include <iostream>
#include <fstream>
#include <atomic>
#include <thread>

using boost::asio::ip::tcp;
using boost::asio::ip::udp;


namespace Loom
{
	// Ensure OpenSSL is initialized only once
	static bool ssl_initialized = false;
	static std::mutex ssl_initilization_mutex;

	// TODO: Add to string utils
	int RemoveSubstrings(
		std::string& str,
		const std::string& substr)
	{
		int count = 0;
		size_t pos = str.find(substr);
		while (pos != std::string::npos) {
			str.erase(pos, substr.length());
			++count;
			pos = str.find(substr);
		};
		
		return count;
	};

	// TODO: Add to string utils
	int ReplaceSubstrings(
		std::string& str,
		const std::string& substr,
		const std::string& replacement)
	{
		int count = 0;
		size_t pos = str.find(substr);
		while (pos != std::string::npos) {
			str.replace(pos, substr.length(), replacement);
			++count;
			pos = str.find(substr, pos + replacement.length());
		};

		return count;
	};

	void TCPServer::Update()
	{
		//boost::asio::ssl::context ssl_context(boost::asio::ssl::context::sslv23);
		//boost::asio::ssl::stream<tcp::socket> socket(io_context, ssl_context);
		tcp::socket socket{ io_context };

		std::cout
			<< std::endl
			<< "Waiting for connection..."
			<< std::endl;

		//acceptor.accept(socket.next_layer());
		//socket.handshake(boost::asio::ssl::stream_base::server);

		acceptor.accept(socket);

		std::cout
			<< "Accepted connection from: "
			<< socket.remote_endpoint().address().to_string()
			<< std::endl;


		// Reading request from the client
		boost::asio::streambuf request;
		boost::asio::read_until(socket, request, "\r\n");
		std::istream request_stream(&request);
		//std::cout << std::endl << &request;
		//


		// Interpretting Request
		std::string method, path, version;
		request_stream >> method >> path >> version;
		std::cout << "Method:  " << method << std::endl;
		std::cout << "Path:    " << path << std::endl;
		std::cout << "Version: " << version << std::endl;
		//


		// Sanitization
		if (path == "/") path = project_directory + "/index.html";

		else
		{
			ReplaceSubstrings(path, "%20", " ");
			//ReplaceSubstrings(path, "%PUBLIC_URL%", "public");
			
			if (path.contains("..")	||
				path.contains("?")	||
				path.contains("=")  ||
				path.contains("%"))
			{
				std::cerr << "Requested path contains a dangerous character, ending request out of abundance of caution" << std::endl;
				return;
			};

			if (boost::regex_match(path, boost::regex("[a-zA-Z0-9_]+")))
			{
				std::cerr << "Requested path did not pass the regex test" << std::endl;
				return;
			};

			std::cout << "Relative: "
				<< std::filesystem::relative(
					std::filesystem::canonical(project_directory + path),
					std::filesystem::canonical(project_directory))
				<< std::endl;

			path = project_directory + path;
		};
		
		std::cout << "Attempting to find: " << path << std::endl;
		//


		// Set Content-Type based on the requested file
		std::string content_type;
		std::string content_encoding;

		if      (path.ends_with(".html"))											content_type = "text/html";\
		else if (path.ends_with(".css"))											content_type = "text/css";
		else if (path.ends_with(".js"))												content_type = "application/javascript";
		else if (path.ends_with(".wasm"))											content_type = "application/wasm";
		else if (path.ends_with(".png"))											content_type = "image/png";
		else if (path.ends_with(".jpg") || path.ends_with(".jpeg"))					content_type = "image/jpeg";
		else if (path.ends_with(".txt"))											content_type = "text/plain";
		else if (path.ends_with(".ico"))											content_type = "image/x-icon";
		else if (path.ends_with(".gz"))
		{
			content_type = "application/octet-stream";
			content_encoding = "gzip";
		}
		else
		{
			std::cerr << "Requested an invalid type of file" << std::endl;
			return;
		};
		//


		// Always send back index.html
		std::ifstream file(path, std::ios::binary);
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string content = buffer.str();
		std::string response = "HTTP/1.1 200 OK\r\n";
		response += "Content-Type: text/html\r\n";
		response += "Content Length: " + std::to_string(content.size()) + "\r\n";
		if (content_encoding != "") response += "Content-Encoding: " + content_encoding + "\r\n";
		response += "\r\n";
		response += content + "\r\n";
		boost::asio::write(socket, boost::asio::buffer(response));
		std::cout << "Sent:               " << path << std::endl;
		//
	};

	void UDPServer::Update()
	{
		std::cout << "Buh?" << std::endl;
	};
};
