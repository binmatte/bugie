#ifndef BUGIE_H
#define BUGIE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <pthread.h>

// ANSI escape codes for colors
#define BUGIE_ANSI_CODE_RED "\033[1;31m"
#define BUGIE_ANSI_CODE_GREEN "\033[32m"
#define BUGIE_ANSI_CODE_YELLOW "\033[1;33m"
#define BUGIE_ANSI_CODE_BLUE "\033[34m"
#define BUGIE_ANSI_CODE_RESET "\033[0m"

// Log level enumeration
typedef enum BUGIE_LOG_LEVEL {
    BUGIE_LOG_LEVEL_DEBUG,
    BUGIE_LOG_LEVEL_INFO,
    BUGIE_LOG_LEVEL_WARNING,
    BUGIE_LOG_LEVEL_ERROR,
    BUGIE_LOG_LEVEL_NONE
} BUGIE_LOG_LEVEL;

// Log struct
typedef struct BUGIE_LOG {
    FILE * stream;
    BUGIE_LOG_LEVEL level;
    const char * context;
    char * message;
} BUGIE_LOG;

// Global variables for log settings
static FILE * bugie_log_output = NULL;
static pthread_mutex_t bugie_log_mutex = PTHREAD_MUTEX_INITIALIZER;

// Function prototypes
void bugieInit(void);
void bugieClean(void);
void bugieSetLogOutput(FILE * stream);
void bugieSetLogOutputPath(const char * filepath);
char * bugieGetTimeStamp(void);
void bugieLog(const BUGIE_LOG * log);
void bugieLogMessage(FILE *stream, BUGIE_LOG_LEVEL level, const char *context, const char *format, ...);

/**
 * @brief Initializes the log system.
 */
void bugieInit(void) {
    // Set the default output stream to stdout
    bugie_log_output = stdout;
}

/**
 * @brief Cleans up the log system.
 */
void bugieClean(void) {
    // Close the log output stream if it's not stdout or stderr
    if (bugie_log_output && bugie_log_output != stdout && bugie_log_output != stderr) {
        fclose(bugie_log_output);
    }
}

/**
 * @brief Sets the log output stream.
 *
 * @param stream The file stream for log output.
 */
void bugieSetLogOutput(FILE * stream) {
    // Close the previous output stream if it's not stdout or stderr
    if (bugie_log_output && bugie_log_output != stdout && bugie_log_output != stderr) {
        fclose(bugie_log_output);
    }

    // Set the new output stream
    bugie_log_output = stream;
}

/**
 * @brief Sets the log output to a file path.
 *
 * @param filepath The file path for log output.
 */
void bugieSetLogOutputPath(const char * filepath) {
    // Close the previous output stream if it's not stdout or stderr
    if (bugie_log_output && bugie_log_output != stdout && bugie_log_output != stderr) {
        fclose(bugie_log_output);
    }

    // Open the file for appending
    FILE * file = fopen(filepath, "a");

    // Set the output stream if the file was opened successfully
    if (file) {
        bugie_log_output = file;
    } else {
        // Print an error message to stderr if the file couldn't be opened
        fprintf(stderr, "failed to open log file %s\n", filepath);
    }
}

/**
 * @brief Gets the current timestamp in ISO 8601 format.
 *
 * @return A dynamically allocated string containing the timestamp.
 */
char * bugieGetTimeStamp(void) {
    time_t raw_time;
    struct tm * time_info;
    char * timestamp = malloc(21); // YYYY-MM-DDTHH:MM:SSZ + '\0'

    // Handle memory allocation error
    if (timestamp == NULL) {
        fprintf(stderr, "failed to allocate memory for timestamp\n");
        return NULL;
    }

    // Get the current time
    time(&raw_time);
    time_info = gmtime(&raw_time);

    // Handle time conversion error
    if (time_info == NULL) {
        free(timestamp);
        return NULL;
    }

    // Format the timestamp
    strftime(timestamp, 21, "%Y-%m-%dT%H:%M:%SZ", time_info);
    return timestamp;
}

/**
 * @brief Logs a message with a specified log level.
 *
 * @param log A BUGIE_LOG struct containing log details.
 */
void bugieLog(const BUGIE_LOG * log) {
    // Acquire the mutex to prevent race conditions
    pthread_mutex_lock(&bugie_log_mutex);
    FILE * stream = log->stream ? log->stream : bugie_log_output;

    // Determine color code and level string based on log level
    const char * color_code;
    const char * level_string;
    switch (log->level) {
        case BUGIE_LOG_LEVEL_DEBUG:
            color_code = BUGIE_ANSI_CODE_BLUE;
            level_string = "DEBUG";
            break;
        case BUGIE_LOG_LEVEL_INFO:
            color_code = BUGIE_ANSI_CODE_GREEN;
            level_string = "INFO";
            break;
        case BUGIE_LOG_LEVEL_WARNING:
            color_code = BUGIE_ANSI_CODE_YELLOW;
            level_string = "WARNING";
            break;
        case BUGIE_LOG_LEVEL_ERROR:
            color_code = BUGIE_ANSI_CODE_RED;
            level_string = "ERROR";
            break;
        default:
            color_code = BUGIE_ANSI_CODE_RESET;
            level_string = "UNKNOWN";
            break;
    }

    // Get the timestamp
    char * timestamp = bugieGetTimeStamp();
    if (timestamp) {
        // Log the message with timestamp, level, context, and message
        fprintf(stream, "%s[%s][%s] [%s] - %s%s\n", color_code, timestamp, level_string, log->context ? log->context : "", log->message, BUGIE_ANSI_CODE_RESET);
        free(timestamp);
    } else {
        // Log the message without timestamp if it couldn't be obtained
        fprintf(stream, "%s[UNKNOWN TIME][%s] [%s] - %s%s\n", color_code, level_string, log->context ? log->context : "", log->message, BUGIE_ANSI_CODE_RESET);
    }

    // Release the mutex
    pthread_mutex_unlock(&bugie_log_mutex);
}

/**
 * @brief Logs a message with a specified log level, context, and format string.
 *
 * @param stream The file stream for log output.
 * @param level The log level.
 * @param context The log context.
 * @param format The format string for the message.
 * @param ... Additional arguments for the format string.
 */
void bugieLogMessage(FILE *stream, BUGIE_LOG_LEVEL level, const char *context, const char *format, ...) {
    // Initialize a variable-argument list
    va_list args;
    va_start(args, format);

    // Create a BUGIE_LOG struct
    BUGIE_LOG log = {
            .stream = stream,
            .level = level,
            .context = context,
            .message = malloc(1024)
    };

    // Format the message using vsnprintf
    vsnprintf(log.message, 1024, format, args);

    // Clean up the variable-argument list
    va_end(args);

    // Log the message
    bugieLog(&log);

    // Free the allocated memory for the message
    free(log.message);
}

#endif //BUGIE_H