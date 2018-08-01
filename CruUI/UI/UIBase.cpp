#include "UIBase.h"

namespace cru {
    namespace ui {
        const Point Point::zero(0, 0);
        const Size Size::zero(0, 0);

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
