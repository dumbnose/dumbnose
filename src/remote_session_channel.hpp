#pragma once

#include "remote_channel.hpp"
#include <functional>
#include <map>
#include <optional>
#include <vector>
#include <winrt\Windows.System.RemoteSystems.h>
#include "event_source.hpp"
#include "status_notifier.hpp"


namespace rs = winrt::Windows::System::RemoteSystems;
namespace cx_rs = winrt::Windows::System::RemoteSystems;
namespace ph = std::placeholders;


namespace dumbnose {

	class remote_session_channel : public remote_channel
	{
	public:
		std::wstring session_display_name_;
		std::wstring channel_display_name_;
		remote_session_channel(std::wstring const & session_display_name, std::wstring const & channel_display_name) : session_display_name_(session_display_name), channel_display_name_(channel_display_name)
		{
		}


		std::future<void> send_message(wc::ValueSet& message) override
		{
			auto succeeded = co_await channel_.BroadcastValueSetAsync(message);
			if (!succeeded) throw std::exception("Send message FAILED.");
		}


		std::future<void> start_async()
		{
			if (started_) return;

			try {

				status_notifier::instance().trace(L"Advertising session...");

				// advertise to potential opponents
				controller_ = rs::RemoteSystemSessionController(session_display_name_);
				controller_.JoinRequested(std::bind(&remote_session_channel::join_requested, this, ph::_1, ph::_2));

				auto result = co_await controller_.CreateSessionAsync();
				if (result.Status() != rs::RemoteSystemSessionCreationStatus::Success) throw std::exception((boost::format("Session creation failed with result = %d") % static_cast<int32_t>(result.Status())).str().c_str());

				session_ = result.Session();
				status_notifier::instance().update_status((boost::wformat(L"System name = %s") % session_.ControllerDisplayName().c_str()).str());

				channel_ = rs::RemoteSystemSessionMessageChannel(session_, L"game");
				channel_.ValueSetReceived(std::bind(&remote_session_channel::message_received, this, ph::_1, ph::_2));

				// look for potential opponents
				watcher_ = rs::RemoteSystemSession::CreateWatcher();
				watcher_.Added(std::bind(&remote_session_channel::remote_session_added, this, ph::_1, ph::_2));
				watcher_.Removed(std::bind(&remote_session_channel::remote_session_removed, this, ph::_1, ph::_2));

				watcher_.Start();

				started_ = true;

			}
			catch (std::exception& ex) {
				stop();
				status_notifier::instance().report_exception(ex);
			}
		}


		void stop()
		{
			if (started_) watcher_.Stop();
			started_ = false;
			clear_list();
		}


		bool is_controller() { return is_controller_;  }
		bool is_connected() { return connected_; }

		
		std::future<bool> connect(rs::RemoteSystemSessionInfo opponent)
		{
			status_notifier::instance().trace(L"Session connecting...");

			std::unique_lock<std::mutex> lock(session_lock_);
			if (connected_) return false;

			rs::RemoteSystemSessionJoinResult result = co_await opponent.JoinAsync();
			if (result.Status() != rs::RemoteSystemSessionJoinStatus::Success) {
				status_notifier::instance().report_error(L"Challenge was rejected");
				return false;
			}

			stop();  // No longer need to advertise, now that a join has been requested

			session_ = result.Session();
			channel_ = rs::RemoteSystemSessionMessageChannel(session_, L"game");
			channel_.ValueSetReceived(std::bind(&remote_session_channel::message_received, this, ph::_1, ph::_2));

			connected_ = true;
			is_controller_ = false;

			status_notifier::instance().trace(L"Challenge was accepted.");

			return true;
		}


		struct session_connected_event_args {
			rs::RemoteSystemSessionInfo session_info;
			//connect4_board::piece_color color;
		};

		event_source<remote_session_channel &, void*> session_list_changed;
		std::vector<rs::RemoteSystemSessionInfo>  SessionList() { return sessions_; }

		event_source<remote_session_channel &, void*> session_joined;

	private:
		std::vector<rs::RemoteSystemSessionInfo> sessions_;
		std::mutex sessions_lock_;

		rs::RemoteSystemSessionWatcher watcher_ = nullptr;
		rs::RemoteSystemSessionController controller_ = nullptr;

		rs::RemoteSystemSession session_ = nullptr;
		rs::RemoteSystemSessionMessageChannel channel_ = nullptr;
		std::mutex session_lock_;
		bool is_controller_ = false;  // indicates whether this instance is the controller or participant
		bool started_ = false;
		bool connected_ = false;
		bool canceled_ = false;


		void remote_session_added(rs::RemoteSystemSessionWatcher sender, rs::RemoteSystemSessionAddedEventArgs args)
		{
			if (args.SessionInfo().DisplayName() == session_display_name_) {
				status_notifier::instance().trace(L"Remote session added.");
				std::unique_lock<std::mutex> lock(sessions_lock_);
				sessions_.push_back(args.SessionInfo());
			}

			update_list();
		}


		// Comparator so we can find a RemoteSystem by Id in a vector 
		struct remote_session_equal_comparator
		{
			rs::RemoteSystemSessionInfo compare_to_;
			remote_session_equal_comparator(rs::RemoteSystemSessionInfo compare_to) : compare_to_(compare_to) {}

			bool operator()(rs::RemoteSystemSessionInfo candidate)
			{
				return candidate.ControllerDisplayName() == compare_to_.ControllerDisplayName();
			}
		};

		
		void remote_session_removed(rs::RemoteSystemSessionWatcher sender, rs::RemoteSystemSessionRemovedEventArgs args)
		{
			status_notifier::instance().trace(L"Remote session removed.");

			{
				std::unique_lock<std::mutex> lock(sessions_lock_);
				sessions_.erase(std::find_if(sessions_.begin(), sessions_.end(), remote_session_equal_comparator(args.SessionInfo())));
			}

			update_list();
		}
		
		void join_requested(rs::RemoteSystemSessionController sender, rs::RemoteSystemSessionJoinRequestedEventArgs args)
		{
			status_notifier::instance().trace(L"Joing requested.");

			stop();  // No longer need to advertise, now that a join has been requested

			std::unique_lock<std::mutex> lock(session_lock_);
			if (connected_) return;

			args.JoinRequest().Accept();
			connected_ = true;
			is_controller_ = true;

			status_notifier::instance().trace(L"Join accepted.");

			session_joined.raise(*this, nullptr);
		}
		

		void message_received(rs::RemoteSystemSessionMessageChannel sender, rs::RemoteSystemSessionValueSetReceivedEventArgs args)
		{
			message_received_event.raise(this, args.Message());
		}


		void update_list()
		{
			std::unique_lock<std::mutex> lock(sessions_lock_);

			session_list_changed.raise(*this, nullptr);
		}


		void clear_list()
		{
			std::unique_lock<std::mutex> lock(sessions_lock_);

			sessions_.clear();
		}


	};

}
