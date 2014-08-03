// Copyright (C) 2012 Codemasters Software Ltd.
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"

#ifdef _IRR_MARMALADE_PLATFORM_

#include "CIrrDeviceMarmalade.h"
#include "IEventReceiver.h"
#include "irrList.h"
#include "os.h"
#include "CTimer.h"
#include "irrString.h"
#include "Keycodes.h"
#include "COSOperator.h"
#include "COGLES2Driver.h"
#include "ICursorControl.h"
#include <stdio.h>
#include <stdlib.h>
#include "SIrrCreationParameters.h"
#include "s3e.h"
#include "IwGL.h"
#include "IwUtil.h"

namespace irr	
{
	namespace video
	{
        IVideoDriver* createOGLES1Driver(const SIrrlichtCreationParameters& params,
            io::IFileSystem* io);

        IVideoDriver* createOGLES2Driver(const SIrrlichtCreationParameters& params,
            io::IFileSystem* io);

        /*
		IVideoDriver* createOGLES1Driver(const SIrrlichtCreationParameters& params,	
			video::SExposedVideoData& data, io::IFileSystem* io);

		IVideoDriver* createOGLES2Driver(const SIrrlichtCreationParameters& params,
			video::SExposedVideoData& data, io::IFileSystem* io);
            */
	}

	static const int key_map[] = {
		s3eKeyEsc,			KEY_ESCAPE,
		s3eKeyTab,			KEY_TAB,
		s3eKeyBackspace,	KEY_BACK,
		s3eKeyEnter,		KEY_RETURN,
		s3eKeyLeftShift,	KEY_SHIFT,
		s3eKeyLeftControl,	KEY_CONTROL,
		s3eKeySpace,		KEY_SPACE,
		s3eKeyLeft,			KEY_LEFT,
		s3eKeyUp,			KEY_UP,
		s3eKeyRight,		KEY_RIGHT,
		s3eKeyDown,			KEY_DOWN,

		s3eKey0,			KEY_KEY_0,
		s3eKey1,			KEY_KEY_1,
		s3eKey2,			KEY_KEY_2,
		s3eKey3,			KEY_KEY_3,
		s3eKey4,			KEY_KEY_4,
		s3eKey5,			KEY_KEY_5,
		s3eKey6,			KEY_KEY_6,
		s3eKey7,			KEY_KEY_7,
		s3eKey8,			KEY_KEY_8,
		s3eKey9,			KEY_KEY_9,

		s3eKeyA,			KEY_KEY_A,
		s3eKeyB,			KEY_KEY_B,
		s3eKeyC,			KEY_KEY_C,
		s3eKeyD,			KEY_KEY_D,
		s3eKeyE,			KEY_KEY_E,
		s3eKeyF,			KEY_KEY_F,
		s3eKeyG,			KEY_KEY_G,
		s3eKeyH,			KEY_KEY_H,
		s3eKeyI,			KEY_KEY_I,
		s3eKeyJ,			KEY_KEY_J,
		s3eKeyK,			KEY_KEY_K,
		s3eKeyL,			KEY_KEY_L,
		s3eKeyM,			KEY_KEY_M,
		s3eKeyN,			KEY_KEY_N,
		s3eKeyO,			KEY_KEY_O,
		s3eKeyP,			KEY_KEY_P,
		s3eKeyQ,			KEY_KEY_Q,
		s3eKeyR,			KEY_KEY_R,
		s3eKeyS,			KEY_KEY_S,
		s3eKeyT,			KEY_KEY_T,
		s3eKeyU,			KEY_KEY_U,
		s3eKeyV,			KEY_KEY_V,
		s3eKeyW,			KEY_KEY_W,
		s3eKeyX,			KEY_KEY_X,
		s3eKeyY,			KEY_KEY_Y,
		s3eKeyZ,			KEY_KEY_Z,

		s3eKeyF1,			KEY_F1,
		s3eKeyF2,			KEY_F2,
		s3eKeyF3,			KEY_F3,
		s3eKeyF4,			KEY_F4,
		s3eKeyF5,			KEY_F5,
		s3eKeyF6,			KEY_F6,
		s3eKeyF7,			KEY_F7,
		s3eKeyF8,			KEY_F8,
		s3eKeyF9,			KEY_F9,
		s3eKeyF10,			KEY_F10,
		s3eKeyF11,			KEY_F11,
		s3eKeyF12,			KEY_F12,

		s3eKeyNumPad0,		KEY_NUMPAD0,
		s3eKeyNumPad1,		KEY_NUMPAD1,
		s3eKeyNumPad2,		KEY_NUMPAD2,
		s3eKeyNumPad3,		KEY_NUMPAD3,
		s3eKeyNumPad4,		KEY_NUMPAD4,
		s3eKeyNumPad5,		KEY_NUMPAD5,
		s3eKeyNumPad6,		KEY_NUMPAD6,
		s3eKeyNumPad7,		KEY_NUMPAD7,
		s3eKeyNumPad8,		KEY_NUMPAD8,
		s3eKeyNumPad9,		KEY_NUMPAD9,
		s3eKeyNumPadPlus,	KEY_PLUS,
		s3eKeyNumPadMinus,	KEY_MINUS,
		s3eKeyNumPadEnter,	KEY_RETURN,

		s3eKeyRightControl,	KEY_CONTROL,
		s3eKeyRightShift,	KEY_SHIFT,


		s3eKeyComma,		KEY_COMMA,
		s3eKeyPeriod,		KEY_PERIOD,
		s3eKeyCapsLock,		KEY_CAPITAL,
		s3eKeyNumLock,		KEY_NUMLOCK,
		s3eKeyInsert,		KEY_INSERT,
		s3eKeyHome,			KEY_HOME,
		s3eKeyEnd,			KEY_END,
		s3eKeyDelete,		KEY_DELETE,
		s3eKeyPause,		KEY_PAUSE,
		s3eKeyBack,			KEY_BACK,
		s3eKeyMenu,			KEY_MENU,
	};

