# hajimu_engine_ui

> **はじむ言語**向け IMGUI スタイル UI 状態管理プラグイン

描画は行わず **UI 状態 (ホバー/クリック/値)** だけを管理します。  
実際の描画は `hajimu_engine_render` の矩形・文字描画関数で行うため、  
自分でテーマを自由にカスタマイズできます。

---

## 機能

| 関数 | 説明 |
|------|------|
| `UI初期化()` | 内部状態をリセット |
| `UI更新(mx,my,押下中,クリック,離した)` | 毎フレーム呼ぶ |
| `UIホバー(x,y,w,h)` | マウスが矩形内か |
| `UIクリック(x,y,w,h)` | クリック瞬間か |
| `UI離した(x,y,w,h)` | リリース瞬間か |
| `UI押下中(x,y,w,h)` | 押下継続中か |
| `UIボタン(id,x,y,w,h)` | 0=通常 1=ホバー 2=押下 3=クリック |
| `UIチェックボックス(id,x,y,w,h,初期値)` | トグル状態 (真/偽) |
| `UIスライダー(id,x,y,w,h,値)` | 0.0〜1.0 の正規化値 |
| `UIスクロール(id,x,y,w,表示高,コンテンツ高,ホイール)` | スクロール量 |
| `UIテキスト入力(id,追加文字,削除数,最大長)` | 入力文字列 |
| `UIテキストクリア(id)` | テキストフィールドをクリア |
| `UIレイアウト開始(x,y,幅,間隔)` | 縦積みレイアウト初期化 |
| `UI次行(高さ)` | カーソルを次行へ |
| `UIカーソルX()` / `UIカーソルY()` | 現在レイアウト位置 |
| `UIボタン色(状態)` | "r,g,b,a" CSV 文字列 |
| `UIHPバー色(比率)` | "r,g,b" CSV 文字列 (緑→黄→赤) |

## ビルド

```bash
git clone https://github.com/ReoShiozawa/hajimu_engine_ui.git
cd hajimu_engine_ui

make          # → build/engine_ui.hjp
make install  # → ~/.hajimu/plugins/engine_ui/
```

> `hajimu_engine_render` も必要です。

## サンプル

- [examples/hello_ui.jp](examples/hello_ui.jp) — ボタン・チェックボックス・スライダー・スクロール・レイアウトデモ

---

## ライセンス

MIT License — Copyright (c) 2026 Reo Shiozawa
