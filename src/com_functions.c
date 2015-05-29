#include <pebble.h>
#include "com_functions.h"
#include "solar_functions.h"
#include "main.h"

void send_message(void){
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Send_message started");
	if (bluetooth_connection_service_peek()){
		APP_LOG(APP_LOG_LEVEL_DEBUG, "BT connected");
		
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Dict itter 1");
		DictionaryIterator *iter;
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Dict itter 2");
		app_message_outbox_begin(&iter);
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Dict itter 3");
		dict_write_uint8(iter, 0, 0);
	
//	dict_write_end(iter);
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Send to outbox");
  	app_message_outbox_send();
	}
}

void in_received_handler(DictionaryIterator *received, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "In received");
	bool rec_height = 0;
	bool rec_noon = 0;
	uint8_t height = 0;
	bool thick = 0;
	uint8_t font = 0;
	uint8_t style = 1;
	bool hor_style = 0;
	bool invert = 0;
	
	struct settings settings;
	
	
	Tuple *t = dict_read_first(received);
	
	  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
/*    case SUNSET_KEY:
			sunset = (int)t->value->int32;
			APP_LOG(APP_LOG_LEVEL_DEBUG, "SUNSET: %i", sunset);

      break;
    case SUNRISE_KEY:
			sunrise = (int)t->value->int32;
			APP_LOG(APP_LOG_LEVEL_DEBUG, "SUNRISE: %i", sunrise);
			
			break;*/
			
		case THICK_KEY:			
			if(strcmp(t->value->cstring, "thick") == 0) {
				thick = 1;
				APP_LOG(APP_LOG_LEVEL_DEBUG, "thick");
			}
			else if(strcmp(t->value->cstring, "thin") == 0) {
				thick = 0;
				APP_LOG(APP_LOG_LEVEL_DEBUG, "thin");
			}
			settings.thickness = thick;
			break;
			
		case TYPE_KEY:
			if(strcmp(t->value->cstring, "lato") == 0) {
				font = 0;
				APP_LOG(APP_LOG_LEVEL_DEBUG, "lato");
			}
			else if(strcmp(t->value->cstring, "futura") == 0) {
				font = 1;
				APP_LOG(APP_LOG_LEVEL_DEBUG, "futura");
			}
			settings.type = font;
			break;
			
		case FONT_KEY:
			if(strcmp(t->value->cstring, "bold") == 0) {
				style = 2;
				APP_LOG(APP_LOG_LEVEL_DEBUG, "bold");
			}
			else if(strcmp(t->value->cstring, "regular") == 0) {
				style = 1;
				APP_LOG(APP_LOG_LEVEL_DEBUG, "regular");
			}
			else if(strcmp(t->value->cstring, "light") == 0) {
				style = 0;
				APP_LOG(APP_LOG_LEVEL_DEBUG, "light");
			}
			settings.font = style;
			break;
			
		case HOR_KEY:
			if(strcmp(t->value->cstring, "block") == 0) {
				hor_style = 0;
				APP_LOG(APP_LOG_LEVEL_DEBUG, "block");
			}
			else if(strcmp(t->value->cstring, "line") == 0) {
				hor_style = 1;
				APP_LOG(APP_LOG_LEVEL_DEBUG, "line");
			}
			settings.hor = hor_style;
			break;
			
		case INVERT_KEY:
			if(strcmp(t->value->cstring, "white") == 0) {
				invert = 0;
				APP_LOG(APP_LOG_LEVEL_DEBUG, "white");
			}
			else if(strcmp(t->value->cstring, "black") == 0) {
				invert = 1;
				APP_LOG(APP_LOG_LEVEL_DEBUG, "black");
			}
			settings.invert = invert;
			break;
			
		case TIMEDATE_KEY:
			if(strcmp(t->value->cstring, "time") == 0) {
				settings.timedate = 0;
				APP_LOG(APP_LOG_LEVEL_DEBUG, "time");
			}
			if(strcmp(t->value->cstring, "timedate") == 0) {
				settings.timedate = 1;
				APP_LOG(APP_LOG_LEVEL_DEBUG, "timedate");
			}
			break;
			
    case HEIGHT_KEY:
			rec_height = 1;
			height = (int)t->value->int32;
			persist_write_int(HEIGHT_KEY, height);
			APP_LOG(APP_LOG_LEVEL_DEBUG, "HEIGHT: %i", height);				
			break;
			
    case NOON_KEY:
			rec_noon = 1;
			noon = (int)t->value->int32;
			persist_write_int(NOON_KEY, noon);
			APP_LOG(APP_LOG_LEVEL_DEBUG, "NOON: %i", noon);

      break;
    default:
      APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }

    // Look for next item
    t = dict_read_next(received);
	}
	
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Out received");
	
	if(rec_height && rec_noon){
		time_t now = time(NULL);
  	struct tm *time_message = localtime(&now);
		date_recieved = time_message->tm_yday;
		persist_write_int(REC_DATE, date_recieved);
		init_draw(height, time_message);
	}
	else{
		apply_settings(&settings);
		persist_write_data(SETTINGS_PKEY, &settings, sizeof(settings));
	}
}

void in_dropped_handler(AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped");
}

// Called when PebbleKitJS does not acknowledge receipt of a message
void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
	APP_LOG(APP_LOG_LEVEL_ERROR, "Send message failed, trying again");
	send_message();
}