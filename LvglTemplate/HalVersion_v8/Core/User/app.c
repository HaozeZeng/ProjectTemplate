#include "lvgl/lvgl.h"
#include "stdio.h"
#include "app.h"

// 定义获取屏幕宽度和高度的宏
#define scr_get_width() lv_obj_get_width(lv_scr_act())
#define scr_get_height() lv_obj_get_height(lv_scr_act())


static lv_obj_t* QQBtn;
static lv_obj_t* RankBtn;
static lv_obj_t* SettingBtn;
static lv_obj_t* TicketBtn;
static lv_obj_t* WeatherBtn;
static lv_obj_t* AliPayBtn;
static lv_obj_t* ExerciseBtn;
static lv_obj_t* GameBtn;
static lv_obj_t* githubBtn;
static lv_obj_t* SignBtn;
const char* APPNameList[] = {
    "QQ",
    "Rank",
    "Setting",
    "Ticket",
    "Weather",
    "AliPay",
    "Exercise",
    "Game",
    "Sign"
};
// 定义按钮数量
#define BUTTON_COUNT 9
lv_obj_t* buttons[BUTTON_COUNT];

// 声明平铺视图和视图对象
static lv_obj_t* tileview;
static lv_obj_t* view1;
static lv_obj_t* view2;
static void BtnsEvent_Cb(lv_event_t* e);
// 按钮分配函数
void BtnsAssignment(lv_obj_t** SCR, uint8_t len, uint8_t width, uint8_t height) {
    int temp = 0; // 当前页面索引
    int i = 0; // 图像编号

    while (i < len) {
        if (i && ((i % (width * height)) == 0)) {
            temp++;
        }
        buttons[i] = lv_imgbtn_create(SCR[temp]);
        lv_obj_set_size(buttons[i],50,50);
        //printf("button%d page%d\r\n", i, temp);
        i++;
    }

    // 将按钮指针存储到数组中
    QQBtn = buttons[0];
    RankBtn = buttons[1];
    SettingBtn = buttons[2];
    TicketBtn = buttons[3];
    WeatherBtn = buttons[4];
    AliPayBtn = buttons[5];
    ExerciseBtn = buttons[6];
    GameBtn = buttons[7];
    SignBtn = buttons[8];

    // 设置按钮的状态图像
	lv_imgbtn_set_src(QQBtn, LV_IMGBTN_STATE_RELEASED, NULL, "0:/PICTURE/icon/QQ.bin", NULL);
	lv_imgbtn_set_src(RankBtn, LV_IMGBTN_STATE_RELEASED, NULL, "0:/PICTURE/icon/Rank.bin", NULL);
	lv_imgbtn_set_src(SettingBtn, LV_IMGBTN_STATE_RELEASED, NULL, "0:/PICTURE/icon/Setting.bin", NULL);
	lv_imgbtn_set_src(TicketBtn, LV_IMGBTN_STATE_RELEASED, NULL, "0:/PICTURE/icon/Ticket.bin", NULL);
	lv_imgbtn_set_src(WeatherBtn, LV_IMGBTN_STATE_RELEASED, NULL, "0:/PICTURE/icon/Weather.bin", NULL);
	lv_imgbtn_set_src(AliPayBtn, LV_IMGBTN_STATE_RELEASED, NULL, "0:/PICTURE/icon/AliPay.bin", NULL);
	lv_imgbtn_set_src(ExerciseBtn, LV_IMGBTN_STATE_RELEASED, NULL, "0:/PICTURE/icon/Exercise.bin", NULL);
	lv_imgbtn_set_src(GameBtn, LV_IMGBTN_STATE_RELEASED, NULL, "0:/PICTURE/icon/Game.bin", NULL);
	lv_imgbtn_set_src(SignBtn, LV_IMGBTN_STATE_RELEASED, NULL, "0:/PICTURE/icon/Sign.bin", NULL);


}

// 定义最大视图数量
#define MaxViewNum 10
static int viewNum = 0;
static lv_obj_t* APPViewList[MaxViewNum];
static lv_obj_t* MenuView;

