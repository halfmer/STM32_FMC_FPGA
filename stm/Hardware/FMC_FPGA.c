#include "stm32h7xx.h"
#include "main.h"
#include "FMC_FPGA.h"
#include <stdio.h>
#include <string.h>
#include "LCD_Init.h"
#include "LCD.h"
#include "arm_math.h"

uint16_t state = 1;
uint16_t addr1_data;
uint16_t addr2_data;
uint16_t addr3_data;

#define FPGA_LENGTH 1024
#define HEARTBEAT_TIMEOUT_MS 2500U

#define UI_COLOR_BG       0xF7BEU
#define UI_COLOR_HEADER   DARKBLUE
#define UI_COLOR_SUBHEAD  GRAYBLUE
#define UI_COLOR_PANEL    WHITE
#define UI_COLOR_ROW_ALT  0xEF7DU
#define UI_COLOR_GRID     LGRAY
#define UI_COLOR_TEXT     BLACK
#define UI_COLOR_MUTED    GRAY
#define UI_COLOR_PASS     GREEN
#define UI_COLOR_FAIL     RED
#define UI_ROW_COUNT      6U
#define UI_TEXT_LEN       16U

typedef struct
{
    uint16_t screen_w;
    uint16_t screen_h;
    uint16_t margin;
    uint16_t header_h;
    uint16_t status_y;
    uint16_t status_h;
    uint16_t table_x;
    uint16_t table_y;
    uint16_t table_w;
    uint16_t row_h;
    uint16_t label_x;
    uint16_t expect_x;
    uint16_t read_x;
    uint16_t state_x;
    uint8_t title_size;
    uint8_t row_size;
    uint8_t small_size;
} UiLayout;

typedef struct
{
    uint8_t valid;
    uint8_t ok;
    char read[UI_TEXT_LEN];
} UiRowCache;

static UiRowCache ui_row_cache[UI_ROW_COUNT];
static uint8_t ui_status_valid = 0U;
static uint8_t ui_status_ok = 0U;

static void FPGA_opt_delay(uint32_t i);
static void UI_BuildLayout(UiLayout *ui);
static void UI_ResetDynamicCache(void);
static void UI_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
static void UI_ShowText(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                        uint8_t size, const char *text, uint16_t color, uint16_t bg);
static uint16_t UI_StatusColor(uint8_t ok);
static void UI_DrawStaticFrame(const UiLayout *ui);
static void UI_DrawStatusBand(const UiLayout *ui, uint8_t all_ok, uint32_t now);
static void UI_DrawStaticRow(const UiLayout *ui, uint8_t index,
                             const char *label, const char *expect);
static void UI_UpdateRowValue(const UiLayout *ui, uint8_t index,
                              const char *read, uint8_t ok);

void FMC_DET_DATA(uint16_t *FMC_NUM, uint16_t *FMC_ADC_DATA)
{
    state = 1;
    addr1_data = 0;
    addr2_data = 0;

    while (state == 1) {
        FMC_WR_ADDR(0x01);
        addr1_data = FMC_RD_DATA();
        if (addr1_data == 255) {
            for (int i = 0; i < FPGA_LENGTH + 3; i++) {
                FMC_WR_ADDR(0x02);
                addr2_data = FMC_RD_DATA();
                FMC_WR_ADDR(0x03);
                addr3_data = FMC_RD_DATA();

                if (addr2_data > 3 && addr2_data < FPGA_LENGTH + 4) {
                    FMC_ADC_DATA[addr2_data - 4] = addr3_data;
                    FMC_NUM[addr2_data - 4] = addr2_data;
                }
            }
            state = 0;
            addr1_data = 0;
            addr2_data = 0;
        }
    }
}

void FMC_WR_ADDR(volatile uint16_t addr)
{
    FPGA->FPGA_REG = addr;
    __DSB();
    FPGA_opt_delay(300);  /* Wait for FPGA address synchronization. */
}

void FMC_WR_DATA(volatile uint16_t data)
{
    FPGA->FPGA_RAM = data;
    __DSB();
    FPGA_opt_delay(300);  /* Wait for FPGA data synchronization. */
}

void FMC_WriteReg(uint16_t addr, uint16_t data)
{
    FMC_WR_ADDR(addr);
    FMC_WR_DATA(data);
}

