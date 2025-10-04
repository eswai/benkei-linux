#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <time.h>
#include "common.h"
#include "functions.h"

#include "nglist.h"
#include "nglistarray.h"
// #include "naginata_func.h"
#include "naginata_v16.h"

extern int64_t timestamp;

#define KEY_NONE 0

// 薙刀式
static NGListArray nginput;
static int pressed_keys = 0UL; // 押しているキーのビットをたてる
static int8_t n_pressed_keys = 0;   // 押しているキーの数

static const int ng_key[] = {
    [KEY_A - KEY_Q] = B_A,     [KEY_B - KEY_Q] = B_B,         [KEY_C - KEY_Q] = B_C,         [KEY_D - KEY_Q] = B_D,
    [KEY_E - KEY_Q] = B_E,     [KEY_F - KEY_Q] = B_F,         [KEY_G - KEY_Q] = B_G,         [KEY_H - KEY_Q] = B_H,
    [KEY_I - KEY_Q] = B_I,     [KEY_J - KEY_Q] = B_J,         [KEY_K - KEY_Q] = B_K,         [KEY_L - KEY_Q] = B_L,
    [KEY_M - KEY_Q] = B_M,     [KEY_N - KEY_Q] = B_N,         [KEY_O - KEY_Q] = B_O,         [KEY_P - KEY_Q] = B_P,
    [KEY_Q - KEY_Q] = B_Q,     [KEY_R - KEY_Q] = B_R,         [KEY_S - KEY_Q] = B_S,         [KEY_T - KEY_Q] = B_T,
    [KEY_U - KEY_Q] = B_U,     [KEY_V - KEY_Q] = B_V,         [KEY_W - KEY_Q] = B_W,         [KEY_X - KEY_Q] = B_X,
    [KEY_Y - KEY_Q] = B_Y,     [KEY_Z - KEY_Q] = B_Z,         [KEY_SEMICOLON - KEY_Q] = B_SEMI,   [KEY_COMMA - KEY_Q] = B_COMMA,
    [KEY_DOT - KEY_Q] = B_DOT, [KEY_SLASH - KEY_Q] = B_SLASH, [KEY_SPACE - KEY_Q] = B_SPACE, [KEY_ENTER - KEY_Q] = B_SPACE,
};

// カナ変換テーブル
typedef struct {
    int shift;
    int douji;
    int kana[6];
    void (*func)(void);
} naginata_kanamap;

