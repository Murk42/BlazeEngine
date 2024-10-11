#include "pch.h"
#include "GlobalData.h"
#include "BlazeEngine/Console/Console.h"
#include "BlazeEngine/Window/Window.h"
#include "SDL2/SDL_events.h"

namespace Blaze
{
	ConsoleOutputStream consoleOutputStream;
	BlazeEngineContext blazeEngineContext;	

	BlazeEngineContext::BlazeEngineContext() : 
		scancodemap({
		{ Key::Unknown, SDL_Scancode::SDL_SCANCODE_UNKNOWN			},
		{ Key::A,	SDL_Scancode::SDL_SCANCODE_A					},
		{ Key::B,	SDL_Scancode::SDL_SCANCODE_B					},
		{ Key::C,	SDL_Scancode::SDL_SCANCODE_C					},
		{ Key::D,	SDL_Scancode::SDL_SCANCODE_D					},
		{ Key::E,	SDL_Scancode::SDL_SCANCODE_E					},
		{ Key::F,	SDL_Scancode::SDL_SCANCODE_F					},
		{ Key::G,	SDL_Scancode::SDL_SCANCODE_G					},
		{ Key::H,	SDL_Scancode::SDL_SCANCODE_H					},
		{ Key::I,	SDL_Scancode::SDL_SCANCODE_I					},
		{ Key::J,	SDL_Scancode::SDL_SCANCODE_J					},
		{ Key::K,	SDL_Scancode::SDL_SCANCODE_K					},
		{ Key::L,	SDL_Scancode::SDL_SCANCODE_L					},
		{ Key::M,	SDL_Scancode::SDL_SCANCODE_M					},
		{ Key::N,	SDL_Scancode::SDL_SCANCODE_N					},
		{ Key::O,	SDL_Scancode::SDL_SCANCODE_O					},
		{ Key::P,	SDL_Scancode::SDL_SCANCODE_P					},
		{ Key::Q,	SDL_Scancode::SDL_SCANCODE_Q					},
		{ Key::R,	SDL_Scancode::SDL_SCANCODE_R					},
		{ Key::S,	SDL_Scancode::SDL_SCANCODE_S					},
		{ Key::T,	SDL_Scancode::SDL_SCANCODE_T					},
		{ Key::U,	SDL_Scancode::SDL_SCANCODE_U					},
		{ Key::V,	SDL_Scancode::SDL_SCANCODE_V					},
		{ Key::W,	SDL_Scancode::SDL_SCANCODE_W					},
		{ Key::X,	SDL_Scancode::SDL_SCANCODE_X					},
		{ Key::Y,	SDL_Scancode::SDL_SCANCODE_Y					},
		{ Key::Z,	SDL_Scancode::SDL_SCANCODE_Z					},

		{ Key::One,SDL_Scancode::SDL_SCANCODE_1						},
		{ Key::Two,SDL_Scancode::SDL_SCANCODE_2						},
		{ Key::Three,	SDL_Scancode::SDL_SCANCODE_3					},
		{ Key::Four,SDL_Scancode::SDL_SCANCODE_4					},
		{ Key::Five,SDL_Scancode::SDL_SCANCODE_5					},
		{ Key::Six,SDL_Scancode::SDL_SCANCODE_6						},
		{ Key::Seven,	SDL_Scancode::SDL_SCANCODE_7					},
		{ Key::Eight,	SDL_Scancode::SDL_SCANCODE_8					},
		{ Key::Nine,SDL_Scancode::SDL_SCANCODE_9					},
		{ Key::Zero,SDL_Scancode::SDL_SCANCODE_0					},

		{ Key::Enter,	SDL_Scancode::SDL_SCANCODE_RETURN		    },
		{ Key::Escape,	SDL_Scancode::SDL_SCANCODE_ESCAPE		    },
		{ Key::Backspace,	SDL_Scancode::SDL_SCANCODE_BACKSPACE		    },
		{ Key::Tab,SDL_Scancode::SDL_SCANCODE_TAB			    	},
		{ Key::Space,	SDL_Scancode::SDL_SCANCODE_SPACE			    },
		{ Key::Minus,	SDL_Scancode::SDL_SCANCODE_MINUS			    },
		{ Key::Equals,	SDL_Scancode::SDL_SCANCODE_EQUALS		    },
		{ Key::LeftBracket,SDL_Scancode::SDL_SCANCODE_LEFTBRACKET	    	},
		{ Key::RightBracket,SDL_Scancode::SDL_SCANCODE_RIGHTBRACKET	    },
		{ Key::Backslash,	SDL_Scancode::SDL_SCANCODE_BACKSLASH		    },
		{ Key::Semicolon,	SDL_Scancode::SDL_SCANCODE_SEMICOLON		    },
		{ Key::Aapostrophe,SDL_Scancode::SDL_SCANCODE_APOSTROPHE	    	},
		{ Key::Grave,	SDL_Scancode::SDL_SCANCODE_GRAVE			    },
		{ Key::Comma,	SDL_Scancode::SDL_SCANCODE_COMMA			    },
		{ Key::Period,	SDL_Scancode::SDL_SCANCODE_PERIOD		    },
		{ Key::Slash,	SDL_Scancode::SDL_SCANCODE_SLASH			    },
		{ Key::CapsLock,SDL_Scancode::SDL_SCANCODE_CAPSLOCK		    },

		{ Key::F1,	SDL_Scancode::SDL_SCANCODE_F1				},
		{ Key::F2,	SDL_Scancode::SDL_SCANCODE_F2				},
		{ Key::F3,	SDL_Scancode::SDL_SCANCODE_F3				},
		{ Key::F4,	SDL_Scancode::SDL_SCANCODE_F4				},
		{ Key::F5,	SDL_Scancode::SDL_SCANCODE_F5				},
		{ Key::F6,	SDL_Scancode::SDL_SCANCODE_F6				},
		{ Key::F7,	SDL_Scancode::SDL_SCANCODE_F7				},
		{ Key::F8,	SDL_Scancode::SDL_SCANCODE_F8				},
		{ Key::F9,	SDL_Scancode::SDL_SCANCODE_F9				},
		{ Key::F10,SDL_Scancode::SDL_SCANCODE_F10					},
		{ Key::F11,SDL_Scancode::SDL_SCANCODE_F11					},
		{ Key::F12,SDL_Scancode::SDL_SCANCODE_F12					},

		{ Key::PrintScreen,SDL_Scancode::SDL_SCANCODE_PRINTSCREEN			},
		{ Key::ScrollLock,	SDL_Scancode::SDL_SCANCODE_SCROLLLOCK		},
		{ Key::Pause,	SDL_Scancode::SDL_SCANCODE_PAUSE				},
		{ Key::Insert,	SDL_Scancode::SDL_SCANCODE_INSERT			},
		{ Key::Home,SDL_Scancode::SDL_SCANCODE_HOME				},
		{ Key::PageUp,	SDL_Scancode::SDL_SCANCODE_PAGEUP			},
		{ Key::Delete,	SDL_Scancode::SDL_SCANCODE_DELETE			},
		{ Key::End,SDL_Scancode::SDL_SCANCODE_END					},
		{ Key::PageDown,SDL_Scancode::SDL_SCANCODE_PAGEDOWN			},
		{ Key::Right,	SDL_Scancode::SDL_SCANCODE_RIGHT				},
		{ Key::Left,SDL_Scancode::SDL_SCANCODE_LEFT				},
		{ Key::Down,SDL_Scancode::SDL_SCANCODE_DOWN				},
		{ Key::Up,	SDL_Scancode::SDL_SCANCODE_UP				},

		{ Key::KeypadDivide,SDL_Scancode::SDL_SCANCODE_KP_DIVIDE			},
		{ Key::KeypadMultiply,	SDL_Scancode::SDL_SCANCODE_KP_MULTIPLY		},
		{ Key::KeypadMinus,SDL_Scancode::SDL_SCANCODE_KP_MINUS				},
		{ Key::KeypadPlus,	SDL_Scancode::SDL_SCANCODE_KP_PLUS			},
		{ Key::KeypadEnter,SDL_Scancode::SDL_SCANCODE_KP_ENTER				},
		{ Key::Keypad1,SDL_Scancode::SDL_SCANCODE_KP_1					},
		{ Key::Keypad2,SDL_Scancode::SDL_SCANCODE_KP_2					},
		{ Key::Keypad3,SDL_Scancode::SDL_SCANCODE_KP_3					},
		{ Key::Keypad4,SDL_Scancode::SDL_SCANCODE_KP_4					},
		{ Key::Keypad5,SDL_Scancode::SDL_SCANCODE_KP_5					},
		{ Key::Keypad6,SDL_Scancode::SDL_SCANCODE_KP_6					},
		{ Key::Keypad7,SDL_Scancode::SDL_SCANCODE_KP_7					},
		{ Key::Keypad8,SDL_Scancode::SDL_SCANCODE_KP_8					},
		{ Key::Keypad9,SDL_Scancode::SDL_SCANCODE_KP_9					},
		{ Key::Keypad0,SDL_Scancode::SDL_SCANCODE_KP_0					},
		{ Key::KeypadPeriod	,SDL_Scancode::SDL_SCANCODE_KP_PERIOD			},

		{ Key::Application,SDL_Scancode::SDL_SCANCODE_APPLICATION			},
		{ Key::Power,	SDL_Scancode::SDL_SCANCODE_POWER				},
		{ Key::F13,SDL_Scancode::SDL_SCANCODE_F13					},
		{ Key::F14,SDL_Scancode::SDL_SCANCODE_F14					},
		{ Key::F15,SDL_Scancode::SDL_SCANCODE_F15					},
		{ Key::F16,SDL_Scancode::SDL_SCANCODE_F16					},
		{ Key::F17,SDL_Scancode::SDL_SCANCODE_F17					},
		{ Key::F18,SDL_Scancode::SDL_SCANCODE_F18					},
		{ Key::F19,SDL_Scancode::SDL_SCANCODE_F19					},
		{ Key::F20,SDL_Scancode::SDL_SCANCODE_F20					},
		{ Key::F21,SDL_Scancode::SDL_SCANCODE_F21					},
		{ Key::F22,SDL_Scancode::SDL_SCANCODE_F22					},
		{ Key::F23,SDL_Scancode::SDL_SCANCODE_F23					},
		{ Key::F24,SDL_Scancode::SDL_SCANCODE_F24					},
		{ Key::Execute,SDL_Scancode::SDL_SCANCODE_EXECUTE				},
		{ Key::Help,SDL_Scancode::SDL_SCANCODE_HELP				},
		{ Key::Menu,SDL_Scancode::SDL_SCANCODE_MENU				},
		{ Key::Select,	SDL_Scancode::SDL_SCANCODE_SELECT			},
		{ Key::Stop,SDL_Scancode::SDL_SCANCODE_STOP				},
		{ Key::Again,	SDL_Scancode::SDL_SCANCODE_AGAIN				},
		{ Key::Undo,SDL_Scancode::SDL_SCANCODE_UNDO				},
		{ Key::Cut,SDL_Scancode::SDL_SCANCODE_CUT					},
		{ Key::Copy,SDL_Scancode::SDL_SCANCODE_COPY				},
		{ Key::Paste,	SDL_Scancode::SDL_SCANCODE_PASTE				},
		{ Key::Mute,SDL_Scancode::SDL_SCANCODE_MUTE				},
		{ Key::VolumeUp,SDL_Scancode::SDL_SCANCODE_VOLUMEUP			},
		{ Key::VolumeDown,	SDL_Scancode::SDL_SCANCODE_VOLUMEDOWN		},

		{ Key::LCtrl,	SDL_Scancode::SDL_SCANCODE_LCTRL				},
		{ Key::LShift,	SDL_Scancode::SDL_SCANCODE_LSHIFT			},
		{ Key::LAlt,SDL_Scancode::SDL_SCANCODE_LALT				},
		{ Key::RCtrl,	SDL_Scancode::SDL_SCANCODE_RCTRL				},
		{ Key::RShift,	SDL_Scancode::SDL_SCANCODE_RSHIFT			},
		{ Key::RAlt,SDL_Scancode::SDL_SCANCODE_RALT				}
			}),
		keymap({
					{ SDL_Scancode::SDL_SCANCODE_UNKNOWN,			Key::Unknown			},
					{ SDL_Scancode::SDL_SCANCODE_A,					Key::A				},
					{ SDL_Scancode::SDL_SCANCODE_B,					Key::B				},
					{ SDL_Scancode::SDL_SCANCODE_C,					Key::C				},
					{ SDL_Scancode::SDL_SCANCODE_D,					Key::D				},
					{ SDL_Scancode::SDL_SCANCODE_E,					Key::E				},
					{ SDL_Scancode::SDL_SCANCODE_F,					Key::F				},
					{ SDL_Scancode::SDL_SCANCODE_G,					Key::G				},
					{ SDL_Scancode::SDL_SCANCODE_H,					Key::H				},
					{ SDL_Scancode::SDL_SCANCODE_I,					Key::I				},
					{ SDL_Scancode::SDL_SCANCODE_J,					Key::J				},
					{ SDL_Scancode::SDL_SCANCODE_K,					Key::K				},
					{ SDL_Scancode::SDL_SCANCODE_L,					Key::L				},
					{ SDL_Scancode::SDL_SCANCODE_M,					Key::M				},
					{ SDL_Scancode::SDL_SCANCODE_N,					Key::N				},
					{ SDL_Scancode::SDL_SCANCODE_O,					Key::O				},
					{ SDL_Scancode::SDL_SCANCODE_P,					Key::P				},
					{ SDL_Scancode::SDL_SCANCODE_Q,					Key::Q				},
					{ SDL_Scancode::SDL_SCANCODE_R,					Key::R				},
					{ SDL_Scancode::SDL_SCANCODE_S,					Key::S				},
					{ SDL_Scancode::SDL_SCANCODE_T,					Key::T				},
					{ SDL_Scancode::SDL_SCANCODE_U,					Key::U				},
					{ SDL_Scancode::SDL_SCANCODE_V,					Key::V				},
					{ SDL_Scancode::SDL_SCANCODE_W,					Key::W				},
					{ SDL_Scancode::SDL_SCANCODE_X,					Key::X				},
					{ SDL_Scancode::SDL_SCANCODE_Y,					Key::Y				},
					{ SDL_Scancode::SDL_SCANCODE_Z,					Key::Z				},

					{ SDL_Scancode::SDL_SCANCODE_1,					Key::One				},
					{ SDL_Scancode::SDL_SCANCODE_2,					Key::Two				},
					{ SDL_Scancode::SDL_SCANCODE_3,					Key::Three			},
					{ SDL_Scancode::SDL_SCANCODE_4,					Key::Four			},
					{ SDL_Scancode::SDL_SCANCODE_5,					Key::Five			},
					{ SDL_Scancode::SDL_SCANCODE_6,					Key::Six				},
					{ SDL_Scancode::SDL_SCANCODE_7,					Key::Seven			},
					{ SDL_Scancode::SDL_SCANCODE_8,					Key::Eight			},
					{ SDL_Scancode::SDL_SCANCODE_9,					Key::Nine			},
					{ SDL_Scancode::SDL_SCANCODE_0,					Key::Zero			},

					{ SDL_Scancode::SDL_SCANCODE_RETURN,		    Key::Enter			},
					{ SDL_Scancode::SDL_SCANCODE_ESCAPE,		    Key::Escape			},
					{ SDL_Scancode::SDL_SCANCODE_BACKSPACE,		    Key::Backspace		},
					{ SDL_Scancode::SDL_SCANCODE_TAB,			    Key::Tab				},
					{ SDL_Scancode::SDL_SCANCODE_SPACE,			    Key::Space			},
					{ SDL_Scancode::SDL_SCANCODE_MINUS,			    Key::Minus			},
					{ SDL_Scancode::SDL_SCANCODE_EQUALS,		    Key::Equals			},
					{ SDL_Scancode::SDL_SCANCODE_LEFTBRACKET,	    Key::LeftBracket		},
					{ SDL_Scancode::SDL_SCANCODE_RIGHTBRACKET,	    Key::RightBracket	},
					{ SDL_Scancode::SDL_SCANCODE_BACKSLASH,		    Key::Backslash		},
					{ SDL_Scancode::SDL_SCANCODE_SEMICOLON,		    Key::Semicolon		},
					{ SDL_Scancode::SDL_SCANCODE_APOSTROPHE,	    Key::Aapostrophe		},
					{ SDL_Scancode::SDL_SCANCODE_GRAVE,			    Key::Grave			},
					{ SDL_Scancode::SDL_SCANCODE_COMMA,			    Key::Comma			},
					{ SDL_Scancode::SDL_SCANCODE_PERIOD,		    Key::Period			},
					{ SDL_Scancode::SDL_SCANCODE_SLASH,			    Key::Slash			},
					{ SDL_Scancode::SDL_SCANCODE_CAPSLOCK,		    Key::CapsLock		},

					{ SDL_Scancode::SDL_SCANCODE_F1,				Key::F1				},
					{ SDL_Scancode::SDL_SCANCODE_F2,				Key::F2				},
					{ SDL_Scancode::SDL_SCANCODE_F3,				Key::F3				},
					{ SDL_Scancode::SDL_SCANCODE_F4,				Key::F4				},
					{ SDL_Scancode::SDL_SCANCODE_F5,				Key::F5				},
					{ SDL_Scancode::SDL_SCANCODE_F6,				Key::F6				},
					{ SDL_Scancode::SDL_SCANCODE_F7,				Key::F7				},
					{ SDL_Scancode::SDL_SCANCODE_F8,				Key::F8				},
					{ SDL_Scancode::SDL_SCANCODE_F9,				Key::F9				},
					{ SDL_Scancode::SDL_SCANCODE_F10,				Key::F10				},
					{ SDL_Scancode::SDL_SCANCODE_F11,				Key::F11				},
					{ SDL_Scancode::SDL_SCANCODE_F12,				Key::F12				},

					{ SDL_Scancode::SDL_SCANCODE_PRINTSCREEN,		Key::PrintScreen		},
					{ SDL_Scancode::SDL_SCANCODE_SCROLLLOCK,		Key::ScrollLock		},
					{ SDL_Scancode::SDL_SCANCODE_PAUSE,				Key::Pause			},
					{ SDL_Scancode::SDL_SCANCODE_INSERT,			Key::Insert			},
					{ SDL_Scancode::SDL_SCANCODE_HOME,				Key::Home			},
					{ SDL_Scancode::SDL_SCANCODE_PAGEUP,			Key::PageUp			},
					{ SDL_Scancode::SDL_SCANCODE_DELETE,			Key::Delete			},
					{ SDL_Scancode::SDL_SCANCODE_END,				Key::End				},
					{ SDL_Scancode::SDL_SCANCODE_PAGEDOWN,			Key::PageDown		},
					{ SDL_Scancode::SDL_SCANCODE_RIGHT,				Key::Right			},
					{ SDL_Scancode::SDL_SCANCODE_LEFT,				Key::Left			},
					{ SDL_Scancode::SDL_SCANCODE_DOWN,				Key::Down			},
					{ SDL_Scancode::SDL_SCANCODE_UP,				Key::Up				},

					{ SDL_Scancode::SDL_SCANCODE_KP_DIVIDE,			Key::KeypadDivide	},
					{ SDL_Scancode::SDL_SCANCODE_KP_MULTIPLY,		Key::KeypadMultiply	},
					{ SDL_Scancode::SDL_SCANCODE_KP_MINUS,			Key::KeypadMinus		},
					{ SDL_Scancode::SDL_SCANCODE_KP_PLUS,			Key::KeypadPlus		},
					{ SDL_Scancode::SDL_SCANCODE_KP_ENTER,			Key::KeypadEnter		},
					{ SDL_Scancode::SDL_SCANCODE_KP_1,				Key::Keypad1			},
					{ SDL_Scancode::SDL_SCANCODE_KP_2,				Key::Keypad2			},
					{ SDL_Scancode::SDL_SCANCODE_KP_3,				Key::Keypad3			},
					{ SDL_Scancode::SDL_SCANCODE_KP_4,				Key::Keypad4			},
					{ SDL_Scancode::SDL_SCANCODE_KP_5,				Key::Keypad5			},
					{ SDL_Scancode::SDL_SCANCODE_KP_6,				Key::Keypad6			},
					{ SDL_Scancode::SDL_SCANCODE_KP_7,				Key::Keypad7			},
					{ SDL_Scancode::SDL_SCANCODE_KP_8,				Key::Keypad8			},
					{ SDL_Scancode::SDL_SCANCODE_KP_9,				Key::Keypad9			},
					{ SDL_Scancode::SDL_SCANCODE_KP_0,				Key::Keypad0			},
					{ SDL_Scancode::SDL_SCANCODE_KP_PERIOD,			Key::KeypadPeriod	},

					{ SDL_Scancode::SDL_SCANCODE_APPLICATION,		Key::Application		},
					{ SDL_Scancode::SDL_SCANCODE_POWER,				Key::Power			},
					{ SDL_Scancode::SDL_SCANCODE_F13,				Key::F13				},
					{ SDL_Scancode::SDL_SCANCODE_F14,				Key::F14				},
					{ SDL_Scancode::SDL_SCANCODE_F15,				Key::F15				},
					{ SDL_Scancode::SDL_SCANCODE_F16,				Key::F16				},
					{ SDL_Scancode::SDL_SCANCODE_F17,				Key::F17				},
					{ SDL_Scancode::SDL_SCANCODE_F18,				Key::F18				},
					{ SDL_Scancode::SDL_SCANCODE_F19,				Key::F19				},
					{ SDL_Scancode::SDL_SCANCODE_F20,				Key::F20				},
					{ SDL_Scancode::SDL_SCANCODE_F21,				Key::F21				},
					{ SDL_Scancode::SDL_SCANCODE_F22,				Key::F22				},
					{ SDL_Scancode::SDL_SCANCODE_F23,				Key::F23				},
					{ SDL_Scancode::SDL_SCANCODE_F24,				Key::F24				},
					{ SDL_Scancode::SDL_SCANCODE_EXECUTE,			Key::Execute			},
					{ SDL_Scancode::SDL_SCANCODE_HELP,				Key::Help			},
					{ SDL_Scancode::SDL_SCANCODE_MENU,				Key::Menu			},
					{ SDL_Scancode::SDL_SCANCODE_SELECT,			Key::Select			},
					{ SDL_Scancode::SDL_SCANCODE_STOP,				Key::Stop			},
					{ SDL_Scancode::SDL_SCANCODE_AGAIN,				Key::Again			},
					{ SDL_Scancode::SDL_SCANCODE_UNDO,				Key::Undo			},
					{ SDL_Scancode::SDL_SCANCODE_CUT,				Key::Cut				},
					{ SDL_Scancode::SDL_SCANCODE_COPY,				Key::Copy			},
					{ SDL_Scancode::SDL_SCANCODE_PASTE,				Key::Paste			},
					{ SDL_Scancode::SDL_SCANCODE_MUTE,				Key::Mute			},
					{ SDL_Scancode::SDL_SCANCODE_VOLUMEUP,			Key::VolumeUp		},
					{ SDL_Scancode::SDL_SCANCODE_VOLUMEDOWN,		Key::VolumeDown		},

					{ SDL_Scancode::SDL_SCANCODE_LCTRL,				Key::LCtrl			},
					{ SDL_Scancode::SDL_SCANCODE_LSHIFT,			Key::LShift			},
					{ SDL_Scancode::SDL_SCANCODE_LALT,				Key::LAlt			},
					{ SDL_Scancode::SDL_SCANCODE_RCTRL,				Key::RCtrl			},
					{ SDL_Scancode::SDL_SCANCODE_RSHIFT,			Key::RShift			},
					{ SDL_Scancode::SDL_SCANCODE_RALT,				Key::RAlt			}
		}),
		mainThreadID(std::this_thread::get_id())
	{

	}