static void FPGA_opt_delay(uint32_t i)
{
    volatile uint32_t n = i;
    while (n--) {
        __NOP();
    }
}

uint16_t FMC_RD_DATA(void)
{
    volatile uint16_t ram;

    __DSB();
    FPGA_opt_delay(120);  /* Wait for bus turn-around and FPGA output settle. */
    ram = FPGA->FPGA_RAM;
    __DSB();
    return ram;
}

static void UI_BuildLayout(UiLayout *ui)
{
    uint8_t portrait;
    uint32_t row_h;

    ui->screen_w = (lcddev.width > 0U) ? lcddev.width : 480U;
    ui->screen_h = (lcddev.height > 0U) ? lcddev.height : 800U;
    portrait = (ui->screen_h > ui->screen_w);

    ui->margin = portrait ? 20U : 24U;
    if (ui->screen_w < 500U) {
        ui->margin = 16U;
    }

    ui->header_h = portrait ? 72U : 64U;
    ui->status_y = ui->header_h + (portrait ? 18U : 14U);
    ui->status_h = portrait ? 106U : 74U;
    ui->table_x = ui->margin;
    ui->table_y = ui->status_y + ui->status_h + (portrait ? 22U : 14U);
    ui->table_w = ui->screen_w - (ui->margin * 2U);

    if (ui->screen_h > (ui->table_y + ui->margin)) {
        row_h = (uint32_t)(ui->screen_h - ui->table_y - ui->margin) / 7U;
    } else {
        row_h = 38U;
    }
    if (row_h < 38U) {
        row_h = 38U;
    } else if (row_h > (portrait ? 72U : 48U)) {
        row_h = portrait ? 72U : 48U;
    }
    ui->row_h = (uint16_t)row_h;

    ui->label_x = ui->table_x + 12U;
    ui->expect_x = ui->table_x + (uint16_t)((uint32_t)ui->table_w * 28U / 100U);
    ui->read_x = ui->table_x + (uint16_t)((uint32_t)ui->table_w * 52U / 100U);
    ui->state_x = ui->table_x + (uint16_t)((uint32_t)ui->table_w * 78U / 100U);

    ui->title_size = (ui->screen_w < 430U) ? 24U : 32U;
    ui->row_size = (ui->screen_w < 430U) ? 16U : 24U;
    ui->small_size = 16U;
}

static void UI_ResetDynamicCache(void)
{
    uint8_t i;

    ui_status_valid = 0U;
    ui_status_ok = 0U;
    for (i = 0U; i < UI_ROW_COUNT; i++) {
        ui_row_cache[i].valid = 0U;
        ui_row_cache[i].ok = 0U;
        ui_row_cache[i].read[0] = '\0';
    }
}

static void UI_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    uint32_t ex;
    uint32_t ey;

    if ((w == 0U) || (h == 0U) || (x >= lcddev.width) || (y >= lcddev.height)) {
        return;
    }

    ex = (uint32_t)x + (uint32_t)w - 1U;
    ey = (uint32_t)y + (uint32_t)h - 1U;

    if (ex >= lcddev.width) {
        ex = (uint32_t)lcddev.width - 1U;
    }
    if (ey >= lcddev.height) {
        ey = (uint32_t)lcddev.height - 1U;
    }

    LCD_Fill(x, y, (uint16_t)ex, (uint16_t)ey, color);
}

static void UI_ShowText(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                        uint8_t size, const char *text, uint16_t color, uint16_t bg)
{
    uint32_t old_bg;

    old_bg = BACK_COLOR;
    BACK_COLOR = bg;
    LCD_Show_String(x, y, w, h, size, (char *)text, color);
    BACK_COLOR = old_bg;
}

static uint16_t UI_StatusColor(uint8_t ok)
{
    return ok ? UI_COLOR_PASS : UI_COLOR_FAIL;
}