	static EKEY_CODE irr_key_from_s3e[s3eKeyCount];
	static s3eKey s3e_key_from_irr[KEY_KEY_CODES_COUNT];

	static void initKeyMap()
	{
		memset(&irr_key_from_s3e, -1, sizeof(irr_key_from_s3e));
		memset(&s3e_key_from_irr, -1, sizeof(s3e_key_from_irr));

		for(int i = 0; i < sizeof(key_map)/sizeof(key_map[0]); i += 2)
		{
			s3eKey s3e = (s3eKey)key_map[i];
			EKEY_CODE key = (EKEY_CODE)key_map[i+1];

			irr_key_from_s3e[s3e] = key;
			s3e_key_from_irr[key] = s3e;
		}
	}

	class CMarmaladeCursor: public gui::ICursorControl
	{
	public:
		CMarmaladeCursor(CIrrDeviceMarmalade *owner)
			: owner(owner), buttons(0)
		{
			mMTStatus.clear();

			if(!s3ePointerGetInt(S3E_POINTER_MULTI_TOUCH_AVAILABLE))
			{
				s3ePointerRegister(S3E_POINTER_MOTION_EVENT, do_motionEvent, this);
				s3ePointerRegister(S3E_POINTER_BUTTON_EVENT, do_buttonEvent, this);
			}
			else
			{
				s3ePointerRegister(S3E_POINTER_TOUCH_EVENT, do_touchEvent, this);
				s3ePointerRegister(S3E_POINTER_TOUCH_MOTION_EVENT, do_touchMotionEvent, this);
			}
		}

