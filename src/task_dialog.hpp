#pragma once


namespace dumbnose {


template<class derived_t>
class task_dialog
{
public:
	task_dialog()
	{
		clicked_button = 0;
		checkbox_checked = FALSE;
		ZeroMemory(&tdc_, sizeof(TASKDIALOGCONFIG));
		tdc_.cbSize = sizeof(TASKDIALOGCONFIG);
	}

	~task_dialog()
	{
	}

	// default callback handler
	//
	//typedef enum _TASKDIALOG_NOTIFICATIONS
	//{
	//	TDN_CREATED                         = 0,
	//	TDN_NAVIGATED                       = 1,
	//	TDN_BUTTON_CLICKED                  = 2,            // wParam = Button ID
	//	TDN_HYPERLINK_CLICKED               = 3,            // lParam = (LPCWSTR)pszHREF
	//	TDN_TIMER                           = 4,            // wParam = Milliseconds since dialog created or timer reset
	//	TDN_DESTROYED                       = 5,
	//	TDN_RADIO_BUTTON_CLICKED            = 6,            // wParam = Radio Button ID
	//	TDN_DIALOG_CONSTRUCTED              = 7,
	//	TDN_VERIFICATION_CLICKED            = 8,             // wParam = 1 if checkbox checked, 0 if not, lParam is unused and always 0
	//	TDN_HELP                            = 9,
	//	TDN_EXPANDO_BUTTON_CLICKED          = 10            // wParam = 0 (dialog is now collapsed), wParam != 0 (dialog is now expanded)
	//} TASKDIALOG_NOTIFICATIONS;
	static HRESULT CALLBACK callback(HWND hwnd, UINT uNotification, WPARAM wParam, LPARAM lParam, LONG_PTR dwRefData)
	{
		switch (uNotification)
			{
			case TDN_HYPERLINK_CLICKED:
				ShellExecute(hwnd, L"open", (LPCWSTR) lParam, NULL, NULL, SW_SHOW);
			break;
			}

		return S_OK;
	}


	void show()
	{
		if(custom_buttons_.size() > 0) {
			tdc_.pButtons = &custom_buttons_[0];
		}
		tdc_.cButtons = custom_buttons_.size();
		tdc_.pfCallback = &derived_t::callback;

		HRESULT hr = TaskDialogIndirect(&tdc_, &clicked_button, NULL, &checkbox_checked);
		if(FAILED(hr)) throw windows_exception("TaskDialogIndirect() failed", hr);
	}

	TASKDIALOGCONFIG& tdc()
	{
		return tdc_;
	}

	void add_button(TASKDIALOG_BUTTON& button)
	{
		custom_buttons_.push_back(button);
	}

private:
	std::vector<TASKDIALOG_BUTTON> custom_buttons_;
	int clicked_button;
	BOOL checkbox_checked;
	TASKDIALOGCONFIG tdc_;

};

} // namespace dumbnose
