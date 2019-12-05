#pragma once


#include <windows.h>
#include "../windows_exception.hpp"


namespace dumbnose { namespace gui {


template<class derived_t>
class window
{
public:

	window() : instance_(0),prev_instance_(0),main_window_(0)
	{
		ZeroMemory(&wc_,sizeof(wc_)); 
	}

	virtual ~window(){}

	HINSTANCE get_instance() { return instance_;}
	HWND get_main_window() { return main_window_;}


	virtual void initialize(HINSTANCE inst, HINSTANCE prev_inst)
	{
		instance_ = inst;
		prev_instance_ = prev_inst;

		register_class();
		create_window();
	}

	virtual void show(int nCmdShow)
	{
		ShowWindow(main_window_, nCmdShow); 
		UpdateWindow(main_window_); 
	}

	virtual void run()
	{
		MSG msg;
		int nRet;
		while ((nRet = GetMessage(&msg, 0, 0, 0)) != 0) {
			if (nRet == -1) throw windows_exception("GetMessage() failed", GetLastError());

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}


protected:

	void register_class()
	{
		if (!prev_instance_) 
		{ 
			wc_.style = 0; 
			wc_.lpfnWndProc = (WNDPROC) derived_t::window_proc;
			wc_.cbClsExtra = 0; 
			wc_.cbWndExtra = 0; 
			wc_.hInstance = instance_; 
			wc_.hIcon = LoadIcon((HINSTANCE) NULL, IDI_APPLICATION); 
			wc_.hCursor = LoadCursor((HINSTANCE) NULL, IDC_ARROW); 
			wc_.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); 
			wc_.lpszMenuName =  L"MainMenu"; 
			wc_.lpszClassName = L"MainWndClass"; 

			if (!RegisterClass(&wc_)) throw windows_exception("Could not RegisterClass()",GetLastError()); 
		} 
	}

	void create_window()
	{
		main_window_ = CreateWindow(L"MainWndClass", L"Sample", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 
									CW_USEDEFAULT, CW_USEDEFAULT, (HWND) NULL, (HMENU) NULL, instance_, (LPVOID) NULL); 
		if (!main_window_) throw windows_exception("CreateWindow() failed",GetLastError()); 
	}

	static LRESULT window_proc(HWND wnd,UINT msg,WPARAM wp,LPARAM lp)
	{
		switch(msg)
		{
			case WM_DESTROY: // Kill app when window closed
				PostQuitMessage(0); 
				break; 
		}

		return DefWindowProc(wnd,msg,wp,lp);
	}

// members (make private later)
	HINSTANCE instance_;
	HINSTANCE prev_instance_;
	HWND main_window_;
	WNDCLASS wc_;
};

}} // namespace dumbnose::gui