// 声明菜单按钮和滑块对象
lv_obj_t* WIFI_Btn;
lv_obj_t* BLUETOOTH_Btn;
lv_obj_t* LIGHT_Slider;
lv_obj_t* VOLUMN_Slider;
lv_obj_t* VOLUMN_Val;
lv_obj_t* LIGHT_Val;
lv_obj_t* LIGHT_LB;
lv_obj_t* VOLUMN_LB;

// 声明样式对象
lv_style_t FontStyle;

// 滑块事件回调函数
static void SliderEvent_Cb(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* tgt = (lv_obj_t*)lv_event_get_target(e);
    int val = 0;

    if (code == LV_EVENT_VALUE_CHANGED) {
        if (tgt == LIGHT_Slider) {
            val = lv_slider_get_value(LIGHT_Slider);
            lv_label_set_text_fmt(LIGHT_Val, "%d%%", val);
            //printf("LIGHT_Slider Val Change %d\r\n", val);
        }
        else {
            val = lv_slider_get_value(VOLUMN_Slider);
            lv_label_set_text_fmt(VOLUMN_Val, "%d%%", val);
            //printf("VOLUMN_Slider Val Change %d\r\n", val);
        }
    }
}

// 自动分配菜单按钮和滑块
void MenuAssignAuto(lv_obj_t* menuview) {
    MenuView = lv_tileview_add_tile(menuview, 0, 0, LV_DIR_BOTTOM);
    WIFI_Btn = lv_btn_create(MenuView);
    BLUETOOTH_Btn = lv_btn_create(MenuView);
    LIGHT_Slider = lv_slider_create(MenuView);
    VOLUMN_Slider = lv_slider_create(MenuView);
    LIGHT_Val = lv_label_create(LIGHT_Slider);
    VOLUMN_Val = lv_label_create(VOLUMN_Slider);
    LIGHT_LB = lv_label_create(MenuView);
    VOLUMN_LB = lv_label_create(MenuView);

    // 初始化字体样式
    lv_style_init(&FontStyle);
    lv_style_set_text_color(&FontStyle, lv_color_hex(0xFFFFFF)); // 设置文本颜色为白色
    lv_style_set_bg_opa(&FontStyle, 0); // 设置背景透明
    lv_style_set_border_width(&FontStyle, 0); // 设置边框宽度为0

    // 设置标签文本
    lv_label_set_text(LIGHT_LB, "LIGHT");
    lv_label_set_text(VOLUMN_LB, "VOLUME");
    lv_label_set_text(LIGHT_Val, "0");
    lv_label_set_text(VOLUMN_Val, "0");

    // 应用字体样式
    lv_obj_add_style(LIGHT_LB, &FontStyle, LV_STATE_DEFAULT);
    lv_obj_add_style(VOLUMN_LB, &FontStyle, LV_STATE_DEFAULT);
    lv_obj_add_style(VOLUMN_Val, &FontStyle, LV_STATE_DEFAULT);
    lv_obj_add_style(LIGHT_Val, &FontStyle, LV_STATE_DEFAULT);

    // 设置滑块范围
    lv_slider_set_range(LIGHT_Slider, 0, 100);
    lv_slider_set_range(VOLUMN_Slider, 0, 100);

    // 添加滑块事件回调
    lv_obj_add_event_cb(LIGHT_Slider, SliderEvent_Cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(VOLUMN_Slider, SliderEvent_Cb, LV_EVENT_VALUE_CHANGED, NULL);

    // 设置按钮和滑块的位置和大小
    lv_obj_set_size(WIFI_Btn, scr_get_width() / 3, scr_get_height() / 5);
    lv_obj_set_size(BLUETOOTH_Btn, scr_get_width() / 3, scr_get_height() / 5);
    lv_obj_set_size(LIGHT_Slider, scr_get_width() / 4, scr_get_height() / 2);
    lv_obj_set_size(VOLUMN_Slider, scr_get_width() / 4, scr_get_height() / 2);

    lv_obj_align(WIFI_Btn, LV_ALIGN_TOP_MID, -scr_get_width() / 4, scr_get_height() / 12);
    lv_obj_align(BLUETOOTH_Btn, LV_ALIGN_TOP_MID, scr_get_width() / 4, scr_get_height() / 12);

    lv_obj_align(LIGHT_Slider, LV_ALIGN_TOP_MID, -scr_get_width() / 4, scr_get_height() / 3);
    lv_obj_align(VOLUMN_Slider, LV_ALIGN_TOP_MID, scr_get_width() / 4, scr_get_height() / 3);

    // 设置按钮样式
    lv_obj_add_flag(WIFI_Btn, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_flag(BLUETOOTH_Btn, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_style_bg_color(WIFI_Btn, lv_color_hex(0x565656), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(BLUETOOTH_Btn, lv_color_hex(0x565656), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(WIFI_Btn, 150, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(BLUETOOTH_Btn, 150, LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(WIFI_Btn, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(BLUETOOTH_Btn, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(WIFI_Btn, lv_color_hex(0x47a7f1), LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(BLUETOOTH_Btn, lv_color_hex(0x47a7f1), LV_STATE_CHECKED);
    lv_obj_set_style_bg_img_src(WIFI_Btn, LV_SYMBOL_WIFI, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(BLUETOOTH_Btn, LV_SYMBOL_BLUETOOTH, LV_STATE_DEFAULT);

    // 设置滑块样式
    lv_obj_set_style_bg_color(LIGHT_Slider, lv_color_hex(0xfafafa), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(LIGHT_Slider, lv_color_hex(0x565656), LV_PART_MAIN);
    lv_obj_set_style_bg_color(VOLUMN_Slider, lv_color_hex(0xfafafa), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(VOLUMN_Slider, lv_color_hex(0x565656), LV_PART_MAIN);

    // 设置标签位置
    lv_obj_set_style_text_color(LIGHT_Val, lv_color_hex(0xFF0000), LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(VOLUMN_Val, lv_color_hex(0xFF0000), LV_STATE_DEFAULT);
    lv_obj_align_to(LIGHT_LB, LIGHT_Slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_align_to(VOLUMN_LB, VOLUMN_Slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_align(LIGHT_Val, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align(VOLUMN_Val, LV_ALIGN_CENTER, 0, 0);

    // 移除滑块的旋钮样式
    lv_obj_remove_style(LIGHT_Slider, NULL, LV_PART_KNOB);
    lv_obj_remove_style(VOLUMN_Slider, NULL, LV_PART_KNOB);
    //事件
    lv_obj_add_event_cb(WIFI_Btn, BtnsEvent_Cb, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(BLUETOOTH_Btn, BtnsEvent_Cb, LV_EVENT_PRESSED, NULL);

}

// 自动创建按钮视图
void BtnViewCreateAuto(lv_obj_t* win, uint8_t len, uint8_t width, uint8_t height) {
    int temp = len;
    while (temp > 0) {
        // 添加新的视图
        APPViewList[viewNum] = lv_tileview_add_tile(win, viewNum, 1, LV_DIR_ALL);
        viewNum++;
        temp -= width * height;
    }
}

void BtnAlignAuto(int page, lv_obj_t** objlist, uint8_t len, uint8_t width, uint8_t height)
{
    int i = 0;
    // 放大计算中心偏移量

    float centerX = (width - 1) / 2.0; // 放大计算
    float centerY = (height - 1) / 2.0; // 放大计算

    for (int ih = 0; ih < height; ih++)
    {
        for (int iw = 0; iw < width; iw++)
        {
            // 计算对齐位置，放大计算后再缩小
            //printf("objlist[%d]\r\n", i + page * width * height);
            lv_obj_align(objlist[i + page * width * height], LV_ALIGN_CENTER,
                (scr_get_width() / width * (iw - centerX)),
                (scr_get_height() / height * (ih - centerY)));


            if (++i >= len) // 使用 len 作为限制
            {
                return;
            }
        }
    }

}







lv_obj_t* statusBar;
lv_obj_t* statusWifi;
lv_obj_t* statusBlueTooth;
lv_obj_t* wins[BUTTON_COUNT];
lv_obj_t* win_contents[BUTTON_COUNT];
lv_obj_t* win_heads[BUTTON_COUNT];
lv_obj_t* win_titles[BUTTON_COUNT];
lv_obj_t* win_btnCloses[BUTTON_COUNT];
static int current;
static bool WIFI_BtnChecked = 0;
static bool BLUETOOTH_BtnChecked = 0;
static void BtnsEvent_Cb(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* tgt = (lv_obj_t*)lv_event_get_target(e);
    for (int i = 0; i < BUTTON_COUNT; i++)
    {
        if (tgt == buttons[i])
        {
            current = i;
            break;
        }
    }
    if (code == LV_EVENT_FOCUSED) {
        lv_obj_set_style_bg_color(tgt, lv_color_hex(0x565656), LV_STATE_FOCUSED);
        lv_obj_set_style_bg_opa(tgt, 150, LV_STATE_FOCUSED);
        lv_obj_clear_flag(wins[current], LV_OBJ_FLAG_HIDDEN);

    }
    if (code == LV_EVENT_CLICKED && tgt == win_btnCloses[current]) {

        lv_obj_add_flag(wins[current], LV_OBJ_FLAG_HIDDEN);

    }
    if (code == LV_EVENT_PRESSED && tgt == WIFI_Btn) {

        WIFI_BtnChecked = !WIFI_BtnChecked;
    }
    if (code == LV_EVENT_PRESSED && tgt == BLUETOOTH_Btn) {

        BLUETOOTH_BtnChecked = !BLUETOOTH_BtnChecked;
    }

}
void BtnEventAssignment()
{
    lv_obj_add_event_cb(QQBtn, BtnsEvent_Cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(AliPayBtn, BtnsEvent_Cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(ExerciseBtn, BtnsEvent_Cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(GameBtn, BtnsEvent_Cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(SignBtn, BtnsEvent_Cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(TicketBtn, BtnsEvent_Cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(WeatherBtn, BtnsEvent_Cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(SettingBtn, BtnsEvent_Cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(RankBtn, BtnsEvent_Cb, LV_EVENT_FOCUSED, NULL);
}
void APPBtnAssign(lv_obj_t* Maintileview, int BtnNum, int width, int height)
{
    int temp = BtnNum;
    BtnViewCreateAuto(Maintileview, BtnNum, width, height);
    BtnsAssignment(APPViewList, BtnNum, width, height);
    //按键对齐
    for (int i = 0; i < viewNum; i++)
    {
        BtnAlignAuto(i, buttons, temp, width, height);
        temp -= width * height;
    }
    BtnEventAssignment();
}
lv_style_t btnCloseStyle;
lv_style_t winContentsStyle;
lv_style_t winTitlesStyle;
lv_style_t winHeadsStyle;
void AppWin()
{
    lv_style_init(&btnCloseStyle);
    lv_style_init(&winContentsStyle);
    lv_style_init(&winTitlesStyle);
    lv_style_init(&winHeadsStyle);

    lv_style_set_bg_opa(&btnCloseStyle,0);
    lv_style_set_shadow_width(&btnCloseStyle,0);
    lv_style_set_text_color(&btnCloseStyle,lv_color_hex(0xFF0000));

    lv_style_set_bg_color(&winContentsStyle, lv_color_hex(0x565656));
    lv_style_set_shadow_width(&winContentsStyle, 0);
    //lv_style_set_text_color(&winContentsStyle, lv_color_hex(0xFF0000));

    lv_style_set_bg_color(&winTitlesStyle, lv_color_hex(0x367656));
    lv_style_set_shadow_width(&winTitlesStyle, 0);
    //lv_style_set_text_font(&winTitlesStyle, LV_FONT_MONTSERRAT_12);
    lv_style_set_text_color(&winTitlesStyle, lv_color_hex(0x00FF56));

    lv_style_set_bg_color(&winHeadsStyle, lv_color_hex(0x120056));
//    lv_style_set_size(&winHeadsStyle, scr_get_width(),scr_get_height()/12 );
    lv_style_set_shadow_width(&winHeadsStyle, 0);

    for (int i = 0; i < sizeof(APPNameList)/sizeof(APPNameList[0]); i++)
    {
        wins[i] = lv_win_create(lv_scr_act(),10);
        win_titles[i] = lv_win_add_title(wins[i], APPNameList[i]);
        win_btnCloses[i] = lv_win_add_btn(wins[i], LV_SYMBOL_CLOSE, 20);
        win_contents[i] = lv_win_get_content(wins[i]);
        win_heads[i] = lv_win_get_header(wins[i]);

        lv_obj_add_event_cb(win_btnCloses[i], BtnsEvent_Cb, LV_EVENT_CLICKED, NULL);
        lv_obj_add_flag(wins[i], LV_OBJ_FLAG_HIDDEN);

        //lv_obj_set_style_bg_color(win_contents[i], lv_color_hex(0x565656),LV_STATE_DEFAULT);
        lv_obj_add_style(win_btnCloses[i], &btnCloseStyle, LV_STATE_DEFAULT);
        lv_obj_add_style(win_contents[i], &winContentsStyle, LV_STATE_DEFAULT);
        lv_obj_add_style(win_titles[i], &winTitlesStyle, LV_STATE_DEFAULT);
        lv_obj_add_style(win_heads[i], &winHeadsStyle, LV_STATE_DEFAULT);
    }
}
static void statusBarEvent_Cb(lv_timer_t* timer)
{
    static int time = 1;
    lv_label_set_text_fmt(statusBar, "2024/12/29/%d", time);
    time++;
    if (WIFI_BtnChecked)
    {
        lv_obj_clear_flag(statusWifi, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        lv_obj_add_flag(statusWifi, LV_OBJ_FLAG_HIDDEN);
    }
    if (BLUETOOTH_BtnChecked)
    {
        lv_obj_clear_flag(statusBlueTooth, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        lv_obj_add_flag(statusBlueTooth, LV_OBJ_FLAG_HIDDEN);
    }

}
lv_timer_t* timer;
void statusBarAssign()
{
    statusWifi = lv_label_create(lv_scr_act());
    statusBlueTooth = lv_label_create(lv_scr_act());
    statusBar = lv_label_create(lv_scr_act());
    lv_obj_set_style_bg_img_src(statusWifi, LV_SYMBOL_WIFI, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(statusBlueTooth, LV_SYMBOL_BLUETOOTH, LV_STATE_DEFAULT);
    lv_obj_add_style(statusBar, &FontStyle, LV_STATE_DEFAULT);
    lv_label_set_text(statusWifi, "");
    lv_label_set_text(statusBlueTooth, "");
    lv_obj_set_size(statusWifi, 20, 20);
    lv_obj_set_size(statusBlueTooth, 20, 20);

    lv_obj_set_style_text_color(statusWifi, lv_color_hex(0xFFFFFF), LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(statusBlueTooth, lv_color_hex(0xFFFFFF), LV_STATE_DEFAULT);

    lv_obj_align(statusBar, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_align_to(statusWifi, statusBar, LV_ALIGN_RIGHT_MID, scr_get_width() / 3, 0);
    lv_obj_align_to(statusBlueTooth, statusWifi, LV_ALIGN_RIGHT_MID, 20, 0);

    lv_obj_add_flag(statusWifi, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(statusBlueTooth, LV_OBJ_FLAG_HIDDEN);
    lv_timer_create(statusBarEvent_Cb, 1000, NULL);
}


void app(void)
{
    tileview = lv_tileview_create(lv_scr_act());
    APPBtnAssign(tileview, BUTTON_COUNT, 2, 2);
    MenuAssignAuto(tileview);
    statusBarAssign();
    AppWin();
    //TODO 顶部状态栏

    lv_obj_set_style_bg_color(tileview, lv_color_hex(0x1e1e1e), LV_STATE_DEFAULT);

}