		~CMarmaladeCursor()
		{
			if(!s3ePointerGetInt(S3E_POINTER_MULTI_TOUCH_AVAILABLE))
			{
				s3ePointerUnRegister(S3E_POINTER_MOTION_EVENT, do_motionEvent);
				s3ePointerUnRegister(S3E_POINTER_BUTTON_EVENT, do_buttonEvent);
			}
			else
			{
				s3ePointerUnRegister(S3E_POINTER_TOUCH_EVENT, do_touchEvent);
				s3ePointerUnRegister(S3E_POINTER_TOUCH_MOTION_EVENT, do_touchMotionEvent);
			}
		}

		void setVisible(bool visible)
		{
			s3ePointerSetInt(S3E_POINTER_HIDE_CURSOR, visible ? 1 : 0);
		}

		bool isVisible() const
		{
			return s3ePointerGetInt(S3E_POINTER_TYPE) == S3E_POINTER_TYPE_MOUSE
				&& !s3ePointerGetInt(S3E_POINTER_HIDE_CURSOR);
		}

		void setPosition(const core::position2d<f32> &pos) {}
		void setPosition(f32 x, f32 y) {};
		void setPosition(const core::position2d<s32> &pos) {}
		void setPosition(s32 x, s32 y) {}

		const core::position2d<s32>& getPosition() { return mPosition; }
		core::position2d<f32> getRelativePosition()
		{
			return core::vector2d<f32>(mPosition.X/(f32)s3eSurfaceGetInt(S3E_SURFACE_WIDTH),
				mPosition.Y/(f32)s3eSurfaceGetInt(S3E_SURFACE_HEIGHT));

		}

		void setReferenceRect(core::rect<s32>* rect=0) {}

		void onMotionEvent(s3ePointerMotionEvent *evt)
		{
			mPosition.X = evt->m_x;
			mPosition.Y = evt->m_y;

			SEvent irrEvt;
			irrEvt.EventType = EET_MOUSE_INPUT_EVENT;
			irrEvt.MouseInput.X = mPosition.X;
			irrEvt.MouseInput.Y = mPosition.Y;
			irrEvt.MouseInput.Event = EMIE_MOUSE_MOVED;
			irrEvt.MouseInput.ButtonStates = buttons;
			irrEvt.MouseInput.Control = 0;
			irrEvt.MouseInput.Shift = 0;
			irrEvt.MouseInput.Wheel = 0;
			owner->postEventFromUser(irrEvt);
		}

		void onButtonEvent(s3ePointerEvent *evt)
		{
			mPosition.X = evt->m_x;
			mPosition.Y = evt->m_y;

			static const EMOUSE_INPUT_EVENT downEvts[] = {
				EMIE_LMOUSE_PRESSED_DOWN,
				EMIE_RMOUSE_PRESSED_DOWN,
				EMIE_MMOUSE_PRESSED_DOWN
			};

			static const EMOUSE_INPUT_EVENT upEvts[] = {
				EMIE_LMOUSE_LEFT_UP,
				EMIE_RMOUSE_LEFT_UP,
				EMIE_MMOUSE_LEFT_UP
			};

			IwAssert(IRRLICHT, sizeof(downEvts) == sizeof(upEvts));

			int btn = evt->m_Button;
			if(btn >= sizeof(downEvts)/sizeof(downEvts[0]))
				return;

			if(evt->m_Pressed)
				buttons |= (1 << btn);
			else
				buttons &=~ (1 << btn);

			SEvent irrEvt;
			irrEvt.EventType = EET_MOUSE_INPUT_EVENT;
			irrEvt.MouseInput.Event = evt->m_Pressed ?
				downEvts[btn] : upEvts[btn];
			irrEvt.MouseInput.ButtonStates = buttons;
			irrEvt.MouseInput.Control = 0;
			irrEvt.MouseInput.Shift = 0;
			irrEvt.MouseInput.Wheel = 0;
			irrEvt.MouseInput.X = mPosition.X;
			irrEvt.MouseInput.Y = mPosition.Y;
			owner->postEventFromUser(irrEvt);
		}

