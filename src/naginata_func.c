
#include "naginata_func.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <time.h>
#include "common.h"
#include "functions.h"
#include <unistd.h>

// 薙刀式をオン
void naginata_on(void) {
}

// 薙刀式をオフ
void naginata_off(void) {
}

void input_unicode_hex(int n1, int n2, int n3, int n4) {
    press_key(KEY_LEFTCTRL);
    press_key(KEY_LEFTSHIFT);
    output_char(KEY_U);
    release_key(KEY_LEFTSHIFT);
    release_key(KEY_LEFTCTRL);
    usleep(10000);
    output_char(n1);
    usleep(10000);
    output_char(n2);
    usleep(10000);
    output_char(n3);
    usleep(10000);
    output_char(n4);
    usleep(10000);
    output_char(KEY_ENTER);
}

void ng_T() { ng_left(1); }

void ng_Y() { ng_right(1); }

void ng_ST() {
    press_key(KEY_LEFTSHIFT);
    ng_left(1);
    release_key(KEY_LEFTSHIFT);
}

void ng_SY() {
    press_key(KEY_LEFTSHIFT);
    ng_right(1);
    release_key(KEY_LEFTSHIFT);
}
void ngh_JKQ() { // ^{End}
    ng_eof();
}

void ngh_JKW() { // ／{改行}
    input_unicode_hex(KEY_F, KEY_F, KEY_0, KEY_F);
}

void ngh_JKE() { // /*ディ*/
    output_char(KEY_D);
    output_char(KEY_H);
    output_char(KEY_I);
}

void ngh_JKR() { // ^s
    ng_save();
}

void ngh_JKT() { // ・
    output_char(KEY_SLASH);
}

void ngh_JKA() { // ……{改行}
    input_unicode_hex(KEY_2, KEY_0, KEY_2, KEY_6);
    input_unicode_hex(KEY_2, KEY_0, KEY_2, KEY_6);
}

void ngh_JKS() { // 『{改行}
    input_unicode_hex(KEY_3, KEY_0, KEY_0, KEY_E);
}

void ngh_JKD() { // ？{改行}
    press_key(KEY_LEFTSHIFT);
    output_char(KEY_SLASH);
    release_key(KEY_LEFTSHIFT);
    output_char(KEY_ENTER);
}

void ngh_JKF() { // 「{改行}
    input_unicode_hex(KEY_3, KEY_0, KEY_0, KEY_C);
}

void ngh_JKG() { // ({改行}
    input_unicode_hex(KEY_F, KEY_F, KEY_0, KEY_8);
}

void ngh_JKZ() { // ――{改行}
    input_unicode_hex(KEY_2, KEY_0, KEY_1, KEY_5);
    input_unicode_hex(KEY_2, KEY_0, KEY_1, KEY_5);
}

void ngh_JKX() { // 』{改行}
    input_unicode_hex(KEY_3, KEY_0, KEY_0, KEY_F);
}

void ngh_JKC() { // ！{改行}
    press_key(KEY_LEFTSHIFT);
    output_char(KEY_1);
    release_key(KEY_LEFTSHIFT);
    output_char(KEY_ENTER);
}

void ngh_JKV() { // 」{改行}
    input_unicode_hex(KEY_3, KEY_0, KEY_0, KEY_D);
}

void ngh_JKB() { // ){改行}
    input_unicode_hex(KEY_F, KEY_F, KEY_0, KEY_9);
}

void ngh_DFY() { // {Home}
    ng_home();
}

void ngh_DFU() { // +{End}{BS}
    press_key(KEY_LEFTSHIFT);
    ng_end();
    release_key(KEY_LEFTSHIFT);
    output_char(KEY_BACKSPACE);
}

void ngh_DFI() { // {vk1Csc079}
    ng_saihenkan();
}

void ngh_DFO() { // {Del}
    output_char(KEY_DELETE);
}

void ngh_DFP() { // +{Esc 2}
    press_key(KEY_LEFTSHIFT);
    output_char(KEY_ESC);
    output_char(KEY_ESC);
    release_key(KEY_LEFTSHIFT);
}

void ngh_DFH() { // {Enter}{End}
    output_char(KEY_ENTER);
    ng_end();
}

void ngh_DFJ() { // {↑}
    ng_up(1);
}

