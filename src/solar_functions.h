#pragma once

void init_draw(uint8_t height, struct tm *time_first);
void place_horizon(uint8_t height);
void draw_sun(Layer *Layer, GContext *ctx);
void place_sun(struct tm *time_input, bool bypas);
void setup_frame();
void destroy_frame();
struct settings;
void solar_settings(struct settings *settings);