		void onTouchEvent(s3ePointerTouchEvent *evt)
		{
			if(evt->m_TouchID >= NUMBER_OF_MULTI_TOUCHES)
				return;

			int prevBtn = mMTStatus.touchedCount()-1;

			mMTStatus.Touched[evt->m_TouchID] = evt->m_Pressed;
			mMTStatus.PrevX[evt->m_TouchID] = mMTStatus.X[evt->m_TouchID];
			mMTStatus.PrevY[evt->m_TouchID] = mMTStatus.Y[evt->m_TouchID];
			mMTStatus.X[evt->m_TouchID] = evt->m_x;
			mMTStatus.Y[evt->m_TouchID] = evt->m_y;

			SEvent irrEvt;
			irrEvt.EventType = EET_MULTI_TOUCH_EVENT;
			irrEvt.MultitouchInput = mMTStatus;
			irrEvt.MultitouchInput.Event = evt->m_Pressed ?
				EMTIE_PRESSED_DOWN : EMTIE_LEFT_UP;
			owner->postEventFromUser(irrEvt);

			// Publish fake event

			s3ePointerEvent fakeEvt;

			for(int i = 0; i < NUMBER_OF_MULTI_TOUCHES; i++)
			{
				if(mMTStatus.Touched[i])
				{
					if(i > evt->m_TouchID)
					{
						mOffset.X += mMTStatus.X[evt->m_TouchID] - mMTStatus.X[i];
						mOffset.Y += mMTStatus.Y[evt->m_TouchID] - mMTStatus.Y[i];
					}

					mPosition.X = mMTStatus.X[i] + mOffset.X;
					mPosition.Y = mMTStatus.Y[i] + mOffset.Y;
					break;
				}
			}

			fakeEvt.m_x = mPosition.X;
			fakeEvt.m_y = mPosition.Y;

			if(prevBtn >= 0)
			{
				fakeEvt.m_Button = (s3ePointerButton)prevBtn;
				fakeEvt.m_Pressed = 0;
				onButtonEvent(&fakeEvt);
			}

			int newBtn = mMTStatus.touchedCount()-1;
			if(newBtn >= 0)
			{
				fakeEvt.m_Button = (s3ePointerButton)newBtn;
				fakeEvt.m_Pressed = 1;
				onButtonEvent(&fakeEvt);
			}
			else
				mOffset.set(0,0);
		}

		void onTouchMotionEvent(s3ePointerTouchMotionEvent *evt)
		{
			if(evt->m_TouchID > NUMBER_OF_MULTI_TOUCHES)
				return;

			mMTStatus.PrevX[evt->m_TouchID] = mMTStatus.X[evt->m_TouchID];
			mMTStatus.PrevY[evt->m_TouchID] = mMTStatus.Y[evt->m_TouchID];
			mMTStatus.X[evt->m_TouchID] = evt->m_x;
			mMTStatus.Y[evt->m_TouchID] = evt->m_y;

			SEvent irrEvt;
			irrEvt.EventType = EET_MULTI_TOUCH_EVENT;
			irrEvt.MultitouchInput = mMTStatus;
			irrEvt.MultitouchInput.Event = EMTIE_MOVED;
			owner->postEventFromUser(irrEvt);

			// Publish fake event
			s3ePointerMotionEvent fakeEvt;
			for(int i = 0; i < NUMBER_OF_MULTI_TOUCHES; i++)
			{
				if(mMTStatus.Touched[i])
				{
					mPosition.X = mMTStatus.X[i] + mOffset.X;
					mPosition.Y = mMTStatus.Y[i] + mOffset.Y;
					break;
				}
			}

			fakeEvt.m_x = mPosition.X;
			fakeEvt.m_y = mPosition.Y;
			onMotionEvent(&fakeEvt);
		}

