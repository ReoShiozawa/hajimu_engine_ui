/**
 * src/plugin.c — jp-engine_ui はじむプラグインエントリ
 *
 * Copyright (c) 2026 Reo Shiozawa — MIT License
 */
#include "eng_ui.h"
#include <hajimu_plugin.h>
#include <stdio.h>
#include <string.h>

/* ── マクロ ──────────────────────────────────────────────*/
#define NEED(n)  if (argc < (n)) return hajimu_null()
#define NUM(i)   ((float)args[i].number)
#define STR(i)   (args[i].string.data)
#define BOOL_(i) (args[i].boolean)

/* ── 初期化・更新 ────────────────────────────────────────*/
static Value fn_ui_init(int argc, Value* args) {
    (void)argc; (void)args;
    ui_init();
    return hajimu_null();
}

static Value fn_ui_update(int argc, Value* args) {
    NEED(5);
    ui_update(NUM(0), NUM(1), BOOL_(2), BOOL_(3), BOOL_(4));
    return hajimu_null();
}

/* ── ヒットテスト ────────────────────────────────────────*/
static Value fn_ui_hover(int argc, Value* args) {
    NEED(4);
    return hajimu_bool(ui_hover(NUM(0), NUM(1), NUM(2), NUM(3)));
}
static Value fn_ui_click(int argc, Value* args) {
    NEED(4);
    return hajimu_bool(ui_click(NUM(0), NUM(1), NUM(2), NUM(3)));
}
static Value fn_ui_release(int argc, Value* args) {
    NEED(4);
    return hajimu_bool(ui_release(NUM(0), NUM(1), NUM(2), NUM(3)));
}
static Value fn_ui_held(int argc, Value* args) {
    NEED(4);
    return hajimu_bool(ui_held(NUM(0), NUM(1), NUM(2), NUM(3)));
}

/* ── ウィジェット ────────────────────────────────────────*/
static Value fn_ui_button(int argc, Value* args) {
    NEED(5);
    return hajimu_number((double)ui_button(
        (int)args[0].number, NUM(1), NUM(2), NUM(3), NUM(4)));
}

static Value fn_ui_checkbox(int argc, Value* args) {
    NEED(6);
    return hajimu_bool(ui_checkbox(
        (int)args[0].number, NUM(1), NUM(2), NUM(3), NUM(4), BOOL_(5)));
}

static Value fn_ui_slider(int argc, Value* args) {
    NEED(6);
    return hajimu_number((double)ui_slider(
        (int)args[0].number, NUM(1), NUM(2), NUM(3), NUM(4), NUM(5)));
}

static Value fn_ui_scroll(int argc, Value* args) {
    NEED(7);
    return hajimu_number((double)ui_scroll(
        (int)args[0].number, NUM(1), NUM(2), NUM(3), NUM(4), NUM(5), NUM(6)));
}

/* ── テキスト入力 ────────────────────────────────────────*/
static Value fn_ui_text_field(int argc, Value* args) {
    NEED(4);
    const char* s = ui_text_field(
        (int)args[0].number,
        STR(1),
        (int)args[2].number,
        (int)args[3].number);
    return hajimu_string(s);
}

static Value fn_ui_text_clear(int argc, Value* args) {
    NEED(1);
    ui_text_field_clear((int)args[0].number);
    return hajimu_null();
}

/* ── レイアウト ──────────────────────────────────────────*/
static Value fn_ui_layout_begin(int argc, Value* args) {
    NEED(4);
    ui_layout_begin(NUM(0), NUM(1), NUM(2), NUM(3));
    return hajimu_null();
}

static Value fn_ui_layout_next_row(int argc, Value* args) {
    NEED(1);
    ui_layout_next_row(NUM(0));
    return hajimu_null();
}

static Value fn_ui_layout_x(int argc, Value* args) {
    (void)argc; (void)args;
    return hajimu_number((double)ui_layout_x());
}

static Value fn_ui_layout_y(int argc, Value* args) {
    (void)argc; (void)args;
    return hajimu_number((double)ui_layout_y());
}

/* ── カラーヘルパー ─────────────────────────────────────*/
/* 返値: "r,g,b,a" の CSV 文字列 */
static Value fn_ui_btn_color(int argc, Value* args) {
    NEED(1);
    float r, g, b, a;
    ui_button_color((int)args[0].number, &r, &g, &b, &a);
    char buf[64];
    snprintf(buf, sizeof(buf), "%.3f,%.3f,%.3f,%.3f", r, g, b, a);
    return hajimu_string(buf);
}

/* 返値: "r,g,b" の CSV 文字列 */
static Value fn_ui_hp_color(int argc, Value* args) {
    NEED(1);
    float r, g, b;
    ui_hp_color(NUM(0), &r, &g, &b);
    char buf[48];
    snprintf(buf, sizeof(buf), "%.3f,%.3f,%.3f", r, g, b);
    return hajimu_string(buf);
}

