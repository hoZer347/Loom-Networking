import Utils;

import <iostream>;


namespace Loom
{
	RAIIPeristentThreadedObject::RAIIPeristentThreadedObject() :
		thread(
			[&]()
			{
				while (isRunning)
					try
					{
						Update();
					}
					catch (const std::exception& ex)
					{
						std::cerr << ex.what() << std::endl;
					};
			})
	{ };

	RAIIPeristentThreadedObject::~RAIIPeristentThreadedObject()
	{
		isRunning = false;
		if (thread.joinable())
			thread.join();
	};

	RAIIPersistentNetworkObject::~RAIIPersistentNetworkObject()
	{
		io_context.stop();
	};

	void RAIIPeristentThreadedObject::WaitUntilDone()
	{
		if (thread.joinable())
			thread.join();
	};
};
