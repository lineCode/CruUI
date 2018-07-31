#include "Event.h"

namespace cru {
    BasicEventArgs::BasicEventArgs(Object * sender) {
        sender_ = sender;
    }

    Object* BasicEventArgs::GetSender() {
        return sender_;
    }
}