void ngh_DFK() { // +{↑}
    press_key(KEY_LEFTSHIFT);
    ng_up(1);
    release_key(KEY_LEFTSHIFT);
}

void ngh_DFL() { // +{↑ 7}
    press_key(KEY_LEFTSHIFT);
    ng_up(7);
    release_key(KEY_LEFTSHIFT);
}

void ngh_DFSCLN() { // ^i
    ng_katakana();
}

void ngh_DFN() { // {End}
    ng_end();
}

void ngh_DFM() { // {↓}
    ng_down(1);
}

void ngh_DFCOMM() { // +{↓}
    press_key(KEY_LEFTSHIFT);
    ng_down(1);
    release_key(KEY_LEFTSHIFT);
}

void ngh_DFDOT() { // +{↓ 7}
    press_key(KEY_LEFTSHIFT);
    ng_down(7);
    release_key(KEY_LEFTSHIFT);
}

void ngh_DFSLSH() { // ^u
    ng_hiragana();
}

void ngh_MCQ() { // ｜{改行}
    input_unicode_hex(KEY_F, KEY_F, KEY_5, KEY_C);
}

void ngh_MCW() { // 　　　×　　　×　　　×{改行 2}
    output_char(KEY_SPACE);
    output_char(KEY_SPACE);
    output_char(KEY_SPACE);
    input_unicode_hex(KEY_0, KEY_0, KEY_D, KEY_7);
    output_char(KEY_SPACE);
    output_char(KEY_SPACE);
    output_char(KEY_SPACE);
    input_unicode_hex(KEY_0, KEY_0, KEY_D, KEY_7);
    output_char(KEY_SPACE);
    output_char(KEY_SPACE);
    output_char(KEY_SPACE);
    input_unicode_hex(KEY_0, KEY_0, KEY_D, KEY_7);
    output_char(KEY_ENTER);
}

void ngh_MCE() { // {Home}{→}{End}{Del 2}{←}
    ng_home();
    ng_prev_row();
    ng_end();
    output_char(KEY_DELETE);
    output_char(KEY_DELETE);
    ng_next_row();
}

void ngh_MCR() { // {Home}{改行}{Space 1}{←}
    ng_home();
    output_char(KEY_ENTER);
    output_char(KEY_SPACE);
    ng_next_row();
}

void ngh_MCT() { // 〇{改行}
    input_unicode_hex(KEY_3, KEY_0, KEY_0, KEY_7);
}

void ngh_MCA() { // 《{改行}
    input_unicode_hex(KEY_3, KEY_0, KEY_0, KEY_A);
}

void ngh_MCS() { // 【{改行}
    input_unicode_hex(KEY_3, KEY_0, KEY_1, KEY_0);
}

void ngh_MCD() { // {Home}{→}{End}{Del 4}{←}
    ng_home();
    ng_prev_row();
    ng_end();
    output_char(KEY_DELETE);
    output_char(KEY_DELETE);
    output_char(KEY_DELETE);
    output_char(KEY_DELETE);
    ng_next_row();
}

void ngh_MCF() { // {Home}{改行}{Space 3}{←}
    ng_home();
    output_char(KEY_ENTER);
    output_char(KEY_SPACE);
    output_char(KEY_SPACE);
    output_char(KEY_SPACE);
    ng_next_row();
}

void ngh_MCG() { // {Space 3}
    output_char(KEY_SPACE);
    output_char(KEY_SPACE);
    output_char(KEY_SPACE);
}

void ngh_MCZ() { // 》{改行}
    input_unicode_hex(KEY_3, KEY_0, KEY_0, KEY_B);
}

void ngh_MCX() { // 】{改行}
    input_unicode_hex(KEY_3, KEY_0, KEY_1, KEY_1);
}

void ngh_MCC() { // 」{改行}{改行}
    input_unicode_hex(KEY_3, KEY_0, KEY_0, KEY_D);
    output_char(KEY_ENTER);
}

void ngh_MCV() { // 」{改行}{改行}「{改行}
    input_unicode_hex(KEY_3, KEY_0, KEY_0, KEY_D);
    output_char(KEY_ENTER);
    input_unicode_hex(KEY_3, KEY_0, KEY_0, KEY_C);
}

