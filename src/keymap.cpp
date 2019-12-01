#include "keymap.h"

std::unordered_map<std::string, uint8_t> keycodes = {
    {
        "",
        0,
    },
    {"a", 4},
    {"b", 5},
    {"c", 6},
    {"d", 7},
    {"e", 8},
    {"f", 9},
    {"g", 10},
    {"h", 11},
    {"i", 12},
    {"j", 13},
    {"k", 14},
    {"l", 15},
    {"m", 16},
    {"n", 17},
    {"o", 18},
    {"p", 19},
    {"q", 20},
    {"r", 21},
    {"s", 22},
    {"t", 23},
    {"u", 24},
    {"v", 25},
    {"w", 26},
    {"x", 27},
    {"y", 28},
    {"z", 29},
    {"1", 30},
    {"2", 31},
    {"3", 32},
    {"4", 33},
    {"5", 34},
    {"6", 35},
    {"7", 36},
    {"8", 37},
    {"9", 38},
    {"0", 39},
    {"return", 40},
    {"escape", 41},
    {"backspace", 42},
    {"tab", 43},
    {"space", 44},
    {"- =", 45},
    {"^ ~", 46},
    {"@ `", 47},
    {"[ {", 48},
    {"] }", 49},
    {"; +", 51},
    {": *", 52},
    {"zen_han", 53},
    {", <", 54},
    {". >", 55},
    {"/ ?", 56},
    {"eisuu_toggle caps_lock", 57},
    {"f1", 58},
    {"f2", 59},
    {"f3", 60},
    {"f4", 61},
    {"f5", 62},
    {"f6", 63},
    {"f7", 64},
    {"f8", 65},
    {"f9", 66},
    {"f10", 67},
    {"f11", 68},
    {"f12", 69},
    {"print", 70},
    {"scroll_lock", 71},
    {"pause", 72},
    {"insert", 73},
    {"home", 74},
    {"page_up", 75},
    {"delete", 76},
    {"end", 77},
    {"page_down", 78},
    {"right", 79},
    {"left", 80},
    {"down", 81},
    {"up", 82},
    {"num_lock", 83},
    {"menu", 101},
    {"touchpad", 112},
    {"silent", 127},
    {"quiet", 129},
    {"loud", 128},
    {"\\ _", 135},
    {"\\ |", 137},
    {"left_click", key_left_click},
    {"right_click", key_right_click},
    {"middle_click", key_middle_click},
};

uint8_t keymap[128];
uint8_t modifiermap[4];

