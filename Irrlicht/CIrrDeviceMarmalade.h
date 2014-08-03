// Copyright (C) 2002-2011 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
// This device code is based on the original SDL device implementation
// contributed by Shane Parker (sirshane).

#ifndef __C_IRR_DEVICE_SDL_H_INCLUDED__
#define __C_IRR_DEVICE_SDL_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_MARMALADE_PLATFORM_

#include "IrrlichtDevice.h"
#include "CIrrDeviceStub.h"
#include "IImagePresenter.h"
#include "ICursorControl.h"
#include "s3e.h"

namespace irr
{

	class CIrrDeviceMarmalade : public CIrrDeviceStub, video::IImagePresenter
	{
	public:

		//! constructor
		CIrrDeviceMarmalade(const SIrrlichtCreationParameters& param);

		//! destructor
		virtual ~CIrrDeviceMarmalade();

		//! runs the device. Returns false if device wants to be deleted
		virtual bool run();

		//! pause execution temporarily
		virtual void yield();

		//! pause execution for a specified time
		virtual void sleep(u32 timeMs, bool pauseTimer);

		//! presents a surface in the client area
		virtual bool present(video::IImage* surface, void* windowId=0, core::rect<s32>* src=0);

		virtual void setWindowCaption(const wchar_t* text) {}

		virtual bool isWindowActive() const;
		virtual bool isWindowFocused() const;
		virtual bool isWindowMinimized() const;
		virtual void setResizable(bool resize=false) {}

		virtual void minimizeWindow() {}
		virtual void maximizeWindow() {}
		virtual void restoreWindow() {}
        virtual core::position2di getWindowPosition();

		virtual void closeDevice();

		//! Get the device type
		virtual E_DEVICE_TYPE getType() const
		{
			return EIDT_MARMALADE;
		}

	protected:
		void onKeyEvent(s3eKeyboardEvent*);
		void onCharEvent(s3eKeyboardCharEvent*);
		void onExit();
		void onPause();
		void onUnpause();

	private:
		bool Paused;

		static int32 do_Exit(void*,void*);
		static int32 do_Pause(void*,void*);
		static int32 do_Unpause(void*,void*);
		static int32 do_KeyEvent(void *,void*);
		static int32 do_CharEvent(void*,void*);
	};

} // end namespace irr

#endif // _IRR_MARMALADE_PLATFORM_H
#endif // __C_IRR_DEVICE_SDL_H_INCLUDED__

