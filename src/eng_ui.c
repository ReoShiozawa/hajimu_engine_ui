/**
 * src/eng_ui.c — IMGUI スタイル UI 実装
 *
 * Copyright (c) 2026 Reo Shiozawa — MIT License
 */
#include "eng_ui.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

/* ── グローバル UI 状態 ──────────────────────────────────*/
#define UI_MAX_WIDGETS 128
#define UI_MAX_TEXTFIELDS 16
#define UI_TEXTFIELD_LEN 256

typedef struct {
    int   id;
    bool  used;
    /* チェックボックス */
    bool  checked;
    /* スライダー */
    float norm_val;
    /* スクロール */
    float scroll;
    /* ボタン前フレーム状態 */
    int   btn_state;
} UIWidget;

typedef struct {
    int  id;
    bool used;
    char buf[UI_TEXTFIELD_LEN];
} UITextField;

static struct {
    float mx, my;
    bool  is_down;
    bool  just_clicked;
    bool  just_released;
    UIWidget     widgets[UI_MAX_WIDGETS];
    UITextField  fields[UI_MAX_TEXTFIELDS];
    /* レイアウト */
    float layout_x, layout_y, layout_col_w, layout_gap;
} g;

/* ── ヘルパー ────────────────────────────────────────────*/
static bool rect_contains(float x, float y, float w, float h,
                           float px, float py) {
    return px >= x && px < x+w && py >= y && py < y+h;
}

static UIWidget* widget_get(int id) {
    for (int i = 0; i < UI_MAX_WIDGETS; ++i) {
        if (g.widgets[i].used && g.widgets[i].id == id)
            return &g.widgets[i];
    }
    /* 新規作成 */
    for (int i = 0; i < UI_MAX_WIDGETS; ++i) {
        if (!g.widgets[i].used) {
            g.widgets[i].id      = id;
            g.widgets[i].used    = true;
            g.widgets[i].norm_val = 0.5f;
            return &g.widgets[i];
        }
    }
    return NULL;
}

static UITextField* field_get(int id) {
    for (int i = 0; i < UI_MAX_TEXTFIELDS; ++i)
        if (g.fields[i].used && g.fields[i].id == id) return &g.fields[i];
    for (int i = 0; i < UI_MAX_TEXTFIELDS; ++i) {
        if (!g.fields[i].used) {
            g.fields[i].id   = id;
            g.fields[i].used = true;
            g.fields[i].buf[0] = '\0';
            return &g.fields[i];
        }
    }
    return NULL;
}

/* ── 初期化・更新 ────────────────────────────────────────*/
void ui_init(void) { memset(&g, 0, sizeof(g)); }

void ui_update(float mx, float my, bool is_down,
               bool just_clicked, bool just_released) {
    g.mx = mx; g.my = my;
    g.is_down      = is_down;
    g.just_clicked  = just_clicked;
    g.just_released = just_released;
}

/* ── ヒットテスト ────────────────────────────────────────*/
bool ui_hover(float x, float y, float w, float h) {
    return rect_contains(x, y, w, h, g.mx, g.my);
}
bool ui_click(float x, float y, float w, float h) {
    return g.just_clicked && rect_contains(x, y, w, h, g.mx, g.my);
}
bool ui_release(float x, float y, float w, float h) {
    return g.just_released && rect_contains(x, y, w, h, g.mx, g.my);
}
bool ui_held(float x, float y, float w, float h) {
    return g.is_down && rect_contains(x, y, w, h, g.mx, g.my);
}

/* ── ボタン ─────────────────────────────────────────────*/
int ui_button(int id, float x, float y, float w, float h) {
    UIWidget* wid = widget_get(id);
    bool over  = rect_contains(x, y, w, h, g.mx, g.my);
    bool click = over && g.just_clicked;
    if (click) return 3;          /* クリック完了 */
    if (over && g.is_down) return 2; /* 押下中 */
    if (over) return 1;           /* ホバー */
    return 0;                     /* 通常 */
}

/* ── チェックボックス ────────────────────────────────────*/
bool ui_checkbox(int id, float x, float y, float w, float h, bool initial_val) {
    UIWidget* wid = widget_get(id);
    if (!wid) return initial_val;
    /* 初回:  initial_val で初期化 */
    if (!wid->checked && initial_val) wid->checked = true; /* 注意: 既存 false にはセットしない */

    if (rect_contains(x, y, w, h, g.mx, g.my) && g.just_clicked)
        wid->checked = !wid->checked;
    return wid->checked;
}

