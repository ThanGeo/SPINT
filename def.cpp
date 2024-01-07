#include "def.h"

/* ERROR LOGGING */
void LOG_ERR(std::string error_text, uint32_t error_code) {
    error_text = "*** " + error_text;
    error_text += " code: %d\n";
    fprintf(stderr, error_text.c_str(), error_code);
}