void ngh_MCB() { // 」{改行}{改行}{Space}
    input_unicode_hex(KEY_3, KEY_0, KEY_0, KEY_D);
    output_char(KEY_ENTER);
    output_char(KEY_SPACE);
}

void ngh_CVY() { // +{Home}
    press_key(KEY_LEFTSHIFT);
    ng_home();
    release_key(KEY_LEFTSHIFT);
}

void ngh_CVU() { // ^x
    ng_cut();
}

void ngh_CVI() { // {vk1Csc079}
    ng_saihenkan();
}

void ngh_CVO() { // ^v
    ng_paste();
}

void ngh_CVP() { // ^z
    ng_undo();
}

void ngh_CVH() { // ^c
    ng_copy();
}

void ngh_CVJ() { // {←}
    ng_left(1);
}

void ngh_CVK() { // {→}
    ng_right(1);
}

void ngh_CVL() { // {改行}{Space}+{Home}^x{BS}
    output_char(KEY_ENTER);
    output_char(KEY_SPACE);
    press_key(KEY_LEFTSHIFT);
    ng_home();
    release_key(KEY_LEFTSHIFT);
    ng_cut();
    output_char(KEY_BACKSPACE);
}

void ngh_CVSCLN() { // ^y
    ng_redo();
}

void ngh_CVN() { // +{End}
    press_key(KEY_LEFTSHIFT);
    ng_end();
    release_key(KEY_LEFTSHIFT);
}

void ngh_CVM() { // +{←}
    press_key(KEY_LEFTSHIFT);
    ng_left(1);
    release_key(KEY_LEFTSHIFT);
}

void ngh_CVCOMM() { // +{→}
    press_key(KEY_LEFTSHIFT);
    ng_right(1);
    release_key(KEY_LEFTSHIFT);
}

void ngh_CVDOT() { // +{← 7}
    press_key(KEY_LEFTSHIFT);
    ng_left(7);
    release_key(KEY_LEFTSHIFT);
}

void ngh_CVSLSH() { // +{→ 7}
    press_key(KEY_LEFTSHIFT);
    ng_right(7);
    release_key(KEY_LEFTSHIFT);
}

void ng_cut() {
    press_key(KEY_LEFTCTRL);
    output_char(KEY_X);
    release_key(KEY_LEFTCTRL);
}

void ng_copy() {
    press_key(KEY_LEFTCTRL);
    output_char(KEY_C);
    release_key(KEY_LEFTCTRL);
}

void ng_paste() {
    press_key(KEY_LEFTCTRL);
    output_char(KEY_V);
    release_key(KEY_LEFTCTRL);
}

void ng_up(int c) {
    for (int i = 0; i < c; i++) {
        output_char(KEY_UP);
    }
}

void ng_down(int c) {
    for (int i = 0; i < c; i++) {
        output_char(KEY_DOWN);
    }
}

void ng_left(int c) {
    for (int i = 0; i < c; i++) {
        output_char(KEY_LEFT);
    }
}

void ng_right(int c) {
    for (int i = 0; i < c; i++) {
        output_char(KEY_RIGHT);
    }
}

void ng_next_row() {
    ng_down(1);
}

void ng_prev_row() {
    ng_up(1);
}

void ng_next_char() {
    ng_right(1);
}

void ng_prev_char() {
    ng_left(1);
}

void ng_home() {
    output_char(KEY_HOME);
}

void ng_end() {
    output_char(KEY_END);
}

void ng_katakana() {
}

void ng_save() {
    press_key(KEY_LEFTCTRL);
    output_char(KEY_S);
    release_key(KEY_LEFTCTRL);
}

void ng_hiragana() {
}

void ng_redo() {
    press_key(KEY_LEFTCTRL);
    output_char(KEY_Y);
    release_key(KEY_LEFTCTRL);
}

void ng_undo() {
    press_key(KEY_LEFTCTRL);
    output_char(KEY_Z);
    release_key(KEY_LEFTCTRL);
}

void ng_saihenkan() {
    press_key(KEY_LEFTCTRL);
    output_char(KEY_BACKSLASH);
    release_key(KEY_LEFTCTRL);
}

void ng_eof() {
    press_key(KEY_LEFTCTRL);
    output_char(KEY_END);
    release_key(KEY_LEFTCTRL);
}