/* ── 追加ウィジェット ────────────────────────────────────*/
static Value fn_ui_progress(int argc, Value* args) {
    NEED(6);
    return hajimu_number((double)ui_progress(
        (int)args[0].number, NUM(1), NUM(2), NUM(3), NUM(4), NUM(5)));
}

static Value fn_ui_radio(int argc, Value* args) {
    NEED(7);
    return hajimu_bool(ui_radio(
        (int)args[0].number, (int)args[1].number,
        NUM(2), NUM(3), NUM(4), NUM(5), BOOL_(6)));
}

static Value fn_ui_toggle(int argc, Value* args) {
    NEED(6);
    return hajimu_bool(ui_toggle(
        (int)args[0].number, NUM(1), NUM(2), NUM(3), NUM(4), BOOL_(5)));
}

/* ── v1.2.0 追加ウィジェット ─────────────────────────────*/
static Value fn_ui_dropdown(int argc, Value* args) {
    /* 引数: id, x, y, w, h, initial → 選択中インデックスを返す
     * items は jp 側で管理; C側はインデックスと開閉状態のみ */
    NEED(6);
    int id = (int)args[0].number;
    /* items はゲーム側で描画するため NULLを渡す */
    return hajimu_number(ui_dropdown(id, NUM(1), NUM(2), NUM(3), NUM(4),
                                     NULL, 0, (int)args[5].number));
}
static Value fn_ui_dropdown_open(int argc, Value* args) {
    NEED(1);
    return hajimu_bool(ui_dropdown_open((int)args[0].number));
}
static Value fn_ui_spinner(int argc, Value* args) {
    /* id, x, y, w, h, val, min, max, step */
    NEED(9);
    return hajimu_number(ui_spinner((int)args[0].number,
                                    NUM(1), NUM(2), NUM(3), NUM(4),
                                    NUM(5), NUM(6), NUM(7), NUM(8)));
}
static Value fn_ui_tab(int argc, Value* args) {
    /* id, group_id, x, y, w, h, initial */
    NEED(7);
    return hajimu_bool(ui_tab((int)args[0].number, (int)args[1].number,
                               NUM(2), NUM(3), NUM(4), NUM(5), BOOL_(6)));
}
static Value fn_ui_tab_selected(int argc, Value* args) {
    NEED(1);
    return hajimu_number(ui_tab_selected((int)args[0].number));
}

/* ── プラグインテーブル ─────────────────────────────────*/
static HajimuPluginFunc funcs[] = {
    /* 初期化・更新 */
    { "UI初期化",         fn_ui_init,          0, 0 },
    { "UI更新",           fn_ui_update,        5, 5 },
    /* ヒットテスト */
    { "UIホバー",         fn_ui_hover,         4, 4 },
    { "UIクリック",       fn_ui_click,         4, 4 },
    { "UI離した",         fn_ui_release,       4, 4 },
    { "UI押下中",         fn_ui_held,          4, 4 },
    /* ウィジェット */
    { "UIボタン",         fn_ui_button,        5, 5 },
    { "UIチェックボックス", fn_ui_checkbox,    6, 6 },
    { "UIスライダー",     fn_ui_slider,        6, 6 },
    { "UIスクロール",     fn_ui_scroll,        7, 7 },
    /* テキスト */
    { "UIテキスト入力",   fn_ui_text_field,    4, 4 },
    { "UIテキストクリア", fn_ui_text_clear,    1, 1 },
    /* レイアウト */
    { "UIレイアウト開始", fn_ui_layout_begin,  4, 4 },
    { "UI次行",           fn_ui_layout_next_row, 1, 1 },
    { "UIカーソルX",      fn_ui_layout_x,      0, 0 },
    { "UIカーソルY",      fn_ui_layout_y,      0, 0 },
    /* カラー */
    { "UIボタン色",       fn_ui_btn_color,     1, 1 },
    { "UIHPバー色",       fn_ui_hp_color,      1, 1 },
    /* 追加ウィジェット */
    { "UIプログレス",     fn_ui_progress,      6, 6 },
    { "UIラジオ",         fn_ui_radio,         7, 7 },
    { "UIトグル",         fn_ui_toggle,        6, 6 },
    /* v1.2.0 */
    { "UIドロップダウン",     fn_ui_dropdown,      6, 6 },
    { "UIドロップダウン開閉", fn_ui_dropdown_open, 1, 1 },
    { "UIスピナー",           fn_ui_spinner,       9, 9 },
    { "UIタブ",               fn_ui_tab,           7, 7 },
    { "UIタブ選択",           fn_ui_tab_selected,  1, 1 },
};

HAJIMU_PLUGIN_EXPORT HajimuPluginInfo* hajimu_plugin_init(void) {
    static HajimuPluginInfo info = {
        .name           = "engine_ui",
        .version        = "1.2.0",
        .functions      = funcs,
        .function_count = sizeof(funcs) / sizeof(funcs[0]),
    };
    return &info;
}