static void UI_DrawStaticFrame(const UiLayout *ui)
{
    uint16_t title_y;
    uint16_t table_header_y;
    uint16_t table_bottom_y;

    title_y = (uint16_t)((ui->header_h - ui->title_size) / 2U);
    table_header_y = ui->table_y;
    table_bottom_y = ui->table_y + (ui->row_h * 7U) - 1U;
    if (table_bottom_y >= ui->screen_h) {
        table_bottom_y = ui->screen_h - 1U;
    }

    UI_FillRect(0U, 0U, ui->screen_w, ui->screen_h, UI_COLOR_BG);
    UI_FillRect(0U, 0U, ui->screen_w, ui->header_h, UI_COLOR_HEADER);
    UI_ShowText(ui->margin, title_y, ui->screen_w - (ui->margin * 2U),
                ui->title_size, ui->title_size, "FMC FPGA LINK MONITOR",
                WHITE, UI_COLOR_HEADER);

    UI_FillRect(ui->table_x, ui->status_y, ui->table_w, ui->status_h, UI_COLOR_PANEL);
    LCD_Draw_Rectangle(ui->table_x, ui->status_y,
                       ui->table_x + ui->table_w - 1U,
                       ui->status_y + ui->status_h - 1U,
                       UI_COLOR_GRID);

    UI_FillRect(ui->table_x, table_header_y, ui->table_w, ui->row_h, UI_COLOR_SUBHEAD);
    LCD_Draw_Rectangle(ui->table_x, table_header_y,
                       ui->table_x + ui->table_w - 1U,
                       table_bottom_y,
                       UI_COLOR_GRID);

    UI_ShowText(ui->label_x, table_header_y + ((ui->row_h - ui->small_size) / 2U),
                ui->expect_x - ui->label_x - 4U, ui->small_size,
                ui->small_size, "ITEM", WHITE, UI_COLOR_SUBHEAD);
    UI_ShowText(ui->expect_x, table_header_y + ((ui->row_h - ui->small_size) / 2U),
                ui->read_x - ui->expect_x - 4U, ui->small_size,
                ui->small_size, "EXPECT", WHITE, UI_COLOR_SUBHEAD);
    UI_ShowText(ui->read_x, table_header_y + ((ui->row_h - ui->small_size) / 2U),
                ui->state_x - ui->read_x - 4U, ui->small_size,
                ui->small_size, "READ", WHITE, UI_COLOR_SUBHEAD);
    UI_ShowText(ui->state_x, table_header_y + ((ui->row_h - ui->small_size) / 2U),
                ui->table_x + ui->table_w - ui->state_x - 8U, ui->small_size,
                ui->small_size, "STATE", WHITE, UI_COLOR_SUBHEAD);
}

static void UI_DrawStatusBand(const UiLayout *ui, uint8_t all_ok, uint32_t now)
{
    uint16_t badge_w;
    uint16_t badge_h;
    uint16_t badge_x;
    uint16_t badge_y;
    uint16_t info_x;
    uint16_t info_w;
    uint16_t color;

    (void)now;
    if ((ui_status_valid != 0U) && (ui_status_ok == all_ok)) {
        return;
    }

    color = UI_StatusColor(all_ok);
    badge_w = (uint16_t)((uint32_t)ui->table_w * 38U / 100U);
    if (badge_w < 154U) {
        badge_w = 154U;
    }
    if (badge_w > (ui->table_w - 24U)) {
        badge_w = ui->table_w - 24U;
    }

    badge_h = ui->status_h - 20U;
    badge_x = ui->table_x + 10U;
    badge_y = ui->status_y + 10U;
    info_x = badge_x + badge_w + 14U;
    info_w = ui->table_x + ui->table_w - info_x - 10U;

    UI_FillRect(ui->table_x + 1U, ui->status_y + 1U,
                ui->table_w - 2U, ui->status_h - 2U, UI_COLOR_PANEL);
    UI_FillRect(badge_x, badge_y, badge_w, badge_h, color);
    UI_ShowText(badge_x + 14U, badge_y + ((badge_h > 32U) ? ((badge_h - 32U) / 2U) : 0U),
                badge_w - 20U, 32U, 32U,
                all_ok ? "LINK PASS" : "LINK FAIL", WHITE, color);

    if (info_w > 80U) {
        UI_ShowText(info_x, badge_y + 4U, info_w, 24U, 24U,
                    "STM32 FMC <-> FPGA", UI_COLOR_TEXT, UI_COLOR_PANEL);
        UI_ShowText(info_x, badge_y + 36U, info_w, 24U, 24U,
                    "FPGA status", UI_COLOR_MUTED, UI_COLOR_PANEL);
    }

    ui_status_valid = 1U;
    ui_status_ok = all_ok;
}

