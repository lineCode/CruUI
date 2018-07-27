#pragma once

#include "UI/UIBase.h"
#include "UI/Control.h"

namespace cru
{
    namespace ui
    {
        namespace controls
        {
            class TextBlock : public Control
            {
            public:
                TextBlock();
                ~TextBlock() override;

                CRU_NO_COPY_MOVE(TextBlock)

            public:

                std::wstring GetText();

                void SetText(const std::wstring& text);



            private:
                void OnTextChangedInternal(const std::wstring& old_text, const std::wstring& new_text);

            private:
                std::wstring text_;
            };
        }
    }
}
