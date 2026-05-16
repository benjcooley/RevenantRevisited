// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *                uistyle.cpp - UI style stack implementation             *
// *************************************************************************
//
// See uistyle.h.
//
// *************************************************************************

#include "uistyle.h"

#include <vector>

namespace {

// Initialized lazily on first access so a default-constructed base style
// is always present at index 0. The function-local static avoids
// static-init-order pitfalls if a TPane subclass ever reads UI::Style()
// during a global construction (none do today, but cheap insurance).
std::vector<UIStyle>& StackImpl()
{
    static std::vector<UIStyle> stack{ UIStyle{} };
    return stack;
}

}  // namespace

namespace UI {

UIStyle& Style()
{
    return StackImpl().back();
}

void PushStyle(const UIStyle& s)
{
    StackImpl().push_back(s);
}

void PopStyle()
{
    auto& stack = StackImpl();
    if (stack.size() > 1)
        stack.pop_back();
}

size_t StyleDepth()
{
    return StackImpl().size();
}

}  // namespace UI
