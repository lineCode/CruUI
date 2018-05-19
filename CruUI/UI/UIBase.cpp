#include "UIBase.h"

namespace cru {
    namespace ui {
		UIEventArgs::UIEventArgs(Object* sender, Object* original_sender)
			: BasicEventArgs(sender), original_sender_(original_sender)
		{

		}

		UIEventArgs::~UIEventArgs()
		{

		}

		Object* UIEventArgs::GetOriginalSender()
		{
			return original_sender_;
		}
	}
}
