#include <pebble.h>
#include "solar_functions.h"
#include "com_functions.h"
#include "main.h"

Window *window;
Layer *window_layer;
uint16_t noon;
uint16_t date_recieved;
bool date = 0;

TextLayer *text_time_layer;
TextLayer *text_date_layer;

GRect time_pos = {.origin = {.x = 0, .y = 0}, .size = {.w = 144-16, .h = 0}};

struct tm *time_first;

void draw_time(struct tm *tick_time) {
	static char time_text[] = "00:00";
  char *time_format;
	
	if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }
	
	strftime(time_text, sizeof(time_text), time_format, tick_time);

  // Kludge to handle lack of non-padded hour format string
  // for twelve hour clock.
  if (!clock_is_24h_style() && (time_text[0] == '0')) {
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }
	
  text_layer_set_text(text_time_layer, time_text);
}

void draw_date(struct tm *tick_time) {
		static char date_text[] = "Xxx 00";
		strftime(date_text, sizeof(date_text), "%b %e", tick_time);
		text_layer_set_text(text_date_layer, date_text);
}


void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
	draw_time(tick_time);
	place_sun(tick_time, 0);
}

void set_font(GFont font1, GFont font2){ //little function for date case
	text_layer_set_font(text_time_layer, font1);
	text_layer_set_font(text_date_layer, font2);
}


void apply_settings(struct settings *settings){
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Apply Settings");
	
	if (!settings->timedate){
		time_pos.origin.x = 8;
		time_pos.origin.y = 10;
		time_pos.size.h = 168-92;
		layer_set_hidden(text_layer_get_layer(text_date_layer), true);
		date = 0;
		
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Apply Settings: Type and Font");
		if (settings->type == 0) {
			if (settings->font == 0) {
				text_layer_set_font(text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LATO_LIGHT_49)));
			}	
			else if (settings->font == 1) {
				text_layer_set_font(text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LATO_REGULAR_49)));
			}
			else if (settings->font == 2) {
				text_layer_set_font(text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LATO_BOLD_49)));
			}
		}	
		else if (settings->type == 1) {
			if (settings->font == 0) {
				text_layer_set_font(text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FUTURA_REGULAR_48)));
			}
			else if (settings->font == 1) {
				text_layer_set_font(text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FUTURA_REGULAR_48)));
			}
			else if (settings->font == 2) {
				text_layer_set_font(text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FUTURA_BOLD_48)));
			}
		}
	}
	
	else if (settings->timedate){
		time_pos.origin.x = 8;
		time_pos.origin.y = 31;
		time_pos.size.h = 144-16;
		layer_set_hidden(text_layer_get_layer(text_date_layer), false);
		time_t now = time(NULL);
		time_first = localtime(&now);
		draw_date(time_first);
	
		date = 1;
		
			//font and and font style
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Apply Settings: Type and Font");
		if (settings->type == 0) {
			if (settings->font == 0) {
				set_font(fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LATO_LIGHT_40)), fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LATO_LIGHT_22)));
			}	
			else if (settings->font == 1) {
				set_font(fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LATO_REGULAR_40)), fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LATO_REGULAR_22)));
			}
			else if (settings->font == 2) {
				set_font(fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LATO_BOLD_40)), fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LATO_BOLD_22)));
			}
		}	
		else if (settings->type == 1) {
			if (settings->font == 0) {
				set_font(fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FUTURA_REGULAR_39)), fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FUTURA_REGULAR_21)));
			}
			else if (settings->font == 1) {
				set_font(fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FUTURA_REGULAR_39)), fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FUTURA_REGULAR_21)));
			}
			else if (settings->font == 2) {
			set_font(fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FUTURA_BOLD_39)), fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FUTURA_BOLD_21)));
			}
		}
	}
	
	layer_set_frame(text_layer_get_layer(text_time_layer), time_pos);
	
	text_layer_set_background_color(text_time_layer, GColorClear);
	text_layer_set_text_alignment(text_time_layer, GTextAlignmentRight);
	layer_add_child(window_layer, text_layer_get_layer(text_time_layer));
	
	//invert and thickness
	solar_settings(settings); //This is issue
	
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Apply Settings: Invert");
	if (settings->invert){
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Apply Settings: Invert, inverted");
		window_set_background_color(window, GColorBlack);
		text_layer_set_text_color(text_time_layer, GColorWhite);
		if (settings->timedate){
			text_layer_set_text_color(text_date_layer, GColorWhite);
		}
	}
	else if (!settings->invert) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Apply Settings: Invert, not inverted");
		window_set_background_color(window, GColorWhite);
		text_layer_set_text_color(text_time_layer, GColorBlack);
		text_layer_set_text_color(text_date_layer, GColorBlack);
	}
	
	APP_LOG(APP_LOG_LEVEL_DEBUG, "end of apply settings");
}

