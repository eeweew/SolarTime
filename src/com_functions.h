#pragma once
	
#define STATUS_KEY 0
//	SUNSET_KEY = 1,
//	SUNRISE_KEY = 2,
#define HEIGHT_KEY 3
#define	NOON_KEY 4

//settings
#define THICK_KEY 5
#define FONT_KEY 6
#define HOR_KEY 7
#define INVERT_KEY 8
#define TYPE_KEY 9
#define TIMEDATE_KEY 10
	
#define SETTINGS_PKEY	11

#define REC_DATE 100
	
struct settings {
	bool thickness, invert, hor, timedate;
	uint8_t font, type;
};

//functions
void error_setup(void);
void error_deint(void);
void send_message(void);
void in_received_handler(DictionaryIterator *received, void *context);
void in_dropped_handler(AppMessageResult reason, void *context);
void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context);