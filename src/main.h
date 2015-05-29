#pragma once
#include <pebble.h>
	
void apply_settings(struct settings *settings);
void draw_date(struct tm *time);
	
extern Window *window;
extern uint16_t noon;
extern uint16_t date_recieved;