	BlazeEngineContext::~BlazeEngineContext()
	{
	}

	void BlazeEngineContext::ExecuteTaskOnMainThread(std::function<void()>&& function)
	{		
		//std::lock_guard<std::mutex> lk{ mainThreadTaskMutex };
		mainThreadTask = std::move(function);

		mainThreadTaskFlag.test_and_set();

		SDL_Event event;
		SDL_memset(&event, 0, sizeof(event));
		event.type = mainThreadTaskEventIdentifier;		
		SDL_PushEvent(&event);		
		
		mainThreadTaskFlag.wait(1);	
	}
	void BlazeEngineContext::ExecuteMainThreadTask()
	{
		if (mainThreadTaskFlag.test())
		{
			if (mainThreadTask)
				mainThreadTask();

			mainThreadTaskFlag.clear();
			mainThreadTaskFlag.notify_one();
		}
	}
	void BlazeEngineContext::HandleWindowKeyEvent(const Input::Events::KeyPressed& event)
	{
		eventStack.Add("KeyPressed", [](const Input::Events::KeyPressed& event) {
			event.window->HandleKeyEvent(event);			
			}, event);		
	}
	void BlazeEngineContext::HandleWindowKeyEvent(const Input::Events::KeyReleased& event)
	{
		eventStack.Add("KeyReleased", [](const Input::Events::KeyReleased& event) {
			event.window->HandleKeyEvent(event);
			}, event);
	}
	void BlazeEngineContext::HandleWindowResizeEvent(const Input::Events::WindowResizedEvent& event)
	{
		event.window->HandleResizeEvent(event);
	}
}