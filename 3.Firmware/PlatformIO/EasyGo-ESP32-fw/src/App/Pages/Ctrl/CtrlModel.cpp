#include "CtrlModel.h"

using namespace Page;

uint32_t CtrlModel::GetData()
{
    return lv_tick_get();
}