void init(void) {
//	timepos = GRect(0, 0, 0, 0);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Begin intit");
	window = window_create();
	window_layer = window_get_root_layer(window);
	bool need_to_send = 0;

	setup_frame();
	
	text_time_layer = text_layer_create(time_pos);
	text_layer_set_background_color(text_time_layer, GColorClear);
	text_layer_set_text_alignment(text_time_layer, GTextAlignmentRight);
	layer_add_child(window_layer, text_layer_get_layer(text_time_layer));
		
	text_date_layer = text_layer_create(GRect(8, 7, 144-16, 30)); //(8, 7, 144-16, 30)
	text_layer_set_background_color(text_date_layer, GColorClear);
	text_layer_set_text_alignment(text_date_layer, GTextAlignmentLeft);
	layer_add_child(window_layer, text_layer_get_layer(text_date_layer));
	
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Window push");
	window_stack_push(window, true);


	time_t now = time(NULL);
  time_first = localtime(&now);
	
	if (persist_exists(REC_DATE)){
		date_recieved = persist_read_int(REC_DATE);
		if (date_recieved != time_first->tm_yday){
			APP_LOG(APP_LOG_LEVEL_DEBUG, "Data is old, Sending message");
			need_to_send = 1;
		}
		else {
			APP_LOG(APP_LOG_LEVEL_DEBUG, "Reading from storage");
			uint8_t height = persist_read_int(HEIGHT_KEY);
			noon = persist_read_int(NOON_KEY);
			APP_LOG(APP_LOG_LEVEL_DEBUG, "does HEIGHT_KEY exist? %i", persist_exists(HEIGHT_KEY));
			APP_LOG(APP_LOG_LEVEL_DEBUG, "height:%i, noon:%i", height, noon);
			init_draw(height, time_first);
		}
	}
	else {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "No previous data, sending message");
		need_to_send = 1;
	}
	
	struct settings settings;
	if (persist_exists(SETTINGS_PKEY)){
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Read settings");
		persist_read_data(SETTINGS_PKEY, &settings, sizeof(settings));
	}
	else {
		settings.thickness = 1;
		settings.invert = 0;
		settings.hor = 0;
		settings.font = 1;
		settings.type = 0;
		settings.timedate = 0;
	}
	apply_settings(&settings);
	
	// Register AppMessage handlers
	APP_LOG(APP_LOG_LEVEL_DEBUG, "App message subscribe begin");
	app_message_register_inbox_received(in_received_handler);
	app_message_register_inbox_dropped(in_dropped_handler); 
	app_message_register_outbox_failed(out_failed_handler);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "App message subscribe end");
	
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Open inbox");
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
	
	if(need_to_send){
		send_message();
	}

	tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Separate minute tick handler");
	draw_time(time_first);
}

void deinit(void) {
	tick_timer_service_unsubscribe();
	text_layer_destroy(text_time_layer);
	app_message_deregister_callbacks();
	destroy_frame();
	window_destroy(window);
}

int main( void ) {
	init();
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Init done");
	app_event_loop();
	deinit();
}