static naginata_kanamap ngdickana[] = {
    // 清音
    {.shift = NONE    , .douji = B_J            , .kana = {KEY_A, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE}, .func = nofunc }, // あ
    {.shift = NONE    , .douji = B_K            , .kana = {KEY_I, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE}, .func = nofunc }, // い
    {.shift = NONE    , .douji = B_L            , .kana = {KEY_U, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE}, .func = nofunc }, // う
    {.shift = B_SPACE , .douji = B_O            , .kana = {KEY_E, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE}, .func = nofunc }, // え
    {.shift = B_SPACE , .douji = B_N            , .kana = {KEY_O, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE}, .func = nofunc }, // お
    {.shift = NONE    , .douji = B_F            , .kana = {KEY_K, KEY_A, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // か
    {.shift = NONE    , .douji = B_W            , .kana = {KEY_K, KEY_I, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // き
    {.shift = NONE    , .douji = B_H            , .kana = {KEY_K, KEY_U, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // く
    {.shift = NONE    , .douji = B_S            , .kana = {KEY_K, KEY_E, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // け
    {.shift = NONE    , .douji = B_V            , .kana = {KEY_K, KEY_O, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // こ
    {.shift = B_SPACE , .douji = B_U            , .kana = {KEY_S, KEY_A, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // さ
    {.shift = NONE    , .douji = B_R            , .kana = {KEY_S, KEY_I, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // し
    {.shift = NONE    , .douji = B_O            , .kana = {KEY_S, KEY_U, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // す
    {.shift = B_SPACE , .douji = B_A            , .kana = {KEY_S, KEY_E, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // せ
    {.shift = NONE    , .douji = B_B            , .kana = {KEY_S, KEY_O, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // そ
    {.shift = NONE    , .douji = B_N            , .kana = {KEY_T, KEY_A, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // た
    {.shift = B_SPACE , .douji = B_G            , .kana = {KEY_T, KEY_I, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ち
    {.shift = B_SPACE , .douji = B_L            , .kana = {KEY_T, KEY_U, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // つ
    {.shift = NONE    , .douji = B_E            , .kana = {KEY_T, KEY_E, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // て
    {.shift = NONE    , .douji = B_D            , .kana = {KEY_T, KEY_O, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // と
    {.shift = NONE    , .douji = B_M            , .kana = {KEY_N, KEY_A, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // な
    {.shift = B_SPACE , .douji = B_D            , .kana = {KEY_N, KEY_I, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // に
    {.shift = B_SPACE , .douji = B_W            , .kana = {KEY_N, KEY_U, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ぬ
    {.shift = B_SPACE , .douji = B_COMMA        , .kana = {KEY_N, KEY_E, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ね
    {.shift = B_SPACE , .douji = B_J            , .kana = {KEY_N, KEY_O, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // の
    {.shift = NONE    , .douji = B_C            , .kana = {KEY_H, KEY_A, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // は
    {.shift = NONE    , .douji = B_X            , .kana = {KEY_H, KEY_I, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ひ
    {.shift = B_SPACE , .douji = B_X            , .kana = {KEY_H, KEY_I, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ひ
    {.shift = B_SPACE , .douji = B_SEMI         , .kana = {KEY_H, KEY_U, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ふ
    {.shift = NONE    , .douji = B_P            , .kana = {KEY_H, KEY_E, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // へ
    {.shift = NONE    , .douji = B_Z            , .kana = {KEY_H, KEY_O, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ほ
    {.shift = B_SPACE , .douji = B_Z            , .kana = {KEY_H, KEY_O, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ほ
    {.shift = B_SPACE , .douji = B_F            , .kana = {KEY_M, KEY_A, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ま
    {.shift = B_SPACE , .douji = B_S            , .kana = {KEY_M, KEY_I, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // み
    {.shift = B_SPACE , .douji = B_B            , .kana = {KEY_M, KEY_U, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // む
    {.shift = B_SPACE , .douji = B_R            , .kana = {KEY_M, KEY_E, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // め
    {.shift = B_SPACE , .douji = B_K            , .kana = {KEY_M, KEY_O, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // も
    {.shift = B_SPACE , .douji = B_H            , .kana = {KEY_Y, KEY_A, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // や
    {.shift = B_SPACE , .douji = B_P            , .kana = {KEY_Y, KEY_U, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ゆ
    {.shift = B_SPACE , .douji = B_I            , .kana = {KEY_Y, KEY_O, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // よ
    {.shift = NONE    , .douji = B_DOT          , .kana = {KEY_R, KEY_A, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ら
    {.shift = B_SPACE , .douji = B_E            , .kana = {KEY_R, KEY_I, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // り
    {.shift = NONE    , .douji = B_I            , .kana = {KEY_R, KEY_U, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // る
    {.shift = NONE    , .douji = B_SLASH        , .kana = {KEY_R, KEY_E, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // れ
    {.shift = B_SPACE , .douji = B_SLASH        , .kana = {KEY_R, KEY_E, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // れ
    {.shift = NONE    , .douji = B_A            , .kana = {KEY_R, KEY_O, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ろ
    {.shift = B_SPACE , .douji = B_DOT          , .kana = {KEY_W, KEY_A, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // わ
    {.shift = B_SPACE , .douji = B_C            , .kana = {KEY_W, KEY_O, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // を
    {.shift = NONE    , .douji = B_COMMA        , .kana = {KEY_N, KEY_N, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ん
    {.shift = NONE    , .douji = B_SEMI         , .kana = {KEY_MINUS, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE}, .func = nofunc }, // ー

    // 濁音
    {.shift = NONE    , .douji = B_J|B_F        , .kana = {KEY_G, KEY_A, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // が
    {.shift = NONE    , .douji = B_J|B_W        , .kana = {KEY_G, KEY_I, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ぎ
    {.shift = 0UL     , .douji = B_F|B_H        , .kana = {KEY_G, KEY_U, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ぐ
    {.shift = NONE    , .douji = B_J|B_S        , .kana = {KEY_G, KEY_E, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // げ
    {.shift = NONE    , .douji = B_J|B_V        , .kana = {KEY_G, KEY_O, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ご
    {.shift = 0UL     , .douji = B_F|B_U        , .kana = {KEY_Z, KEY_A, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ざ
    {.shift = NONE    , .douji = B_J|B_R        , .kana = {KEY_Z, KEY_I, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // じ
    {.shift = 0UL     , .douji = B_F|B_O        , .kana = {KEY_Z, KEY_U, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ず
    {.shift = NONE    , .douji = B_J|B_A        , .kana = {KEY_Z, KEY_E, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ぜ
    {.shift = NONE    , .douji = B_J|B_B        , .kana = {KEY_Z, KEY_O, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ぞ
    {.shift = 0UL     , .douji = B_F|B_N        , .kana = {KEY_D, KEY_A, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // だ
    {.shift = NONE    , .douji = B_J|B_G        , .kana = {KEY_D, KEY_I, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ぢ
    {.shift = 0UL     , .douji = B_F|B_L        , .kana = {KEY_D, KEY_U, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // づ
    {.shift = NONE    , .douji = B_J|B_E        , .kana = {KEY_D, KEY_E, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // で
    {.shift = NONE    , .douji = B_J|B_D        , .kana = {KEY_D, KEY_O, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ど
    {.shift = NONE    , .douji = B_J|B_C        , .kana = {KEY_B, KEY_A, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ば
    {.shift = NONE    , .douji = B_J|B_X        , .kana = {KEY_B, KEY_I, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // び
    {.shift = 0UL     , .douji = B_F|B_SEMI     , .kana = {KEY_B, KEY_U, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ぶ
    {.shift = 0UL     , .douji = B_F|B_P        , .kana = {KEY_B, KEY_E, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // べ
    {.shift = NONE    , .douji = B_J|B_Z        , .kana = {KEY_B, KEY_O, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ぼ
    {.shift = 0UL     , .douji = B_F|B_L|B_SEMI , .kana = {KEY_V, KEY_U, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ゔ

    // 半濁音
    {.shift = NONE    , .douji = B_M|B_C        , .kana = {KEY_P, KEY_A, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ぱ
    {.shift = NONE    , .douji = B_M|B_X        , .kana = {KEY_P, KEY_I, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ぴ
    {.shift = NONE    , .douji = B_V|B_SEMI     , .kana = {KEY_P, KEY_U, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ぷ
    {.shift = NONE    , .douji = B_V|B_P        , .kana = {KEY_P, KEY_E, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ぺ
    {.shift = NONE    , .douji = B_M|B_Z        , .kana = {KEY_P, KEY_O, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ぽ

    // 小書き
    {.shift = NONE    , .douji = B_Q|B_H        , .kana = {KEY_X, KEY_Y, KEY_A, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // ゃ
    {.shift = NONE    , .douji = B_Q|B_P        , .kana = {KEY_X, KEY_Y, KEY_U, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // ゅ
    {.shift = NONE    , .douji = B_Q|B_I        , .kana = {KEY_X, KEY_Y, KEY_O, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // ょ
    {.shift = NONE    , .douji = B_Q|B_J        , .kana = {KEY_X, KEY_A, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ぁ
    {.shift = NONE    , .douji = B_Q|B_K        , .kana = {KEY_X, KEY_I, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ぃ
    {.shift = NONE    , .douji = B_Q|B_L        , .kana = {KEY_X, KEY_U, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ぅ
    {.shift = NONE    , .douji = B_Q|B_O        , .kana = {KEY_X, KEY_E, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ぇ
    {.shift = NONE    , .douji = B_Q|B_N        , .kana = {KEY_X, KEY_O, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ぉ
    {.shift = NONE    , .douji = B_Q|B_DOT      , .kana = {KEY_X, KEY_W, KEY_A, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // ゎ
    {.shift = NONE    , .douji = B_G            , .kana = {KEY_X, KEY_T, KEY_U, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // っ
    {.shift = NONE    , .douji = B_Q|B_S        , .kana = {KEY_X, KEY_K, KEY_E, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // ヶ
    {.shift = NONE    , .douji = B_Q|B_F        , .kana = {KEY_X, KEY_K, KEY_A, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // ヵ

    // 清音拗音 濁音拗音 半濁拗音
    {.shift = NONE    , .douji = B_R|B_H        , .kana = {KEY_S, KEY_Y, KEY_A, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // しゃ
    {.shift = NONE    , .douji = B_R|B_P        , .kana = {KEY_S, KEY_Y, KEY_U, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // しゅ
    {.shift = NONE    , .douji = B_R|B_I        , .kana = {KEY_S, KEY_Y, KEY_O, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // しょ
    {.shift = NONE    , .douji = B_J|B_R|B_H    , .kana = {KEY_Z, KEY_Y, KEY_A, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // じゃ
    {.shift = NONE    , .douji = B_J|B_R|B_P    , .kana = {KEY_Z, KEY_Y, KEY_U, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // じゅ
    {.shift = NONE    , .douji = B_J|B_R|B_I    , .kana = {KEY_Z, KEY_Y, KEY_O, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // じょ
    {.shift = NONE    , .douji = B_W|B_H        , .kana = {KEY_K, KEY_Y, KEY_A, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // きゃ
    {.shift = NONE    , .douji = B_W|B_P        , .kana = {KEY_K, KEY_Y, KEY_U, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // きゅ
    {.shift = NONE    , .douji = B_W|B_I        , .kana = {KEY_K, KEY_Y, KEY_O, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // きょ
    {.shift = NONE    , .douji = B_J|B_W|B_H    , .kana = {KEY_G, KEY_Y, KEY_A, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // ぎゃ
    {.shift = NONE    , .douji = B_J|B_W|B_P    , .kana = {KEY_G, KEY_Y, KEY_U, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // ぎゅ
    {.shift = NONE    , .douji = B_J|B_W|B_I    , .kana = {KEY_G, KEY_Y, KEY_O, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // ぎょ
    {.shift = NONE    , .douji = B_G|B_H        , .kana = {KEY_T, KEY_Y, KEY_A, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // ちゃ
    {.shift = NONE    , .douji = B_G|B_P        , .kana = {KEY_T, KEY_Y, KEY_U, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // ちゅ
    {.shift = NONE    , .douji = B_G|B_I        , .kana = {KEY_T, KEY_Y, KEY_O, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // ちょ
    {.shift = NONE    , .douji = B_J|B_G|B_H    , .kana = {KEY_D, KEY_Y, KEY_A, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // ぢゃ
    {.shift = NONE    , .douji = B_J|B_G|B_P    , .kana = {KEY_D, KEY_Y, KEY_U, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // ぢゅ
    {.shift = NONE    , .douji = B_J|B_G|B_I    , .kana = {KEY_D, KEY_Y, KEY_O, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // ぢょ
    {.shift = NONE    , .douji = B_D|B_H        , .kana = {KEY_N, KEY_Y, KEY_A, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // にゃ
    {.shift = NONE    , .douji = B_D|B_P        , .kana = {KEY_N, KEY_Y, KEY_U, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // にゅ
    {.shift = NONE    , .douji = B_D|B_I        , .kana = {KEY_N, KEY_Y, KEY_O, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // にょ
    {.shift = NONE    , .douji = B_X|B_H        , .kana = {KEY_H, KEY_Y, KEY_A, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // ひゃ
    {.shift = NONE    , .douji = B_X|B_P        , .kana = {KEY_H, KEY_Y, KEY_U, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // ひゅ
    {.shift = NONE    , .douji = B_X|B_I        , .kana = {KEY_H, KEY_Y, KEY_O, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // ひょ
    {.shift = NONE    , .douji = B_J|B_X|B_H    , .kana = {KEY_B, KEY_Y, KEY_A, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // びゃ
    {.shift = NONE    , .douji = B_J|B_X|B_P    , .kana = {KEY_B, KEY_Y, KEY_U, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // びゅ
    {.shift = NONE    , .douji = B_J|B_X|B_I    , .kana = {KEY_B, KEY_Y, KEY_O, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // びょ
    {.shift = NONE    , .douji = B_M|B_X|B_H    , .kana = {KEY_P, KEY_Y, KEY_A, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // ぴゃ
    {.shift = NONE    , .douji = B_M|B_X|B_P    , .kana = {KEY_P, KEY_Y, KEY_U, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // ぴゅ
    {.shift = NONE    , .douji = B_M|B_X|B_I    , .kana = {KEY_P, KEY_Y, KEY_O, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // ぴょ
    {.shift = NONE    , .douji = B_S|B_H        , .kana = {KEY_M, KEY_Y, KEY_A, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // みゃ
    {.shift = NONE    , .douji = B_S|B_P        , .kana = {KEY_M, KEY_Y, KEY_U, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // みゅ
    {.shift = NONE    , .douji = B_S|B_I        , .kana = {KEY_M, KEY_Y, KEY_O, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // みょ
    {.shift = NONE    , .douji = B_E|B_H        , .kana = {KEY_R, KEY_Y, KEY_A, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // りゃ
    {.shift = NONE    , .douji = B_E|B_P        , .kana = {KEY_R, KEY_Y, KEY_U, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // りゅ
    {.shift = NONE    , .douji = B_E|B_I        , .kana = {KEY_R, KEY_Y, KEY_O, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // りょ

    // 清音外来音 濁音外来音
    {.shift = NONE    , .douji = B_M|B_E|B_K    , .kana = {KEY_T, KEY_H, KEY_I, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // てぃ
    {.shift = NONE    , .douji = B_M|B_E|B_P    , .kana = {KEY_T, KEY_E, KEY_X, KEY_Y, KEY_U, KEY_NONE            }, .func = nofunc }, // てゅ
    {.shift = NONE    , .douji = B_J|B_E|B_K    , .kana = {KEY_D, KEY_H, KEY_I, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // でぃ
    {.shift = NONE    , .douji = B_J|B_E|B_P    , .kana = {KEY_D, KEY_H, KEY_U, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // でゅ
    {.shift = NONE    , .douji = B_M|B_D|B_L    , .kana = {KEY_T, KEY_O, KEY_X, KEY_U, KEY_NONE, KEY_NONE         }, .func = nofunc }, // とぅ
    {.shift = NONE    , .douji = B_J|B_D|B_L    , .kana = {KEY_D, KEY_O, KEY_X, KEY_U, KEY_NONE, KEY_NONE         }, .func = nofunc }, // どぅ
    {.shift = NONE    , .douji = B_M|B_R|B_O    , .kana = {KEY_S, KEY_Y, KEY_E, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // しぇ
    {.shift = NONE    , .douji = B_M|B_G|B_O    , .kana = {KEY_T, KEY_Y, KEY_E, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // ちぇ
    {.shift = NONE    , .douji = B_J|B_R|B_O    , .kana = {KEY_Z, KEY_Y, KEY_E, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // じぇ
    {.shift = NONE    , .douji = B_J|B_G|B_O    , .kana = {KEY_D, KEY_Y, KEY_E, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // ぢぇ
    {.shift = NONE    , .douji = B_V|B_SEMI|B_J , .kana = {KEY_F, KEY_A, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ふぁ
    {.shift = NONE    , .douji = B_V|B_SEMI|B_K , .kana = {KEY_F, KEY_I, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ふぃ
    {.shift = NONE    , .douji = B_V|B_SEMI|B_O , .kana = {KEY_F, KEY_E, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ふぇ
    {.shift = NONE    , .douji = B_V|B_SEMI|B_N , .kana = {KEY_F, KEY_O, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ふぉ
    {.shift = NONE    , .douji = B_V|B_SEMI|B_P , .kana = {KEY_F, KEY_Y, KEY_U, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // ふゅ
    {.shift = NONE    , .douji = B_V|B_K|B_O    , .kana = {KEY_I, KEY_X, KEY_E, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // いぇ
    {.shift = NONE    , .douji = B_V|B_L|B_K    , .kana = {KEY_W, KEY_I, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // うぃ
    {.shift = NONE    , .douji = B_V|B_L|B_O    , .kana = {KEY_W, KEY_E, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // うぇ
    {.shift = NONE    , .douji = B_V|B_L|B_N    , .kana = {KEY_U, KEY_X, KEY_O, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // うぉ
    {.shift = NONE    , .douji = B_F|B_L|B_J    , .kana = {KEY_V, KEY_A, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ゔぁ
    {.shift = NONE    , .douji = B_F|B_L|B_K    , .kana = {KEY_V, KEY_I, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ゔぃ
    {.shift = NONE    , .douji = B_F|B_L|B_O    , .kana = {KEY_V, KEY_E, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ゔぇ
    {.shift = NONE    , .douji = B_F|B_L|B_N    , .kana = {KEY_V, KEY_O, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc }, // ゔぉ
    {.shift = NONE    , .douji = B_F|B_L|B_P    , .kana = {KEY_V, KEY_U, KEY_X, KEY_Y, KEY_U, KEY_NONE            }, .func = nofunc }, // ゔゅ
    {.shift = NONE    , .douji = B_V|B_H|B_J    , .kana = {KEY_K, KEY_U, KEY_X, KEY_A, KEY_NONE, KEY_NONE         }, .func = nofunc }, // くぁ
    {.shift = NONE    , .douji = B_V|B_H|B_K    , .kana = {KEY_K, KEY_U, KEY_X, KEY_I, KEY_NONE, KEY_NONE         }, .func = nofunc }, // くぃ
    {.shift = NONE    , .douji = B_V|B_H|B_O    , .kana = {KEY_K, KEY_U, KEY_X, KEY_E, KEY_NONE, KEY_NONE         }, .func = nofunc }, // くぇ
    {.shift = NONE    , .douji = B_V|B_H|B_N    , .kana = {KEY_K, KEY_U, KEY_X, KEY_O, KEY_NONE, KEY_NONE         }, .func = nofunc }, // くぉ
    {.shift = NONE    , .douji = B_V|B_H|B_DOT  , .kana = {KEY_K, KEY_U, KEY_X, KEY_W, KEY_A, KEY_NONE            }, .func = nofunc }, // くゎ
    {.shift = NONE    , .douji = B_F|B_H|B_J    , .kana = {KEY_G, KEY_U, KEY_X, KEY_A, KEY_NONE, KEY_NONE         }, .func = nofunc }, // ぐぁ
    {.shift = NONE    , .douji = B_F|B_H|B_K    , .kana = {KEY_G, KEY_U, KEY_X, KEY_I, KEY_NONE, KEY_NONE         }, .func = nofunc }, // ぐぃ
    {.shift = NONE    , .douji = B_F|B_H|B_O    , .kana = {KEY_G, KEY_U, KEY_X, KEY_E, KEY_NONE, KEY_NONE         }, .func = nofunc }, // ぐぇ
    {.shift = NONE    , .douji = B_F|B_H|B_N    , .kana = {KEY_G, KEY_U, KEY_X, KEY_O, KEY_NONE, KEY_NONE         }, .func = nofunc }, // ぐぉ
    {.shift = NONE    , .douji = B_F|B_H|B_DOT  , .kana = {KEY_G, KEY_U, KEY_X, KEY_W, KEY_A, KEY_NONE            }, .func = nofunc }, // ぐゎ
    {.shift = NONE    , .douji = B_V|B_L|B_J    , .kana = {KEY_T, KEY_S, KEY_A, KEY_NONE, KEY_NONE, KEY_NONE      }, .func = nofunc }, // つぁ
    
    // 追加
    {.shift = NONE    , .douji = B_SPACE        , .kana = {KEY_SPACE, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE  }, .func = nofunc},
    {.shift = B_SPACE , .douji = B_V            , .kana = {KEY_COMMA, KEY_ENTER, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE }, .func = nofunc},
    {.shift = NONE    , .douji = B_Q            , .kana = {KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc},
    {.shift = B_SPACE , .douji = B_M            , .kana = {KEY_DOT, KEY_ENTER, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc},
    {.shift = NONE    , .douji = B_U            , .kana = {KEY_BACKSPACE, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE   }, .func = nofunc},

    {.shift = NONE    , .douji = B_V|B_M        , .kana = {KEY_ENTER, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE  }, .func = nofunc}, // enter
    // {.shift = B_SPACE, .douji = B_V|B_M, .kana = {ENTER, NONE, NONE, NONE, NONE, NONE}, .func = nofunc}, // enter+シフト(連続シフト)

    // {.shift = NONE    , .douji = B_T            , .kana = {NONE, NONE, NONE, NONE, NONE, NONE   }, .func = ng_T}, //
    // {.shift = NONE    , .douji = B_Y            , .kana = {NONE, NONE, NONE, NONE, NONE, NONE   }, .func = ng_Y}, //
    // {.shift = B_SPACE , .douji = B_T            , .kana = {NONE, NONE, NONE, NONE, NONE, NONE   }, .func = ng_ST}, //
    // {.shift = B_SPACE , .douji = B_Y            , .kana = {NONE, NONE, NONE, NONE, NONE, NONE   }, .func = ng_SY}, //
    // {.shift = NONE    , .douji = B_H|B_J        , .kana = {NONE, NONE, NONE, NONE, NONE, NONE   }, .func = naginata_on}, // 　かなオン
    // // {.shift = NONE, .douji = B_F | B_G, .kana = {NONE, NONE, NONE, NONE, NONE, NONE}, .func = naginata_off}, // 　かなオフ

    // // 編集モード
    // {.shift = B_J|B_K    , .douji = B_Q     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_JKQ    }, // ^{End}
    // {.shift = B_J|B_K    , .douji = B_W     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_JKW    }, // ／{改行}
    // {.shift = B_J|B_K    , .douji = B_E     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_JKE    }, // /*ディ*/
    // {.shift = B_J|B_K    , .douji = B_R     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_JKR    }, // ^s
    // {.shift = B_J|B_K    , .douji = B_T     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_JKT    }, // ・
    // {.shift = B_J|B_K    , .douji = B_A     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_JKA    }, // ……{改行}
    // {.shift = B_J|B_K    , .douji = B_S     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_JKS    }, // 『{改行}
    // {.shift = B_J|B_K    , .douji = B_D     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_JKD    }, // ？{改行}
    // {.shift = B_J|B_K    , .douji = B_F     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_JKF    }, // 「{改行}
    // {.shift = B_J|B_K    , .douji = B_G     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_JKG    }, // ({改行}
    // {.shift = B_J|B_K    , .douji = B_Z     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_JKZ    }, // ――{改行}
    // {.shift = B_J|B_K    , .douji = B_X     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_JKX    }, // 』{改行}
    // {.shift = B_J|B_K    , .douji = B_C     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_JKC    }, // ！{改行}
    // {.shift = B_J|B_K    , .douji = B_V     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_JKV    }, // 」{改行}
    // {.shift = B_J|B_K    , .douji = B_B     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_JKB    }, // ){改行}
    // {.shift = B_D|B_F    , .douji = B_Y     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_DFY    }, // {Home}
    // {.shift = B_D|B_F    , .douji = B_U     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_DFU    }, // +{End}{BS}
    // {.shift = B_D|B_F    , .douji = B_I     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_DFI    }, // {vk1Csc079}
    // {.shift = B_D|B_F    , .douji = B_O     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_DFO    }, // {Del}
    // {.shift = B_D|B_F    , .douji = B_P     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_DFP    }, // +{Esc 2}
    // {.shift = B_D|B_F    , .douji = B_H     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_DFH    }, // {Enter}{End}
    // {.shift = B_D|B_F    , .douji = B_J     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_DFJ    }, // {↑}
    // {.shift = B_D|B_F    , .douji = B_K     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_DFK    }, // +{↑}
    // {.shift = B_D|B_F    , .douji = B_L     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_DFL    }, // +{↑ 7}
    // {.shift = B_D|B_F    , .douji = B_SEMI  , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_DFSCLN }, // ^i
    // {.shift = B_D|B_F    , .douji = B_N     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_DFN    }, // {End}
    // {.shift = B_D|B_F    , .douji = B_M     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_DFM    }, // {↓}
    // {.shift = B_D|B_F    , .douji = B_COMMA , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_DFCOMM }, // +{↓}
    // {.shift = B_D|B_F    , .douji = B_DOT   , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_DFDOT  }, // +{↓ 7}
    // {.shift = B_D|B_F    , .douji = B_SLASH , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_DFSLSH }, // ^u
    // {.shift = B_M|B_COMMA, .douji = B_Q     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_MCQ    }, // ｜{改行}
    // {.shift = B_M|B_COMMA, .douji = B_W     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_MCW    }, // 　　　×　　　×　　　×{改行 2}
    // {.shift = B_M|B_COMMA, .douji = B_E     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_MCE    }, // {Home}{→}{End}{Del 2}{←}
    // {.shift = B_M|B_COMMA, .douji = B_R     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_MCR    }, // {Home}{改行}{Space 1}{←}
    // {.shift = B_M|B_COMMA, .douji = B_T     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_MCT    }, // 〇{改行}
    // {.shift = B_M|B_COMMA, .douji = B_A     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_MCA    }, // 《{改行}
    // {.shift = B_M|B_COMMA, .douji = B_S     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_MCS    }, // 【{改行}
    // {.shift = B_M|B_COMMA, .douji = B_D     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_MCD    }, // {Home}{→}{End}{Del 4}{←}
    // {.shift = B_M|B_COMMA, .douji = B_F     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_MCF    }, // {Home}{改行}{Space 3}{←}
    // {.shift = B_M|B_COMMA, .douji = B_G     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_MCG    }, // {Space 3}
    // {.shift = B_M|B_COMMA, .douji = B_Z     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_MCZ    }, // 》{改行}
    // {.shift = B_M|B_COMMA, .douji = B_X     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_MCX    }, // 】{改行}
    // {.shift = B_M|B_COMMA, .douji = B_C     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_MCC    }, // 」{改行}{改行}
    // {.shift = B_M|B_COMMA, .douji = B_V     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_MCV    }, // 」{改行}{改行}「{改行}
    // {.shift = B_M|B_COMMA, .douji = B_B     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_MCB    }, // 」{改行}{改行}{Space}
    // {.shift = B_C|B_V    , .douji = B_Y     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_CVY    }, // +{Home}
    // {.shift = B_C|B_V    , .douji = B_U     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_CVU    }, // ^x
    // {.shift = B_C|B_V    , .douji = B_I     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_CVI    }, // {vk1Csc079}
    // {.shift = B_C|B_V    , .douji = B_O     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_CVO    }, // ^v
    // {.shift = B_C|B_V    , .douji = B_P     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_CVP    }, // ^z
    // {.shift = B_C|B_V    , .douji = B_H     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_CVH    }, // ^c
    // {.shift = B_C|B_V    , .douji = B_J     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_CVJ    }, // {←}
    // {.shift = B_C|B_V    , .douji = B_K     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_CVK    }, // {→}
    // {.shift = B_C|B_V    , .douji = B_L     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_CVL    }, // {改行}{Space}+{Home}^x{BS}
    // {.shift = B_C|B_V    , .douji = B_SEMI  , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_CVSCLN }, // ^y
    // {.shift = B_C|B_V    , .douji = B_N     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_CVN    }, // +{End}
    // {.shift = B_C|B_V    , .douji = B_M     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_CVM    }, // +{←}
    // {.shift = B_C|B_V    , .douji = B_COMMA , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_CVCOMM }, // +{→}
    // {.shift = B_C|B_V    , .douji = B_DOT   , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_CVDOT  }, // +{← 7}
    // {.shift = B_C|B_V    , .douji = B_SLASH , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_CVSLSH }, // +{→ 7}

};

// Helper function for counting matches/candidates
static int count_kana_entries(NGList *keys, bool exact_match) {
  if (keys->size == 0) return 0;

  int count = 0;
  int keyset0 = 0UL, keyset1 = 0UL, keyset2 = 0UL;
  
  // keysetを配列にしたらバイナリサイズが増えた
  switch (keys->size) {
    case 1:
      keyset0 = ng_key[keys->elements[0] - KEY_Q];
      break;
    case 2:
      keyset0 = ng_key[keys->elements[0] - KEY_Q];
      keyset1 = ng_key[keys->elements[1] - KEY_Q];
      break;
    default:
      keyset0 = ng_key[keys->elements[0] - KEY_Q];
      keyset1 = ng_key[keys->elements[1] - KEY_Q];
      keyset2 = ng_key[keys->elements[2] - KEY_Q];
      break;
  }

  for (unsigned long int i = 0; i < sizeof ngdickana / sizeof ngdickana[0]; i++) {
    bool matches = false;

    switch (keys->size) {
      case 1:
        if (exact_match) {
          matches = (ngdickana[i].shift == keyset0) || 
                   (ngdickana[i].shift == 0UL && ngdickana[i].douji == keyset0);
        } else {
          matches = ((ngdickana[i].shift & keyset0) == keyset0) ||
                   (ngdickana[i].shift == 0UL && (ngdickana[i].douji & keyset0) == keyset0);
        }
        break;
      case 2:
        if (exact_match) {
          matches = (ngdickana[i].shift == (keyset0 | keyset1)) ||
                   (ngdickana[i].shift == keyset0 && ngdickana[i].douji == keyset1) ||
                   (ngdickana[i].shift == 0UL && ngdickana[i].douji == (keyset0 | keyset1));
        } else {
          matches = (ngdickana[i].shift == (keyset0 | keyset1)) ||
                   (ngdickana[i].shift == keyset0 && (ngdickana[i].douji & keyset1) == keyset1) ||
                   (ngdickana[i].shift == 0UL && (ngdickana[i].douji & (keyset0 | keyset1)) == (keyset0 | keyset1));
          // しぇ、ちぇ、など2キーで確定してはいけない
          if (matches && (ngdickana[i].shift | ngdickana[i].douji) != (keyset0 | keyset1)) {
            count = 2;
          }
        }
        break;
      default:
        if (exact_match) {
          matches = (ngdickana[i].shift == (keyset0 | keyset1) && ngdickana[i].douji == keyset2) ||
                   (ngdickana[i].shift == keyset0 && ngdickana[i].douji == (keyset1 | keyset2)) ||
                   (ngdickana[i].shift == 0UL && ngdickana[i].douji == (keyset0 | keyset1 | keyset2));
        } else {
          matches = (ngdickana[i].shift == (keyset0 | keyset1) && (ngdickana[i].douji & keyset2) == keyset2) ||
                   (ngdickana[i].shift == keyset0 && (ngdickana[i].douji & (keyset1 | keyset2)) == (keyset1 | keyset2)) ||
                   (ngdickana[i].shift == 0UL && (ngdickana[i].douji & (keyset0 | keyset1 | keyset2)) == (keyset0 | keyset1 | keyset2));
        }
        break;
    }

    if (matches) {
      count++;
      if (count > 1) break;
    }
  }

  return count;
}

int number_of_matches(NGList *keys) {  
  int result = count_kana_entries(keys, true);
  return result;
}

int number_of_candidates(NGList *keys) {
  int result = count_kana_entries(keys, false);
  return result;
}

// キー入力を文字に変換して出力する
void ng_type(NGList *keys) {
    if (keys->size == 0)
        return;

    if (keys->size == 1 && keys->elements[0] == KEY_ENTER) {
        printf(" NAGINATA type keycode 0x%02X\n", KEY_ENTER);
        output_char(KEY_ENTER);
        return;
    }

    int keyset = 0UL;
    for (int i = 0; i < keys->size; i++) {
        keyset |= ng_key[keys->elements[i] - KEY_Q];
    }

    for (unsigned long int i = 0; i < sizeof ngdickana / sizeof ngdickana[0]; i++) {
        if ((ngdickana[i].shift | ngdickana[i].douji) == keyset) {
            if (ngdickana[i].kana[0] != KEY_NONE) {
                for (int k = 0; k < 6; k++) {
                    if (ngdickana[i].kana[k] == KEY_NONE)
                        break;
                    printf(" NAGINATA type keycode 0x%02X\n", ngdickana[i].kana[k]);
                    output_char(ngdickana[i].kana[k]);
                }
            } else {
                ngdickana[i].func();
            }
            return;
        }
    }

    // JIみたいにJIを含む同時押しはたくさんあるが、JIのみの同時押しがないとき
    // 最後の１キーを別に分けて変換する
    NGList a, b;
    initializeList(&a);
    initializeList(&b);
    for (int i = 0; i < keys->size - 1; i++) {
        addToList(&a, keys->elements[i]);
    }
    addToList(&b, keys->elements[keys->size - 1]);
    ng_type(&a);
    ng_type(&b);
}

// 薙刀式の入力処理
bool naginata_press(OYAYUBI_EVENT ev) {
    __u16 keycode = ev.keyCode;

    switch (keycode) {
    case KEY_Q ... KEY_P:
    case KEY_A ... KEY_SEMICOLON:
    case KEY_Z ... KEY_SLASH:
    case KEY_SPACE:
    case KEY_ENTER:
        n_pressed_keys++;
        pressed_keys |= ng_key[keycode - KEY_Q]; // キーの重ね合わせ

        if (keycode == KEY_SPACE || keycode == KEY_ENTER) {
            NGList a;
            initializeList(&a);
            addToList(&a, keycode);
            addToListArray(&nginput, &a);
        } else {
            NGList a;
            NGList b;
            if (nginput.size > 0) {
                copyList(&(nginput.elements[nginput.size - 1]), &a);
                copyList(&a, &b);
                addToList(&b, keycode);
            }

            // 前のキーとの同時押しの可能性があるなら前に足す
            // 同じキー連打を除外
            if (nginput.size > 0 && a.elements[a.size - 1] != keycode &&
                number_of_candidates(&b) > 0) {
                removeFromListArrayAt(&nginput, nginput.size - 1);
                addToListArray(&nginput, &b);
                // 前のキーと同時押しはない
            } else {
                // 連続シフトではない
                NGList e;
                initializeList(&e);
                addToList(&e, keycode);
                addToListArray(&nginput, &e);
            }
        }

        // 連続シフト
        static int rs[10][2] = {{KEY_D, KEY_F},     {KEY_C, KEY_V}, {KEY_J, KEY_K}, {KEY_M, KEY_COMMA}, {KEY_SPACE, 0},
                                     {KEY_ENTER, 0}, {KEY_F, 0}, {KEY_V, 0}, {KEY_J, 0},     {KEY_M, 0}};

        int keyset = 0UL;
        for (int i = 0; i < nginput.elements[0].size; i++) {
            keyset |= ng_key[nginput.elements[0].elements[i] - KEY_Q];
        }
        for (int i = 0; i < 10; i++) {
            NGList rskc;
            initializeList(&rskc);
            addToList(&rskc, rs[i][0]);
            if (rs[i][1] > 0) {
                addToList(&rskc, rs[i][1]);
            }

            int c = includeList(&rskc, keycode);
            int brs = 0UL;
            for (int j = 0; j < rskc.size; j++) {
                brs |= ng_key[rskc.elements[j] - KEY_Q];
            }

            NGList l = nginput.elements[nginput.size - 1];
            for (int j = 0; j < l.size; j++) {
                addToList(&rskc, l.elements[j]);
            }

            if (c < 0 && ((brs & pressed_keys) == brs) && (keyset & brs) != brs && number_of_matches(&rskc) > 0) {
                nginput.elements[nginput.size - 1] = rskc;
                break;
            }
        }

        if (nginput.size > 1 || number_of_candidates(&(nginput.elements[0])) == 1) {
            ng_type(&(nginput.elements[0]));
            removeFromListArrayAt(&nginput, 0);
        }
        break;
    }

    return true;
}

bool naginata_release(OYAYUBI_EVENT ev) {
    __u16 keycode = ev.keyCode;

    switch (keycode) {
    case KEY_Q ... KEY_P:
    case KEY_A ... KEY_SEMICOLON:
    case KEY_Z ... KEY_SLASH:
    case KEY_SPACE:
    case KEY_ENTER:
        if (n_pressed_keys > 0)
            n_pressed_keys--;
        if (n_pressed_keys == 0)
            pressed_keys = 0UL;

        pressed_keys &= ~ng_key[keycode - KEY_Q]; // キーの重ね合わせ

        if (pressed_keys == 0UL) {
            while (nginput.size > 0) {
                ng_type(&(nginput.elements[0]));
                removeFromListArrayAt(&nginput, 0);
            }
        } else {
            if (nginput.size > 0 && number_of_candidates(&(nginput.elements[0])) == 1) {
                ng_type(&(nginput.elements[0]));
                removeFromListArrayAt(&nginput, 0);
            }
        }
        break;
    }

    return true;
}

void handle_naginata_event(OYAYUBI_EVENT ev) {
    if (ev.isRepeat) {
        return;
    }
    printf(" NAGINATA handle event type %d keycode 0x%02X\n", ev.eventType, ev.keyCode);

    switch (ev.eventType) {
    case ET_KEYDOWN:
        naginata_press(ev);
        break;
    case ET_KEYUP:
        naginata_release(ev);
        break;
    }
}

// 薙刀式

// static int naginata_init(const struct device *dev) {
//     printf("NAGINATA INIT");

//     initializeListArray(&nginput);
//     pressed_keys = 0UL;
//     n_pressed_keys = 0;

//     return 0;
// };

void nofunc() {}
