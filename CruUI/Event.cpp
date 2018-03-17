#include "Event.h"

namespace cru {
    BasicEventArgs::BasicEventArgs(Object * sender) {
        sender_ = sender;
    }

    BasicEventArgs::~BasicEventArgs() {
    }

    Object* BasicEventArgs::GetSender() {
        return sender_;
    }
}
