#pragma once

#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>
#include <thread>


namespace Loom
{
	// Maintains a looping update function that starts and ends with the lifetime of this object
	struct RAIIPeristentThreadedObject
	{
		virtual ~RAIIPeristentThreadedObject();

		void WaitUntilDone();

	protected:
		RAIIPeristentThreadedObject();
		virtual void Update() = 0;

	private:
		std::thread thread;
		bool isRunning = true;
	};

	// Maintains a looping update function that starts and ends with the lifetime of this object
	// Contains a io_context for network operations
	struct RAIIPersistentNetworkObject :
		public RAIIPeristentThreadedObject
	{
		virtual ~RAIIPersistentNetworkObject();

		boost::asio::io_context io_context{ };
	};
};
