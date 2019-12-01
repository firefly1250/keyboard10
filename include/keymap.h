#ifndef KEYMAP_H_
#define KEY_MAP_H

#include <unordered_map>
#include <string>

extern std::unordered_map<std::string, uint8_t> keycodes;
extern uint8_t keymap[128];
extern uint8_t modifiermap[4];

constexpr uint8_t key_ctrl = 1;
constexpr uint8_t key_shift = 2;
constexpr uint8_t key_alt = 4;
constexpr uint8_t key_gui = 8;

constexpr uint8_t key_left_click = 250;
constexpr uint8_t key_right_click = 251;
constexpr uint8_t key_middle_click = 252;

void init_keymap();

#endif