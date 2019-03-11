#include "TextField.h"

template <typename Color, typename Range, Color white>
int TextFieldsDefaultCickListener (ActionData arg)
{
    GTextField<Color, Range, white> *textField = (GTextField<Color, Range, white> *)arg.object;
    textField->setSelectionStart(arg.touchPoint.x);
    textField->setSelectionEnd(arg.touchPoint.x);
    return 0;
}

template <typename Color, typename Range, Color white>
int TextFieldsDefaultTouchListener (ActionData arg)
{
    GTextField<Color, Range, white> *textField = (GTextField<Color, Range, white> *)arg.object;
    textField->setSelectionEnd(arg.touchPoint.x);
    textField->setSelect();
    return 0;
}

template <typename Color, typename Range, Color white>
int TextFieldsDefaultReleaseListener (ActionData arg)
{
    GTextField<Color, Range, white> *textField = (GTextField<Color, Range, white> *)arg.object;
    textField->setSelectionStart(0);
    textField->setSelectionEnd(0);
    return 0;
}