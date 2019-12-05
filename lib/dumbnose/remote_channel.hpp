#pragma once

#include <future>
#include <winrt\Windows.Foundation.Collections.h>
#include "event_source.hpp"


namespace wc = winrt::Windows::Foundation::Collections;


namespace dumbnose {


	class remote_channel
	{
	public:
		virtual ~remote_channel() {}

		virtual std::future<void> send_message(wc::ValueSet& message) = 0;

		// events
		event_source<remote_channel*, wc::ValueSet&> message_received_event;
		event_source<remote_channel*, void*> channel_closed_event;
	};


}
