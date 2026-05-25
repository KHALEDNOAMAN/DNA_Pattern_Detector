
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <gtk/gtk.h>
#include <string>
#include <vector>
#include "../FastaParser.h"

/**
 * Holds all GTK widget pointers and application state
 * so callbacks can access them through gpointer user_data.
 */
struct AppWidgets {
    GtkWidget* window;
    GtkWidget* fileEntry;
    GtkWidget* patternEntry;
    GtkWidget* threadSpin;
    GtkWidget* searchButton;
    GtkWidget* resultsView;
    GtkWidget* sequenceView;
    GtkWidget* statusLabel;
    GtkWidget* seqCombo;           // combo box for selecting sequences
    GtkTextBuffer* resultsBuffer;
    GtkTextBuffer* sequenceBuffer;
    std::vector<FastaRecord> records;
};

/**
 * Build and show the main application window.
 * The caller owns the AppWidgets struct.
 */
void create_main_window(AppWidgets* app);

#endif