static void UI_DrawStaticRow(const UiLayout *ui, uint8_t index,
                             const char *label, const char *expect)
{
    uint16_t y;
    uint16_t text_y;
    uint16_t line_y;
    uint16_t row_bg;

    y = ui->table_y + ui->row_h + ((uint16_t)index * ui->row_h);
    if (y >= ui->screen_h) {
        return;
    }
    text_y = y + (uint16_t)((ui->row_h - ui->row_size) / 2U);
    line_y = y + ui->row_h - 1U;
    if (line_y >= ui->screen_h) {
        line_y = ui->screen_h - 1U;
    }
    row_bg = (index & 1U) ? UI_COLOR_ROW_ALT : UI_COLOR_PANEL;

    UI_FillRect(ui->table_x + 1U, y, ui->table_w - 2U, ui->row_h, row_bg);
    LCD_Draw_Line(ui->table_x, line_y,
                  ui->table_x + ui->table_w - 1U, line_y,
                  UI_COLOR_GRID);

    UI_ShowText(ui->label_x, text_y, ui->expect_x - ui->label_x - 4U,
                ui->row_size, ui->row_size, label, UI_COLOR_TEXT, row_bg);
    UI_ShowText(ui->expect_x, text_y, ui->read_x - ui->expect_x - 4U,
                ui->row_size, ui->row_size, expect, UI_COLOR_MUTED, row_bg);
}

static void UI_UpdateRowValue(const UiLayout *ui, uint8_t index,
                              const char *read, uint8_t ok)
{
    uint16_t y;
    uint16_t text_y;
    uint16_t state_w;
    uint16_t read_w;
    uint16_t badge_h;
    uint16_t badge_y;
    uint16_t row_bg;
    uint16_t color;
    uint8_t read_changed;
    uint8_t ok_changed;

    if (index >= UI_ROW_COUNT) {
        return;
    }
    read_changed = ((ui_row_cache[index].valid == 0U) ||
                    (strncmp(ui_row_cache[index].read, read, UI_TEXT_LEN) != 0));
    ok_changed = ((ui_row_cache[index].valid == 0U) ||
                  (ui_row_cache[index].ok != ok));
    if ((read_changed == 0U) && (ok_changed == 0U)) {
        return;
    }

    y = ui->table_y + ui->row_h + ((uint16_t)index * ui->row_h);
    if (y >= ui->screen_h) {
        return;
    }
    text_y = y + (uint16_t)((ui->row_h - ui->row_size) / 2U);
    read_w = ui->state_x - ui->read_x - 4U;
    state_w = ui->table_x + ui->table_w - ui->state_x - 12U;
    badge_h = (ui->row_h > 34U) ? 28U : (ui->row_h - 8U);
    badge_y = y + (uint16_t)((ui->row_h - badge_h) / 2U);
    row_bg = (index & 1U) ? UI_COLOR_ROW_ALT : UI_COLOR_PANEL;
    color = UI_StatusColor(ok);

    if (read_changed != 0U) {
        UI_FillRect(ui->read_x, y + 2U, read_w, ui->row_h - 4U, row_bg);
        UI_ShowText(ui->read_x, text_y, read_w, ui->row_size,
                    ui->row_size, read, UI_COLOR_TEXT, row_bg);
    }

    if (ok_changed != 0U) {
        UI_FillRect(ui->state_x, badge_y, state_w, badge_h, color);
        UI_ShowText(ui->state_x + 10U, badge_y + ((badge_h > 16U) ? ((badge_h - 16U) / 2U) : 0U),
                    state_w - 12U, 16U, 16U, ok ? "OK" : "ERR", WHITE, color);
    }

    ui_row_cache[index].valid = 1U;
    ui_row_cache[index].ok = ok;
    strncpy(ui_row_cache[index].read, read, UI_TEXT_LEN - 1U);
    ui_row_cache[index].read[UI_TEXT_LEN - 1U] = '\0';
}