/* ── スライダー (水平) ────────────────────────────────────*/
float ui_slider(int id, float x, float y, float w, float h, float norm_val) {
    UIWidget* wid = widget_get(id);
    if (!wid) return norm_val;
    if (rect_contains(x, y, w, h, g.mx, g.my) && g.is_down) {
        float t = (g.mx - x) / w;
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
        wid->norm_val = t;
    }
    return wid->norm_val;
}

/* ── スクロール ──────────────────────────────────────────*/
float ui_scroll(int id, float x, float y, float w, float view_h,
                float content_h, float wheel_dy) {
    UIWidget* wid = widget_get(id);
    if (!wid) return 0.0f;
    if (content_h <= view_h) { wid->scroll = 0.0f; return 0.0f; }

    if (rect_contains(x, y, w, view_h, g.mx, g.my) && wheel_dy != 0.0f) {
        float max_scroll = content_h - view_h;
        wid->scroll += wheel_dy * 20.0f;
        if (wid->scroll < 0.0f) wid->scroll = 0.0f;
        if (wid->scroll > max_scroll) wid->scroll = max_scroll;
    }
    /* スクロールバードラッグ */
    float bar_h = view_h * (view_h / content_h);
    float bar_y = y + wid->scroll / (content_h - view_h) * (view_h - bar_h);
    float bar_x = x + w - 12;
    if (rect_contains(bar_x, bar_y, 12, bar_h, g.mx, g.my) && g.is_down) {
        float t = (g.my - y) / view_h;
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
        wid->scroll = t * (content_h - view_h);
    }
    return wid->scroll;
}

/* ── テキスト入力 ────────────────────────────────────────*/
const char* ui_text_field(int id, const char* append, int backspace_count,
                           int max_len) {
    UITextField* f = field_get(id);
    if (!f) return "";
    if (backspace_count > 0) {
        int len = (int)strlen(f->buf);
        int del = backspace_count < len ? backspace_count : len;
        f->buf[len - del] = '\0';
    }
    if (append && *append) {
        int cur = (int)strlen(f->buf);
        int add = (int)strlen(append);
        if (max_len <= 0) max_len = UI_TEXTFIELD_LEN - 1;
        if (cur + add > max_len) add = max_len - cur;
        if (add > 0) {
            strncat(f->buf, append, add);
            f->buf[UI_TEXTFIELD_LEN - 1] = '\0';
        }
    }
    return f->buf;
}
void ui_text_field_clear(int id) {
    UITextField* f = field_get(id);
    if (f) f->buf[0] = '\0';
}

/* ── レイアウト ──────────────────────────────────────────*/
void ui_layout_begin(float ox, float oy, float col_w, float gap) {
    g.layout_x   = ox;
    g.layout_y   = oy;
    g.layout_col_w = col_w;
    g.layout_gap   = gap;
}
void ui_layout_next_row(float h) {
    g.layout_y += h + g.layout_gap;
}
float ui_layout_x(void) { return g.layout_x; }
float ui_layout_y(void) { return g.layout_y; }

/* ── カラーヘルパー ─────────────────────────────────────*/
void ui_button_color(int state,
                     float* r, float* g2, float* b, float* a) {
    *a = 1.0f;
    switch (state) {
    case 3: /* クリック */ *r=0.4f; *g2=0.7f; *b=1.0f; break;
    case 2: /* 押下 */    *r=0.2f; *g2=0.5f; *b=0.9f; break;
    case 1: /* ホバー */  *r=0.7f; *g2=0.7f; *b=0.9f; break;
    default:              *r=0.5f; *g2=0.5f; *b=0.7f; break;
    }
}
void ui_hp_color(float hp_ratio, float* r, float* g, float* b) {
    if (hp_ratio > 0.5f) {
        /* 緑→黄 */
        *r = (1.0f - hp_ratio) * 2.0f;
        *g = 1.0f;
        *b = 0.0f;
    } else {
        /* 黄→赤 */
        *r = 1.0f;
        *g = hp_ratio * 2.0f;
        *b = 0.0f;
    }
}
