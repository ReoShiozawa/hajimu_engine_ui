/**
 * include/eng_ui.h — はじむ IMGUI スタイル UI システム 公開 API
 *
 * 即時モード (Immediate Mode) GUI:
 * - ウィジェットの状態を内部に保持し、毎フレーム判定する
 * - 描画は行わない (jp スクリプトが engine_render で描画する)
 * - ウィジェット ID は任意の正整数 (同じ ID は同じ状態を共有)
 *
 * Copyright (c) 2026 Reo Shiozawa — MIT License
 */
#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ── UI コンテキスト (シングルトン) ─────────────────────*/

/** UI システム初期化。アプリ起動時に一度だけ呼ぶ。 */
void ui_init(void);

/**
 * フレームごとに呼ぶ更新関数。
 * mx/my = マウス座標, is_down = マウスボタン押下中,
 * just_clicked = 押した瞬間 (pressed), just_released = 離した瞬間。
 */
void ui_update(float mx, float my, bool is_down,
               bool just_clicked, bool just_released);

/* ── ヒットテスト ──────────────────────────────────────*/

/** マウスが矩形上にあるかどうか (ホバー)。 */
bool ui_hover(float x, float y, float w, float h);

/** 矩形上でマウスボタンが押されたかどうか (クリック瞬間)。 */
bool ui_click(float x, float y, float w, float h);

/** 矩形上でマウスボタンが離されたかどうか。 */
bool ui_release(float x, float y, float w, float h);

/** 矩形上でマウスボタンが押し続けられているかどうか。 */
bool ui_held(float x, float y, float w, float h);

/* ── ウィジェット状態 ───────────────────────────────────*/

/** ボタン判定。戻り値: 0=通常,1=ホバー,2=押下中,3=クリック完了 */
int ui_button(int id, float x, float y, float w, float h);

/**
 * チェックボックス判定。
 * 初回呼び出し時の initial_val が初期状態。
 * 戻り値: 現在の checked 状態 (true/false)。
 */
bool ui_checkbox(int id, float x, float y, float w, float h, bool initial_val);

/**
 * スライダー判定。
 * 戻り値: 0.0〜1.0 の正規化値 (min/max への変換は jp 側で行う)。
 * mx/my は現在マウス座標 (ui_update で自動設定、不要ならそのまま呼ぶ)。
 */
float ui_slider(int id, float x, float y, float w, float h, float norm_val);

/**
 * スクロールビュー。
 * content_h = コンテンツ全体の高さ、view_h = 表示領域の高さ。
 * 戻り値: 正規化 (0〜1) スクロール位置。
 * ホイール入力は wheel_dy (正=下, 負=上) で渡す。
 */
float ui_scroll(int id, float x, float y, float w, float view_h,
                float content_h, float wheel_dy);

/* ── テキスト入力 ───────────────────────────────────────*/

/**
 * テキストフィールドへのキー入力を付加して返す。
 * 注意: 入力バッファは C の静的領域。複数フィールド使用時は id で識別。
 * backspace_count = 削除する文字数 (0=なし)。
 * append = 追加する文字列 (NULL or "" で変更なし)。
 * 戻り値: 現在のバッファ文字列。
 */
const char* ui_text_field(int id, const char* append, int backspace_count,
                           int max_len);

/** テキストフィールドのバッファをクリア。 */
void ui_text_field_clear(int id);

/* ── レイアウトヘルパー ─────────────────────────────────*/

/** レイアウトカーソル初期化。(origin_x, origin_y) からスタート。 */
void ui_layout_begin(float origin_x, float origin_y, float col_w, float gap);

/** カーソルを次の行へ進める (高さ h + gap)。 */
void ui_layout_next_row(float h);

/** 現在のカーソル X。 */
float ui_layout_x(void);

/** 現在のカーソル Y。 */
float ui_layout_y(void);

/* ── カラーヘルパー ─────────────────────────────────────*/

/**
 * ボタン状態 (ui_button の戻り値) に対応する RGBA を返す。
 * 出_r/g/b/a に書き込む。
 */
void ui_button_color(int state,
                     float* out_r, float* out_g, float* out_b, float* out_a);

/** HP バーの色。 hp_ratio = 0.0〜1.0。 */
void ui_hp_color(float hp_ratio,
                 float* out_r, float* out_g, float* out_b);

/* ── 追加ウィジェット ───────────────────────────────────*/

/**
 * プログレスバー。value を 0.0〜1.0 にクランプして返す。
 * 描画は jp 側が行う (返値を使って矩形を塗る)。
 */
float ui_progress(int id, float x, float y, float w, float h, float value);

/**
 * ラジオボタン。同じ group_id を持つ中で1つだけ選択可能。
 * 戻り値: このボタンが選択されているかどうか。
 * initial_selected: 初回呼び出し時の初期選択状態。
 */
bool  ui_radio(int id, int group_id, float x, float y, float w, float h,
               bool initial_selected);

/**
 * トグルボタン (ON/OFF)。クリックで反転。
 * 戻り値: 現在の ON/OFF 状態。
 * initial_val: 初回呼び出し時の初期状態。
 */
bool  ui_toggle(int id, float x, float y, float w, float h, bool initial_val);

/**
 * ドロップダウン。戻り値: 現在選択中インデックス。
 * items: 選択肢文字列配列, count: 要素数, initial: 初期インデックス。
 * ヘッダー部をクリックで開閉、選択肢クリックで選択&閉じる。
 */
int  ui_dropdown(int id, float x, float y, float w, float h,
                 const char** items, int count, int initial);

/** ドロップダウンが現在開いているか。 */
bool ui_dropdown_open(int id);

/**
 * スピナー (数値増減ウィジェット)。戻り値: 現在値。
 * val: 初期値, min/max: 範囲, step: 増減量。
 * 右端に +/- ボタン領域を配置。
 */
float ui_spinner(int id, float x, float y, float w, float h,
                 float val, float min, float max, float step);

/**
 * タブ。同じ group_id のタブから1つだけアクティブにできる。
 * 戻り値: このタブが選択されているか。
 * initial: true なら初回にこのタブを選択にする。
 */
bool ui_tab(int id, int group_id, float x, float y, float w, float h,
            bool initial);

/** group_id で現在選択されているタブIDを返す (0=未選択)。 */
int  ui_tab_selected(int group_id);

#ifdef __cplusplus
}
#endif
