#include "UIBase.h"

namespace cru {
    namespace ui {
		UiEventArgs::UiEventArgs(Object* sender, Object* original_sender)
			: BasicEventArgs(sender), original_sender_(original_sender)
		{

		}

		Object* UiEventArgs::GetOriginalSender()
		{
			return original_sender_;
		}
	}
}
