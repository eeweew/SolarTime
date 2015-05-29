#include <pebble.h>
#include "com_functions.h"
#include "solar_functions.h"
#include "main.h"
	
BitmapLayer *sine_layer;
Layer *sun_layer;

GBitmap *sine_thick;
GBitmap *sine_thin;
InverterLayer *horizon_layer;
Layer *horizon_line_layer;
Layer *current_hor_layer;
GRect horizon_pos = {.origin = {.x = 0, .y = 0}, .size = {.w = 145, .h = 1}}; //52 //116
GRect sun_pos = {.origin = {.x = 63, .y = 80}, .size = {.w = 17, .h = 17}};
GColor line_color;

void horizon_line_update_callback(Layer *layer, GContext* ctx) {
  graphics_context_set_fill_color(ctx, line_color); //foreground
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
}

int lookup(int pixel){
	static const uint8_t table[72] = {
		0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 7, 8, 9, 10 ,11 ,12 ,13 ,14 ,15 ,16 ,18 ,19 ,20 ,21 ,23, 
		24, 25, 26, 28, 29, 30, 32, 33, 34, 36, 37, 38, 39, 41, 42, 43, 44, 45, 46,
		48, 49, 50, 51, 52, 52, 53, 54, 55, 56, 56, 57, 57, 58, 58, 59, 59, 59,
		60, 60, 60, 60};
	
	if (pixel > 71){
		pixel = 143 - pixel;
	}
	
	return table[pixel];
}

void solar_settings(struct settings *settings) {
	if (settings->invert) {
		bitmap_layer_set_compositing_mode(sine_layer, GCompOpAssignInverted);
		line_color = GColorWhite;
	}
	else if (!settings->invert) {
		bitmap_layer_set_compositing_mode(sine_layer, GCompOpAssign);
		line_color = GColorBlack;
	}
	layer_mark_dirty(horizon_line_layer);
	
	if (settings->thickness){
		bitmap_layer_set_bitmap(sine_layer, sine_thick);
	}
	if (!settings->thickness){
		bitmap_layer_set_bitmap(sine_layer, sine_thin);
	}
	
	if (settings->hor) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Line selected");
		horizon_pos.size.h = 2;
		layer_set_frame(horizon_line_layer, horizon_pos);
		layer_set_hidden(inverter_layer_get_layer(horizon_layer), true);
		layer_set_hidden(horizon_line_layer, false);
		current_hor_layer = horizon_line_layer;
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Line selected end");
	}
	else if (!settings->hor) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Block selected");
		Layer *horizon_layer_frame = inverter_layer_get_layer(horizon_layer);
		horizon_pos.size.h = 84;
		layer_set_frame(horizon_layer_frame, horizon_pos);
		layer_set_hidden(inverter_layer_get_layer(horizon_layer), false);
		layer_set_hidden(horizon_line_layer, true);
		current_hor_layer = horizon_layer_frame;
	}
}
	
void setup_frame(void){
	Layer *window_layer = window_get_root_layer(window);
	
	sine_layer = bitmap_layer_create(GRect(0, 84, 144, 64));
	sine_thin = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SINE_THIN);
	sine_thick = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SINE);
	
	layer_add_child(window_layer, bitmap_layer_get_layer(sine_layer));
	
	horizon_layer = inverter_layer_create(horizon_pos);
	layer_add_child(window_layer, inverter_layer_get_layer(horizon_layer));
	layer_set_hidden(inverter_layer_get_layer(horizon_layer), true);
	
	horizon_line_layer = layer_create(horizon_pos);
	layer_set_update_proc(horizon_line_layer, horizon_line_update_callback);
	layer_add_child(window_layer, horizon_line_layer);
	layer_set_hidden(horizon_line_layer, true);
	
	current_hor_layer = horizon_line_layer;
	
	sun_layer = layer_create(sun_pos);
	layer_set_update_proc(sun_layer, draw_sun);
	layer_add_child(window_layer, sun_layer);
	layer_set_hidden(sun_layer, true);
}

void destroy_frame(void){
	bitmap_layer_destroy(sine_layer);
	layer_destroy(sun_layer);
	inverter_layer_destroy(horizon_layer);
	gbitmap_destroy(sine_thick);
	gbitmap_destroy(sine_thin);
}

void init_draw(uint8_t height, struct tm *time_first){
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Starting init_draw()");
	APP_LOG(APP_LOG_LEVEL_DEBUG, "height = : %i", height);
	place_horizon(height);
	place_sun(time_first, 1);
	layer_set_hidden(sun_layer, false);
}

void place_horizon(uint8_t height) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Now inside place_horizon()");
	APP_LOG(APP_LOG_LEVEL_DEBUG, "height = %i", height);
	horizon_pos.origin.y = (84+(64-height));
	layer_set_frame(current_hor_layer, horizon_pos);	
}

void draw_sun(Layer *Layer, GContext *ctx) {
	GPoint p = GPoint(8, 8);
	graphics_draw_circle(ctx, p, 8);
	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_fill_circle(ctx, p, 7);
}

void place_sun(struct tm *time_input, bool bypas){
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Place sun started");
	
	uint16_t minutes = 60*time_input->tm_hour + time_input->tm_min;
	APP_LOG(APP_LOG_LEVEL_DEBUG, "hour: %i, minute: %i, minutes: %i", time_input->tm_hour, time_input->tm_min, minutes);
	int solar_minute = minutes - noon;
	if (solar_minute < 0){
		solar_minute += 1440;
	}
	
	APP_LOG(APP_LOG_LEVEL_DEBUG, "solar_minute: %i, noon: %i", solar_minute, noon);
	
	if (solar_minute % 10 == 0 || bypas == 1){
		if (time_input->tm_yday != date_recieved){
			send_message();
			draw_date(time_input);
			APP_LOG(APP_LOG_LEVEL_DEBUG, "New day, fetching new information");
		}
		uint8_t pixelindex = solar_minute/10;
		APP_LOG(APP_LOG_LEVEL_DEBUG, "pixelindex %i", pixelindex);
		uint8_t elev = lookup(pixelindex);
		sun_pos.origin.x = pixelindex-8;
		sun_pos.origin.y = 168-(elev+22+8);
		layer_set_frame(sun_layer, sun_pos);
	}
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Place sun ended");
}