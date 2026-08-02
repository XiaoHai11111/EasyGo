#include "NavigationView.h"
#include "NavigationModel.h"

using namespace Page;

void NavigationView::Create(lv_obj_t* root, const char* mapPath, int distanceMeters, int minutes)
{
    EasyGoUi::Screen(root, lv_color_white());
    EasyGoUi::StatusBar(root);

    ui.back = EasyGoUi::Panel(root, 5, 34, 28, 28, lv_color_hex(0xEEF2F7), 8);
    EasyGoUi::Pressable(ui.back);
    EasyGoUi::Symbol(ui.back, LV_SYMBOL_LEFT, EasyGoUi::Ink, 5, 4, &lv_font_montserrat_20);

    EasyGoUi::Panel(root, 57, 48, 9, 34, EasyGoUi::Green, 4);
    EasyGoUi::Symbol(root, LV_SYMBOL_UP, EasyGoUi::Green, 45, 32, &lv_font_montserrat_36);
    EasyGoUi::Label(root, "直行", &font_easygo_20, EasyGoUi::Green, 88, 30);
    ui.topDistance = EasyGoUi::Label(root, "--", &lv_font_montserrat_32, EasyGoUi::Ink, 88, 50);
    EasyGoUi::Label(root, "米", &font_easygo_20, EasyGoUi::Ink, 148, 55);
    lv_obj_t* toilet = EasyGoUi::Panel(root, 83, 82, 20, 20, EasyGoUi::Blue, 5);
    EasyGoUi::Label(toilet, "WC", &lv_font_montserrat_10, lv_color_white(), 1, 6, 20);
    ui.target = EasyGoUi::Label(root, "正在准备导航", &font_easygo_14, EasyGoUi::Ink, 108, 82);

    lv_obj_t* map = EasyGoUi::Panel(
        root,
        0,
        105,
        NavigationModel::MapViewportWidth,
        NavigationModel::MapViewportHeight,
        lv_color_hex(0xEEF1ED),
        0
    );
    ui.mapViewport = map;
    lv_obj_set_style_clip_corner(map, true, 0);
    lv_obj_add_flag(map, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_MOMENTUM);
    lv_obj_clear_flag(
        map,
        LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_CHAIN | LV_OBJ_FLAG_GESTURE_BUBBLE
    );
    lv_obj_set_scroll_dir(map, LV_DIR_ALL);
    lv_obj_set_scrollbar_mode(map, LV_SCROLLBAR_MODE_OFF);

    ui.mapContent = EasyGoUi::Panel(
        map,
        0,
        0,
        NavigationModel::MapCanvasWidth,
        NavigationModel::MapCanvasHeight,
        lv_color_hex(0xEEF1ED),
        0
    );
    lv_obj_clear_flag(ui.mapContent, LV_OBJ_FLAG_CLICKABLE);

    ui.mapImage = lv_img_create(ui.mapContent);
    lv_img_set_src(ui.mapImage, mapPath);
    lv_obj_set_pos(ui.mapImage, 0, 0);
    lv_obj_clear_flag(ui.mapImage, LV_OBJ_FLAG_CLICKABLE);

    routePoints[0] = { NavigationModel::MapInitialScrollX + 79, NavigationModel::MapInitialScrollY + 147 };
    routePoints[1] = { NavigationModel::MapInitialScrollX + 91, NavigationModel::MapInitialScrollY + 119 };
    routePoints[2] = { NavigationModel::MapInitialScrollX + 91, NavigationModel::MapInitialScrollY + 74 };
    routePoints[3] = { NavigationModel::MapInitialScrollX + 139, NavigationModel::MapInitialScrollY + 74 };
    routePoints[4] = { NavigationModel::MapInitialScrollX + 174, NavigationModel::MapInitialScrollY + 51 };
    lv_obj_t* route = lv_line_create(ui.mapContent);
    lv_line_set_points(route, routePoints, 5);
    lv_obj_set_style_line_color(route, EasyGoUi::Blue, 0);
    lv_obj_set_style_line_width(route, 5, 0);
    lv_obj_set_style_line_rounded(route, true, 0);
    lv_obj_set_style_line_dash_width(route, 9, 0);
    lv_obj_set_style_line_dash_gap(route, 7, 0);

    lv_obj_t* current = EasyGoUi::Panel(
        ui.mapContent,
        NavigationModel::MapInitialScrollX + 65,
        NavigationModel::MapInitialScrollY + 133,
        28,
        28,
        EasyGoUi::Blue,
        LV_RADIUS_CIRCLE
    );
    EasyGoUi::Symbol(current, LV_SYMBOL_UP, lv_color_white(), 5, 3, &lv_font_montserrat_20);
    lv_obj_t* target = EasyGoUi::Panel(
        ui.mapContent,
        NavigationModel::MapInitialScrollX + 162,
        NavigationModel::MapInitialScrollY + 27,
        31,
        31,
        EasyGoUi::Blue,
        LV_RADIUS_CIRCLE
    );
    EasyGoUi::Label(target, "WC", &lv_font_montserrat_12, lv_color_white(), 4, 8);

    lv_obj_update_layout(map);
    lv_obj_scroll_to(
        map,
        NavigationModel::MapInitialScrollX,
        NavigationModel::MapInitialScrollY,
        LV_ANIM_OFF
    );

    ui.zoomIn = EasyGoUi::Panel(root, 200, 178, 32, 34, lv_color_white(), 8);
    ui.zoomOut = EasyGoUi::Panel(root, 200, 214, 32, 34, lv_color_white(), 8);
    EasyGoUi::CardShadow(ui.zoomIn);
    EasyGoUi::CardShadow(ui.zoomOut);
    EasyGoUi::Pressable(ui.zoomIn);
    EasyGoUi::Pressable(ui.zoomOut);
    EasyGoUi::Symbol(ui.zoomIn, LV_SYMBOL_PLUS, EasyGoUi::Ink, 6, 6, &lv_font_montserrat_20);
    EasyGoUi::Symbol(ui.zoomOut, LV_SYMBOL_MINUS, EasyGoUi::Ink, 6, 6, &lv_font_montserrat_20);
    lv_obj_t* mapName = EasyGoUi::Panel(root, 7, 111, 52, 20, lv_color_white(), 6);
    lv_obj_set_style_border_color(mapName, lv_color_hex(0xD8DFDA), 0);
    lv_obj_set_style_border_width(mapName, 1, 0);
    EasyGoUi::Label(mapName, "宝安区", &font_easygo_12, EasyGoUi::Muted, 7, 2);
    EasyGoUi::Label(root, "© OpenStreetMap", &lv_font_montserrat_10, EasyGoUi::Muted, 122, 255);

    lv_obj_t* footer = EasyGoUi::Panel(root, 0, 269, 240, 51, lv_color_white(), 0);
    lv_obj_set_style_border_color(footer, lv_color_hex(0xDDE1E5), 0);
    lv_obj_set_style_border_width(footer, 1, 0);
    lv_obj_set_style_border_side(footer, LV_BORDER_SIDE_TOP, 0);
    (void)distanceMeters;
    (void)minutes;
    ui.status = EasyGoUi::Label(footer, "", &font_easygo_12, EasyGoUi::Muted, 7, 2, 226);
    ui.distance = EasyGoUi::Label(footer, "距离: 120 米", &font_easygo_16, EasyGoUi::Ink, 13, 25);
    EasyGoUi::Label(footer, "|", &lv_font_montserrat_20, EasyGoUi::Muted, 119, 22);
    ui.minutes = EasyGoUi::Label(footer, "约 2 分钟", &font_easygo_16, EasyGoUi::Ink, 145, 25);
}

void NavigationView::UpdateState(const AccountSystem::CareGo_State_t& state)
{
    if (!ui.status) return;
    lv_label_set_text(ui.status, state.statusText);
    lv_label_set_text_fmt(ui.topDistance, state.distanceMeters > 0 ? "%ld" : "--",
        static_cast<long>(state.distanceMeters));
    lv_label_set_text_fmt(ui.distance, state.distanceMeters > 0 ? "距离: %ld 米" : "距离: --",
        static_cast<long>(state.distanceMeters));
    lv_label_set_text_fmt(ui.minutes, state.minutes > 0 ? "约 %d 分钟" : "时间待定", state.minutes);
    if (state.targetName[0]) lv_label_set_text_fmt(ui.target, "前方: %s", state.targetName);
    else lv_label_set_text(ui.target, "正在获取目标");
}

void NavigationView::SetMapSource(const char* mapPath)
{
    if (!ui.mapImage || !mapPath) return;
    lv_img_set_src(ui.mapImage, mapPath);
    lv_obj_invalidate(ui.mapImage);
}
