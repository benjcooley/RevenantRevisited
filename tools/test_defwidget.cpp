// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   test_defwidget.cpp - which DEF widgets take a click (src/defpane.h)  *
// *************************************************************************

#include "../src/defpane.h"

#include <gtest/gtest.h>

namespace {

SDefWidget Widget(EDefWidget type, int32_t x, int32_t y, int32_t w, int32_t h)
{
    SDefWidget widget;
    widget.type = type;
    widget.x = x;
    widget.y = y;
    widget.w = w;
    widget.h = h;
    return widget;
}

TEST(DefWidget, DecorationTakesNoClick)
{
    // The load dialog's backdrop FRAME, listed first, covers its list.
    for (const EDefWidget type : {EDefWidget::Frame, EDefWidget::Text, EDefWidget::Bitmap})
        EXPECT_FALSE(Widget(type, 35, 70, 562, 320).TakesClick(330, 100));
    for (const EDefWidget type : {EDefWidget::Button, EDefWidget::Scrollbar, EDefWidget::Listbox,
                                  EDefWidget::Edit, EDefWidget::Droplist})
        EXPECT_TRUE(Widget(type, 284, 87, 298, 287).TakesClick(330, 100));
}

TEST(DefWidget, OnlyInItsRect)
{
    const SDefWidget list = Widget(EDefWidget::Listbox, 284, 87, 298, 287);
    EXPECT_TRUE(list.TakesClick(284, 87));
    EXPECT_FALSE(list.TakesClick(283, 87));
    EXPECT_FALSE(list.TakesClick(284 + 298, 100));
    EXPECT_FALSE(list.TakesClick(300, 87 + 287));
}

TEST(DefWidget, HiddenOrDisabledTakesNoClick)
{
    SDefWidget button = Widget(EDefWidget::Button, 158, 406, 140, 30);
    button.hidden = true;
    EXPECT_FALSE(button.TakesClick(200, 420));
    button.hidden = false;
    button.disabled = true;
    EXPECT_FALSE(button.TakesClick(200, 420));
}

}  // namespace