void init_keymap() {
  modifiermap[0] = key_ctrl;
  modifiermap[1] = key_shift;
  modifiermap[2] = key_alt;
  modifiermap[3] = key_gui;

  constexpr auto fn = 64;
  keymap[0] = keycodes[""];
  keymap[0 | fn] = keycodes[""];
  keymap[1] = keycodes["1"];
  keymap[1 | fn] = keycodes["f1"];
  keymap[2] = keycodes["2"];
  keymap[2 | fn] = keycodes["f2"];
  keymap[3] = keycodes["3"];
  keymap[3 | fn] = keycodes["f3"];
  keymap[4] = keycodes["4"];
  keymap[4 | fn] = keycodes["f4"];
  keymap[5] = keycodes["5"];
  keymap[5 | fn] = keycodes["f5"];
  keymap[6] = keycodes["6"];
  keymap[6 | fn] = keycodes["f6"];
  keymap[7] = keycodes["7"];
  keymap[7 | fn] = keycodes["f7"];
  keymap[8] = keycodes["8"];
  keymap[8 | fn] = keycodes["f8"];
  keymap[9] = keycodes["9"];
  keymap[9 | fn] = keycodes["f9"];
  keymap[10] = keycodes["0"];
  keymap[10 | fn] = keycodes["f10"];
  keymap[11] = keycodes["a"];
  keymap[11 | fn] = keycodes["f11"];
  keymap[12] = keycodes["b"];
  keymap[12 | fn] = keycodes["f12"];
  keymap[13] = keycodes["c"];
  keymap[13 | fn] = keycodes[""];
  keymap[14] = keycodes["d"];
  keymap[14 | fn] = keycodes[""];
  keymap[15] = keycodes["e"];
  keymap[15 | fn] = keycodes["eisuu_toggle caps_lock"];
  keymap[16] = keycodes["f"];
  keymap[16 | fn] = keycodes[""];
  keymap[17] = keycodes["g"];
  keymap[17 | fn] = keycodes[""];
  keymap[18] = keycodes["h"];
  keymap[18 | fn] = keycodes[""];
  keymap[19] = keycodes["i"];
  keymap[19 | fn] = keycodes["insert"];
  keymap[20] = keycodes["j"];
  keymap[20 | fn] = keycodes[""];
  keymap[21] = keycodes["k"];
  keymap[21 | fn] = keycodes[""];
  keymap[22] = keycodes["l"];
  keymap[22 | fn] = keycodes["loud"];
  keymap[23] = keycodes["m"];
  keymap[23 | fn] = keycodes["menu"];
  keymap[24] = keycodes["n"];
  keymap[24 | fn] = keycodes[""];
  keymap[25] = keycodes["o"];
  keymap[25 | fn] = keycodes[""];
  keymap[26] = keycodes["p"];
  keymap[26 | fn] = keycodes["print"];
  keymap[27] = keycodes["q"];
  keymap[27 | fn] = keycodes["quiet"];
  keymap[28] = keycodes["r"];
  keymap[28 | fn] = keycodes[""];
  keymap[29] = keycodes["s"];
  keymap[29 | fn] = keycodes["silent"];
  keymap[30] = keycodes["t"];
  keymap[30 | fn] = keycodes["touchpad"];
  keymap[31] = keycodes["u"];
  keymap[31 | fn] = keycodes[""];
  keymap[32] = keycodes["v"];
  keymap[32 | fn] = keycodes[""];
  keymap[33] = keycodes["w"];
  keymap[33 | fn] = keycodes[""];
  keymap[34] = keycodes["x"];
  keymap[34 | fn] = keycodes[""];
  keymap[35] = keycodes["y"];
  keymap[35 | fn] = keycodes[""];
  keymap[36] = keycodes["z"];
  keymap[36 | fn] = keycodes["zen_han"];
  keymap[37] = keycodes["return"];
  keymap[37 | fn] = keycodes[""];
  keymap[38] = keycodes["space"];
  keymap[38 | fn] = keycodes[""];
  keymap[39] = keycodes["backspace"];
  keymap[39 | fn] = keycodes[""];
  keymap[40] = keycodes["delete"];
  keymap[40 | fn] = keycodes[""];
  keymap[41] = keycodes["tab"];
  keymap[41 | fn] = keycodes[""];
  keymap[42] = keycodes["left"];
  keymap[42 | fn] = keycodes[""];
  keymap[43] = keycodes["right"];
  keymap[43 | fn] = keycodes[""];
  keymap[44] = keycodes["up"];
  keymap[44 | fn] = keycodes["page_up"];
  keymap[45] = keycodes["down"];
  keymap[45 | fn] = keycodes["page_down"];
  keymap[46] = keycodes["home"];
  keymap[46 | fn] = keycodes[""];
  keymap[47] = keycodes["end"];
  keymap[47 | fn] = keycodes[""];
  keymap[48] = keycodes["\\ _"];
  keymap[48 | fn] = keycodes[""];
  keymap[49] = keycodes["\\ |"];
  keymap[49 | fn] = keycodes[""];
  keymap[50] = keycodes["left_click"];
  keymap[50 | fn] = keycodes[""];
  keymap[51] = keycodes["- ="];
  keymap[51 | fn] = keycodes[""];
  keymap[52] = keycodes["right_click"];
  keymap[52 | fn] = keycodes[""];
  keymap[53] = keycodes["^ ~"];
  keymap[53 | fn] = keycodes[""];
  keymap[54] = keycodes["middle_click"];
  keymap[54 | fn] = keycodes[""];
  keymap[55] = keycodes["@ `"];
  keymap[55 | fn] = keycodes[""];
  keymap[56] = keycodes["[ {"];
  keymap[56 | fn] = keycodes[""];
  keymap[57] = keycodes["] }"];
  keymap[57 | fn] = keycodes[""];
  keymap[58] = keycodes["; +"];
  keymap[58 | fn] = keycodes[""];
  keymap[59] = keycodes[": *"];
  keymap[59 | fn] = keycodes[""];
  keymap[60] = keycodes[", <"];
  keymap[60 | fn] = keycodes[""];
  keymap[61] = keycodes[". >"];
  keymap[61 | fn] = keycodes[""];
  keymap[62] = keycodes["/ ?"];
  keymap[62 | fn] = keycodes[""];
  keymap[63] = keycodes["escape"];
  keymap[63 | fn] = keycodes[""];
}