		static int32 do_motionEvent(void *sys, void *me)
		{
			static_cast<CMarmaladeCursor*>(me)->onMotionEvent(
				static_cast<s3ePointerMotionEvent*>(sys));
			return 0;
		}

		static int32 do_buttonEvent(void *sys, void *me)
		{
			static_cast<CMarmaladeCursor*>(me)->onButtonEvent(
				static_cast<s3ePointerEvent*>(sys));
			return 0;
		}

		static int32 do_touchEvent(void *sys, void *me)
		{
			static_cast<CMarmaladeCursor*>(me)->onTouchEvent(
				static_cast<s3ePointerTouchEvent*>(sys));
			return 0;
		}

		static int32 do_touchMotionEvent(void *sys, void *me)
		{
			static_cast<CMarmaladeCursor*>(me)->onTouchMotionEvent(
				static_cast<s3ePointerTouchMotionEvent*>(sys));
			return 0;
		}

	private:
		CIrrDeviceMarmalade *owner;
		uint32_t buttons;
		core::position2d<s32> mPosition;
		core::vector2d<s32> mOffset;
		SEvent::SMultitouchInput mMTStatus;
	};

	//! constructor
	CIrrDeviceMarmalade::CIrrDeviceMarmalade(const SIrrlichtCreationParameters& param)
		: CIrrDeviceStub(param), Paused(false)
	{
		initKeyMap();

		CursorControl = new CMarmaladeCursor(this);

		//video::SExposedVideoData data;

		SIrrlichtCreationParameters params = param;
		params.WindowSize.Width = s3eSurfaceGetInt(S3E_SURFACE_WIDTH);
		params.WindowSize.Height = s3eSurfaceGetInt(S3E_SURFACE_HEIGHT);

		int32 glver = s3eGLGetInt(S3E_GL_VERSION);
		if(glver >= 0x200)
			VideoDriver = video::createOGLES2Driver(params, FileSystem);
		else
			VideoDriver = video::createOGLES1Driver(params, FileSystem);

        glViewport(0, 0, params.WindowSize.Width, params.WindowSize.Height);

		if (VideoDriver)
			createGUIAndScene();

		s3eDeviceRegister(S3E_DEVICE_EXIT, &do_Exit, this);
		s3eDeviceRegister(S3E_DEVICE_PAUSE, &do_Pause, this);
		s3eDeviceRegister(S3E_DEVICE_UNPAUSE, &do_Unpause, this);

		s3eKeyboardRegister(S3E_KEYBOARD_KEY_EVENT, &do_KeyEvent, this);
		s3eKeyboardRegister(S3E_KEYBOARD_CHAR_EVENT, &do_CharEvent, this);
	}


	//! destructor
	CIrrDeviceMarmalade::~CIrrDeviceMarmalade()
	{

		s3eDeviceUnRegister(S3E_DEVICE_UNPAUSE, &do_Unpause);
		s3eDeviceUnRegister(S3E_DEVICE_PAUSE, &do_Pause);
		s3eDeviceUnRegister(S3E_DEVICE_EXIT, &do_Exit);

		s3eKeyboardUnRegister(S3E_KEYBOARD_CHAR_EVENT, &do_CharEvent);
		s3eKeyboardUnRegister(S3E_KEYBOARD_KEY_EVENT, &do_CharEvent);
	}

	bool CIrrDeviceMarmalade::isWindowActive() const
	{
		return !Paused;
	}

	bool CIrrDeviceMarmalade::isWindowFocused() const
	{
		return !Paused;
	}

	bool CIrrDeviceMarmalade::isWindowMinimized() const
	{
		return Paused;
	}

	//! runs the device. Returns false if device wants to be deleted
	bool CIrrDeviceMarmalade::run()
	{
		s3eDeviceYield();
		s3ePointerUpdate();
		s3eKeyboardUpdate();
		os::Timer::tick();
		return !Close;
	}

