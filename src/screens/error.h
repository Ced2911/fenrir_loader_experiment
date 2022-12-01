#pragma once
#include "screens/screen.h"

extern screen_t error_no_sd;
extern screen_t error_bad_filesystem;

// error
void error_set_message(const char * str) {}
void error_set_title(const char * str) {}
void error_wait_for_btn() {}

// info
void info_set_message(const char * str) {}
void info_set_title(const char * str) {}
void info_wait_for_btn() {}