/* Board self-test: read known FPGA registers and show PASS/FAIL on LCD. */
void FMC_SelfTest_Display(void)
{
    char buf[40];
    UiLayout ui;
    uint16_t sig, cst, loop, hb50, hb130, stat;
    uint32_t now;
    static uint8_t ui_ready = 0;
    static uint16_t ui_w = 0;
    static uint16_t ui_h = 0;
    static uint8_t hb_init = 0;
    static uint16_t hb50_prev = 0;
    static uint16_t hb130_prev = 0;
    static uint32_t hb50_last_ms = 0;
    static uint32_t hb130_last_ms = 0;
    uint8_t ok_sig, ok_cst, ok_loop, ok_hb50, ok_pll, ok_stat, all;
    uint8_t hb50_back = 0;
    uint8_t hb130_back = 0;

    now = HAL_GetTick();
    FMC_WR_ADDR(0x0000); sig = FMC_RD_DATA();
    FMC_WR_ADDR(0x0001); cst = FMC_RD_DATA();
    FMC_WR_ADDR(0x0002); hb50 = FMC_RD_DATA();
    FMC_WR_ADDR(0x0004); hb130 = FMC_RD_DATA();
    FMC_WR_ADDR(0x0005); stat = FMC_RD_DATA();

    /* Loopback register check. */
    FMC_WR_ADDR(0x0003);
    FMC_WR_DATA(0x55AA);
    FMC_WR_ADDR(0x0003);
    loop = FMC_RD_DATA();

    if (!hb_init) {
        hb50_prev = hb50;
        hb130_prev = hb130;
        hb50_last_ms = now;
        hb130_last_ms = now;
        hb_init = 1;
    } else {
        hb50_back = (hb50 < hb50_prev);
        hb130_back = (hb130 < hb130_prev);
        if (hb50 != hb50_prev) {
            hb50_prev = hb50;
            hb50_last_ms = now;
        }
        if (hb130 != hb130_prev) {
            hb130_prev = hb130;
            hb130_last_ms = now;
        }
    }

    ok_sig = (sig == 0xA55A);
    ok_cst = (cst == 0x1234);
    ok_loop = (loop == 0x55AA);
    ok_hb50 = (!hb50_back) && ((now - hb50_last_ms) < HEARTBEAT_TIMEOUT_MS);
    ok_pll = (!hb130_back) && ((now - hb130_last_ms) < HEARTBEAT_TIMEOUT_MS);
    ok_stat = ((stat & 0xFFF8U) == 0U) && ((stat & 0x0006U) == 0x0006U);
    all = ok_sig && ok_cst && ok_loop && ok_hb50 && ok_pll && ok_stat;

    UI_BuildLayout(&ui);
    if ((!ui_ready) || (ui_w != ui.screen_w) || (ui_h != ui.screen_h)) {
        UI_DrawStaticFrame(&ui);
        UI_DrawStaticRow(&ui, 0U, "SIGN", "A55A");
        UI_DrawStaticRow(&ui, 1U, "CONST", "1234");
        UI_DrawStaticRow(&ui, 2U, "LOOP", "55AA");
        UI_DrawStaticRow(&ui, 3U, "HB50", "RUN");
        UI_DrawStaticRow(&ui, 4U, "HB130", "RUN");
        UI_DrawStaticRow(&ui, 5U, "STAT", "S1 P1");
        UI_ResetDynamicCache();
        ui_w = ui.screen_w;
        ui_h = ui.screen_h;
        ui_ready = 1U;
    }

    UI_DrawStatusBand(&ui, all, now);

    sprintf(buf, "%04X", (unsigned int)sig);
    UI_UpdateRowValue(&ui, 0U, buf, ok_sig);
    sprintf(buf, "%04X", (unsigned int)cst);
    UI_UpdateRowValue(&ui, 1U, buf, ok_cst);
    sprintf(buf, "%04X", (unsigned int)loop);
    UI_UpdateRowValue(&ui, 2U, buf, ok_loop);
    sprintf(buf, "%5u", (unsigned int)hb50);
    UI_UpdateRowValue(&ui, 3U, buf, ok_hb50);
    sprintf(buf, "%5u", (unsigned int)hb130);
    UI_UpdateRowValue(&ui, 4U, buf, ok_pll);
    sprintf(buf, "R%u S%u P%u",
            (unsigned int)(stat & 1U),
            (unsigned int)((stat >> 1) & 1U),
            (unsigned int)((stat >> 2) & 1U));
    UI_UpdateRowValue(&ui, 5U, buf, ok_stat);
}