    core::position2di CIrrDeviceMarmalade::getWindowPosition()
    {
        return core::position2di(0, 0);
    }

	void CIrrDeviceMarmalade::closeDevice()
	{
		Close = true;
	}

	//! pause execution temporarily
	void CIrrDeviceMarmalade::yield()
	{
		s3eDeviceYield();
	}

	//! pause execution for a specified time
	void CIrrDeviceMarmalade::sleep(u32 timeMs, bool pauseTimer)
	{
		const bool wasStopped = Timer ? Timer->isStopped() : true;
		if (pauseTimer && !wasStopped)
			Timer->stop();

		s3eDeviceYield(timeMs);

		if (pauseTimer && !wasStopped)
			Timer->start();
	}

	//! presents a surface in the client area
	bool CIrrDeviceMarmalade::present(video::IImage* surface, void* windowId, core::rect<s32>* srcClip)
	{
		return true;
	}

	void CIrrDeviceMarmalade::onKeyEvent(s3eKeyboardEvent *evt)
	{
		SEvent irrEvt;
		irrEvt.EventType = EET_KEY_INPUT_EVENT;
		irrEvt.KeyInput.Char = 0;
		irrEvt.KeyInput.Key = irr_key_from_s3e[evt->m_Key];
		irrEvt.KeyInput.PressedDown = !!evt->m_Pressed;
		irrEvt.KeyInput.Control = 0;
		irrEvt.KeyInput.Shift = 0;
		postEventFromUser(irrEvt);
	}

	void CIrrDeviceMarmalade::onCharEvent(s3eKeyboardCharEvent *evt)
	{
		SEvent irrEvt;
		irrEvt.EventType = EET_KEY_INPUT_EVENT;
		irrEvt.KeyInput.Char = evt->m_Char;
		irrEvt.KeyInput.Key = (EKEY_CODE)0;
		irrEvt.KeyInput.PressedDown = 1;
		irrEvt.KeyInput.Control = 0;
		irrEvt.KeyInput.Shift = 0;
		postEventFromUser(irrEvt);
		irrEvt.KeyInput.PressedDown = 0;
		postEventFromUser(irrEvt);
	}

	void CIrrDeviceMarmalade::onExit()
	{
		Close = true;
	}

	void CIrrDeviceMarmalade::onPause()
	{
		Paused = true;
	}

	void CIrrDeviceMarmalade::onUnpause()
	{
		Paused = false;
	}

	int32 CIrrDeviceMarmalade::do_KeyEvent(void *sys, void *usr)
	{
		CIrrDeviceMarmalade *self = static_cast<CIrrDeviceMarmalade*>(usr);
		self->onKeyEvent(static_cast<s3eKeyboardEvent*>(sys));
		return 0;
	}

	int32 CIrrDeviceMarmalade::do_CharEvent(void *sys, void *usr)
	{
		CIrrDeviceMarmalade *self = static_cast<CIrrDeviceMarmalade*>(usr);
		self->onCharEvent(static_cast<s3eKeyboardCharEvent*>(sys));
		return 0;
	}

	int32 CIrrDeviceMarmalade::do_Exit(void*, void *usr)
	{
		CIrrDeviceMarmalade *self = static_cast<CIrrDeviceMarmalade*>(usr);
		self->onExit();
		return 0;
	}

	int32 CIrrDeviceMarmalade::do_Pause(void*, void *usr)
	{
		CIrrDeviceMarmalade *self = static_cast<CIrrDeviceMarmalade*>(usr);
		self->onPause();
		return 0;
	}

	int32 CIrrDeviceMarmalade::do_Unpause(void*, void *usr)
	{
		CIrrDeviceMarmalade *self = static_cast<CIrrDeviceMarmalade*>(usr);
		self->onUnpause();
		return 0;
	}

} // end namespace irr

#endif // _IRR_MARMALADE_PLATFORM_
