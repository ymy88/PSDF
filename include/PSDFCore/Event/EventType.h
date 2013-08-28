#pragma once

#include "../Common/Common.h"


enum EVENT_PRIORITY_LEVEL
{
	PRIORITY_LOW = 0,
	PRIORITY_NORMAL,
	PRIORITY_HIGH,
	PRIORITY_LEVEL_COUNT
};

// events specified by module
const string		FROM_SYSTEM					= "System";
const string		TO_ALL_MODULES				= "All modules";

/* 
    Each event type (32 bits) consists of two parts:
    1. Event group type (28 bits)
    2. Event type within the group (4 bits)

    Each event group type contains one and only
    contains one "1" in all bits. Each event type
    within group counts from 0x1 to 0xf.

	At most 28 event group types are supported
 */


const unsigned int	EVENT_STATION_ID_NON_VIEW	= 0x00000001;
const unsigned int	EVENT_STATION_ID_ALL_STA	= 0xFFFFFFFF;

const unsigned int	EVENT_GROUP_BASE				= 0x00000008;
const unsigned int	EVENT_GROUP_ALL					= 0xFFFFFFF8;
const short			EVENT_GROUP_COUNT				= 29;

/* get the event cluster type from a event type */
inline unsigned int getEventGroup( unsigned int eventType )
{
	return ( eventType & EVENT_GROUP_ALL );
}

inline int createParam( short high, short low )
{
	int h = (int)high;
	int l = (int)low;
	return ( ( h << 16 ) | ( l & 0x0000FFFF ) );
}

inline void getFromParam( int param, short &high, short &low )
{
	low = (short)param;
	high = (param >> 16);
}

#ifndef QNAMESPACE_H
enum KeyboardModifier {
	NoModifier           = 0x00000000,
	ShiftModifier        = 0x02000000,
	ControlModifier      = 0x04000000,
	AltModifier          = 0x08000000,
	MetaModifier         = 0x10000000,
	KeypadModifier       = 0x20000000,
	GroupSwitchModifier  = 0x40000000,
	KeyboardModifierMask = 0xfe000000
};
#endif

//
// System event type
//                                                                          | param1_h | param1_l | param2_h  |  param2_l |
const unsigned int		APP_EVENT_GROUP       = EVENT_GROUP_BASE << 0;
const unsigned int		  APP_QUIT              = APP_EVENT_GROUP | 0; 
const unsigned int		  FOCUS_IN              = APP_EVENT_GROUP | 1; 
const unsigned int		  FOCUS_OUT             = APP_EVENT_GROUP | 2; 

const unsigned int		MOUSE_CLICK_GROUP     = EVENT_GROUP_BASE << 1;
const unsigned int		  LBUTTON_DOWN          = MOUSE_CLICK_GROUP | 0;  // | x        | y        | Qt::KeyboardModifiers |
const unsigned int		  LBUTTON_UP            = MOUSE_CLICK_GROUP | 1;  // | x        | y        | Qt::KeyboardModifiers |
const unsigned int		  RBUTTON_DOWN          = MOUSE_CLICK_GROUP | 2;  // | x        | y        | Qt::KeyboardModifiers |
const unsigned int		  RBUTTON_UP            = MOUSE_CLICK_GROUP | 3;  // | x        | y        | Qt::KeyboardModifiers |
const unsigned int		  MBUTTON_DOWN          = MOUSE_CLICK_GROUP | 4;  // | x        | y        | Qt::KeyboardModifiers |
const unsigned int		  MBUTTON_UP            = MOUSE_CLICK_GROUP | 5;  // | x        | y        | Qt::KeyboardModifiers |
const unsigned int		  LBUTTON_DOUBLECLICK   = MOUSE_CLICK_GROUP | 6;  // | x        | y        | Qt::KeyboardModifiers |
const unsigned int		  RBUTTON_DOUBLECLICK   = MOUSE_CLICK_GROUP | 7;  // | x        | y        | Qt::KeyboardModifiers |

const unsigned int		MOUSE_MOTION_GROUP    = EVENT_GROUP_BASE << 2;
const unsigned int		  MOUSE_MOVE            = MOUSE_MOTION_GROUP | 0; // | x        | y        | Qt::KeyboardModifiers |

const unsigned int		MOUSE_SCROLL_GROUP    = EVENT_GROUP_BASE << 3;
const unsigned int		  SCROLL_UP             = MOUSE_SCROLL_GROUP | 0; // | delta               | Qt::KeyboardModifiers |
const unsigned int		  SCROLL_DOWN           = MOUSE_SCROLL_GROUP | 1; // | delta               | Qt::KeyboardModifiers |

const unsigned int		KEYBOARD_GROUP        = EVENT_GROUP_BASE << 4;
const unsigned int		  KEY_DOWN              = KEYBOARD_GROUP | 0;     // | key                 | Qt::KeyboardModifiers |
const unsigned int		  KEY_UP                = KEYBOARD_GROUP | 1;     // | key                 | Qt::KeyboardModifiers |

const unsigned int		SCENE_GROUP           = EVENT_GROUP_BASE << 5;
const unsigned int		  FRAME_BEGIN			= SCENE_GROUP | 0;
const unsigned int		  LAYER_CHANGED			= SCENE_GROUP | 1;
const unsigned int		  CAMERA_CHANGED		= SCENE_GROUP | 2;        // | viewport            | position | scale      |

const unsigned int		CUSTOM_GROUP          = EVENT_GROUP_BASE << 6; 

const unsigned int		SYSTEM_EVENT_GROUP	= APP_EVENT_GROUP | MOUSE_CLICK_GROUP | 
											  MOUSE_MOTION_GROUP | MOUSE_SCROLL_GROUP |
											  KEYBOARD_GROUP | SCENE_GROUP;

inline unsigned int PSDF_CORE_DLL_DECL generateCustomEventGroup()
{
	static unsigned int customGroupBase = CUSTOM_GROUP;
	assert(customGroupBase != 0x80000000);
	customGroupBase <<= 1;
	return customGroupBase;
}





/* 
    **   Note   **
  Custom event type should obey the above principle.
  The following format is recommended
  
  header file:
  extern const unsigned int MY_GROUP;
  extern const unsigned int     MY_EVENT1;
  extern const unsigned int     MY_EVENT2;

  cpp file:
  const unsigned int MY_GROUP = generateCustomGroup();
  const unsigned int     MY_EVENT1 = MY_GROUP | 0;
  const unsigned int     MY_EVENT2 = MY_GROUP | 1;
 */