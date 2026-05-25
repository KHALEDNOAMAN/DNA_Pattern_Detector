
#include "MainWindow.h"
#include "../DNAPatternDetector.h"
#include <chrono>
#include <sstream>
#include <algorithm>
#include <thread>

/* ------------------------------------------------------------------ */
/*  Dark-theme CSS  (Tokyo Night inspired)                            */
/* ------------------------------------------------------------------ */
static const char* APP_CSS = R"CSS(

/* ---- window ---- */
window {
    background-color: #1a1b26;
}

/* ---- header bar ---- */
headerbar {
    background: linear-gradient(135deg, #1a1b26 0%, #24283b 100%);
    border-bottom: 1px solid #414868;
    padding: 4px 8px;
}
headerbar .title {
    color: #c0caf5;
    font-weight: bold;
    font-size: 15px;
}
headerbar .subtitle {
    color: #565f89;
    font-size: 11px;
}

/* ---- custom CSS classes ---- */
.section-frame {
    background-color: #24283b;
    border-radius: 10px;
    border: 1px solid #414868;
    padding: 14px;
}

.section-label {
    color: #7aa2f7;
    font-weight: bold;
    font-size: 13px;
}

.field-label {
    color: #a9b1d6;
    font-size: 12px;
}

.dark-entry {
    background-color: #1a1b26;
    color: #c0caf5;
    border: 1px solid #414868;
    border-radius: 6px;
    padding: 6px 10px;
    font-family: monospace;
    font-size: 13px;
}
.dark-entry:focus {
    border-color: #7aa2f7;
}

.browse-btn {
    background: linear-gradient(135deg, #7aa2f7 0%, #7dcfff 100%);
    color: #1a1b26;
    border-radius: 6px;
    font-weight: bold;
    padding: 6px 18px;
    border: none;
}
.browse-btn:hover {
    background: linear-gradient(135deg, #7dcfff 0%, #7aa2f7 100%);
}

.search-btn {
    background: linear-gradient(135deg, #9ece6a 0%, #73daca 100%);
    color: #1a1b26;
    border-radius: 8px;
    font-weight: bold;
    font-size: 14px;
    padding: 10px 32px;
    border: none;
}
.search-btn:hover {
    background: linear-gradient(135deg, #73daca 0%, #9ece6a 100%);
}

.results-text {
    background-color: #1a1b26;
    color: #c0caf5;
    font-family: monospace;
    font-size: 12px;
}

.status-bar {
    background-color: #24283b;
    color: #565f89;
    border-top: 1px solid #414868;
    padding: 4px 14px;
    font-size: 11px;
}

spinbutton {
    background-color: #1a1b26;
    color: #c0caf5;
    border: 1px solid #414868;
    border-radius: 6px;
}
spinbutton:focus {
    border-color: #7aa2f7;
}

combobox button {
    background-color: #1a1b26;
    color: #c0caf5;
    border: 1px solid #414868;
    border-radius: 6px;
}

scrolledwindow {
    border-radius: 6px;
    border: 1px solid #414868;
}

)CSS";

/* ------------------------------------------------------------------ */
/*  Helper: apply a CSS class to a widget                              */
/* ------------------------------------------------------------------ */
static void add_css_class(GtkWidget* widget, const char* className) {
    GtkStyleContext* ctx = gtk_widget_get_style_context(widget);
    gtk_style_context_add_class(ctx, className);
}

/* ------------------------------------------------------------------ */
/*  Helper: create a label with a CSS class                            */
/* ------------------------------------------------------------------ */
static GtkWidget* make_label(const char* text, const char* cssClass,
                             GtkAlign align = GTK_ALIGN_START) {
    GtkWidget* label = gtk_label_new(text);
    gtk_widget_set_halign(label, align);
    if (cssClass) add_css_class(label, cssClass);
    return label;
}

/* ------------------------------------------------------------------ */
/*  Callback: Browse for file                                          */
/* ------------------------------------------------------------------ */
static void on_browse_clicked(GtkWidget* /*widget*/, gpointer data) {
    AppWidgets* app = static_cast<AppWidgets*>(data);

    GtkWidget* dialog = gtk_file_chooser_dialog_new(
        "Open DNA / FASTA File",
        GTK_WINDOW(app->window),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open",   GTK_RESPONSE_ACCEPT,
        NULL);

    /* File filters */
    GtkFileFilter* fastaFilter = gtk_file_filter_new();
    gtk_file_filter_set_name(fastaFilter, "FASTA files (*.fasta, *.fa)");
    gtk_file_filter_add_pattern(fastaFilter, "*.fasta");
    gtk_file_filter_add_pattern(fastaFilter, "*.fa");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), fastaFilter);

    GtkFileFilter* txtFilter = gtk_file_filter_new();
    gtk_file_filter_set_name(txtFilter, "Text files (*.txt)");
    gtk_file_filter_add_pattern(txtFilter, "*.txt");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), txtFilter);

    GtkFileFilter* allFilter = gtk_file_filter_new();
    gtk_file_filter_set_name(allFilter, "All files (*)");
    gtk_file_filter_add_pattern(allFilter, "*");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), allFilter);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        gtk_entry_set_text(GTK_ENTRY(app->fileEntry), filename);

        /* Parse immediately so we can populate the sequence combo */
        try {
            app->records = FastaParser::parse(std::string(filename));

            /* Populate combo box */
            gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(app->seqCombo));
            for (size_t i = 0; i < app->records.size(); i++) {
                std::string label = "Seq " + std::to_string(i + 1) + ": "
                                  + app->records[i].header
                                  + " (" + std::to_string(app->records[i].sequence.size())
                                  + " bp)";
                gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(app->seqCombo),
                                               label.c_str());
            }
            if (!app->records.empty()) {
                gtk_combo_box_set_active(GTK_COMBO_BOX(app->seqCombo), 0);
            }

            /* Update status */
            std::string status = "Loaded " + std::to_string(app->records.size())
                               + " sequence(s) from " + std::string(filename);
            gtk_label_set_text(GTK_LABEL(app->statusLabel), status.c_str());

        } catch (const std::exception& e) {
            gtk_label_set_text(GTK_LABEL(app->statusLabel), e.what());
        }

        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

/* ------------------------------------------------------------------ */
/*  Callback: Run search                                               */
/* ------------------------------------------------------------------ */
static void on_search_clicked(GtkWidget* /*widget*/, gpointer data) {
    AppWidgets* app = static_cast<AppWidgets*>(data);

    if (app->records.empty()) {
        gtk_label_set_text(GTK_LABEL(app->statusLabel),
                           "⚠  No file loaded. Please browse for a DNA/FASTA file first.");
        return;
    }

    const char* patternText = gtk_entry_get_text(GTK_ENTRY(app->patternEntry));
    std::string pattern(patternText);

    if (pattern.empty()) {
        gtk_label_set_text(GTK_LABEL(app->statusLabel),
                           "⚠  Please enter a pattern to search for.");
        return;
    }

    /* Convert pattern to uppercase */
    std::transform(pattern.begin(), pattern.end(), pattern.begin(),
                   [](unsigned char c) { return std::toupper(c); });

    int threads = gtk_spin_button_get_value_as_int(
                      GTK_SPIN_BUTTON(app->threadSpin));

    /* --- Run search on all sequences --- */
    std::ostringstream out;
    out << "═══════════════════════════════════════════\n";
    out << "  DNA Pattern Detector — Results (OpenMP)\n";
    out << "═══════════════════════════════════════════\n";
    out << "  Pattern:   " << pattern << "\n";
    out << "  Threads:   " << threads << "\n";
    out << "  Sequences: " << app->records.size() << "\n";
    out << "═══════════════════════════════════════════\n\n";

    auto totalStart = std::chrono::high_resolution_clock::now();
    int totalMatches = 0;

    for (size_t s = 0; s < app->records.size(); s++) {
        const auto& rec = app->records[s];

        out << "── Sequence " << (s + 1) << ": " << rec.header
            << " (" << rec.sequence.size() << " bp) ──\n";

        DNAPatternDetector detector(rec.sequence, pattern, threads);

        auto start   = std::chrono::high_resolution_clock::now();
        std::vector<int> matches = detector.searchPattern();
        auto end     = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<
                           std::chrono::microseconds>(end - start);

        out << "  Matches: " << matches.size() << "\n";
        out << "  Time:    " << (elapsed.count() / 1000.0) << " ms\n";

        if (!matches.empty()) {
            out << "  Positions: ";
            for (size_t i = 0; i < matches.size(); i++) {
                out << matches[i];
                if (i < matches.size() - 1) out << ", ";
            }
            out << "\n";
        }
        out << "\n";

        totalMatches += static_cast<int>(matches.size());
    }

    auto totalEnd = std::chrono::high_resolution_clock::now();
    auto totalDuration = std::chrono::duration_cast<
                             std::chrono::microseconds>(totalEnd - totalStart);

    out << "═══════════════════════════════════════════\n";
    out << "  TOTAL MATCHES: " << totalMatches << "\n";
    out << "  TOTAL TIME:    " << (totalDuration.count() / 1000.0) << " ms\n";
    out << "═══════════════════════════════════════════\n";

    /* Display results */
    gtk_text_buffer_set_text(app->resultsBuffer, out.str().c_str(), -1);

    /* Show selected sequence in the preview */
    int active = gtk_combo_box_get_active(GTK_COMBO_BOX(app->seqCombo));
    if (active >= 0 && active < static_cast<int>(app->records.size())) {
        const std::string& seq = app->records[active].sequence;
        /* Insert sequence with line wrapping at 80 chars */
        std::string formatted;
        formatted.reserve(seq.size() + seq.size() / 80 + 1);
        for (size_t i = 0; i < seq.size(); i++) {
            formatted += seq[i];
            if ((i + 1) % 80 == 0) formatted += '\n';
        }
        gtk_text_buffer_set_text(app->sequenceBuffer,
                                 formatted.c_str(), -1);
    }

    /* Status */
    std::string statusMsg = "✓ Search complete — "
                          + std::to_string(totalMatches) + " match(es) in "
                          + std::to_string(totalDuration.count() / 1000.0)
                          + " ms";
    /* Truncate to avoid very long float */
    gtk_label_set_text(GTK_LABEL(app->statusLabel), statusMsg.c_str());
}

/* ------------------------------------------------------------------ */
/*  Build the main window                                              */
/* ------------------------------------------------------------------ */
void create_main_window(AppWidgets* app) {

    /* --- Load CSS --- */
    GtkCssProvider* cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(cssProvider, APP_CSS, -1, NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(cssProvider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(cssProvider);

    /* --- Window --- */
    app->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(app->window), "DNA Pattern Detector");
    gtk_window_set_default_size(GTK_WINDOW(app->window), 900, 700);
    g_signal_connect(app->window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    /* --- Header bar --- */
    GtkWidget* headerBar = gtk_header_bar_new();
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(headerBar), TRUE);
    gtk_header_bar_set_title(GTK_HEADER_BAR(headerBar), "🧬 DNA Pattern Detector");
    gtk_header_bar_set_subtitle(GTK_HEADER_BAR(headerBar),
                                "FASTA Parser  •  OpenMP Parallel Search");
    gtk_window_set_titlebar(GTK_WINDOW(app->window), headerBar);

    /* --- Main vertical box --- */
    GtkWidget* mainBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_container_set_border_width(GTK_CONTAINER(mainBox), 16);
    gtk_container_add(GTK_CONTAINER(app->window), mainBox);

    /* ====== INPUT SECTION ====== */
    GtkWidget* inputFrame = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    add_css_class(inputFrame, "section-frame");

    gtk_box_pack_start(GTK_BOX(inputFrame),
        make_label("📂  Input", "section-label"), FALSE, FALSE, 0);

    /* -- File row -- */
    GtkWidget* fileBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_box_pack_start(GTK_BOX(fileBox),
        make_label("File:", "field-label"), FALSE, FALSE, 0);

    app->fileEntry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->fileEntry),
                                   "Browse for a .fasta or .txt file…");
    add_css_class(app->fileEntry, "dark-entry");
    gtk_widget_set_hexpand(app->fileEntry, TRUE);
    gtk_box_pack_start(GTK_BOX(fileBox), app->fileEntry, TRUE, TRUE, 0);

    GtkWidget* browseBtn = gtk_button_new_with_label("Browse");
    add_css_class(browseBtn, "browse-btn");
    g_signal_connect(browseBtn, "clicked", G_CALLBACK(on_browse_clicked), app);
    gtk_box_pack_start(GTK_BOX(fileBox), browseBtn, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(inputFrame), fileBox, FALSE, FALSE, 0);

    /* -- Pattern + Threads row -- */
    GtkWidget* paramBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);

    /* Pattern */
    GtkWidget* patBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_box_pack_start(GTK_BOX(patBox),
        make_label("Pattern:", "field-label"), FALSE, FALSE, 0);

    app->patternEntry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->patternEntry), "e.g. GATTACA");
    add_css_class(app->patternEntry, "dark-entry");
    gtk_entry_set_width_chars(GTK_ENTRY(app->patternEntry), 24);
    gtk_box_pack_start(GTK_BOX(patBox), app->patternEntry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(paramBox), patBox, TRUE, TRUE, 0);

    /* Threads */
    GtkWidget* threadBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_box_pack_start(GTK_BOX(threadBox),
        make_label("Threads:", "field-label"), FALSE, FALSE, 0);

    int hwThreads = static_cast<int>(std::thread::hardware_concurrency());
    if (hwThreads < 1) hwThreads = 4;
    GtkAdjustment* adj = gtk_adjustment_new(hwThreads, 1, 128, 1, 4, 0);
    app->threadSpin = gtk_spin_button_new(adj, 1, 0);
    gtk_box_pack_start(GTK_BOX(threadBox), app->threadSpin, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(paramBox), threadBox, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(inputFrame), paramBox, FALSE, FALSE, 0);

    /* -- Sequence selector combo -- */
    GtkWidget* seqBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_box_pack_start(GTK_BOX(seqBox),
        make_label("Sequence:", "field-label"), FALSE, FALSE, 0);

    app->seqCombo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(app->seqCombo),
                                   "(load a file first)");
    gtk_combo_box_set_active(GTK_COMBO_BOX(app->seqCombo), 0);
    gtk_widget_set_hexpand(app->seqCombo, TRUE);
    gtk_box_pack_start(GTK_BOX(seqBox), app->seqCombo, TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(inputFrame), seqBox, FALSE, FALSE, 0);

    /* -- Search button (centered) -- */
    GtkWidget* btnAlign = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(btnAlign, GTK_ALIGN_CENTER);

    app->searchButton = gtk_button_new_with_label("🔍  Search Pattern");
    add_css_class(app->searchButton, "search-btn");
    g_signal_connect(app->searchButton, "clicked",
                     G_CALLBACK(on_search_clicked), app);
    gtk_box_pack_start(GTK_BOX(btnAlign), app->searchButton, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(inputFrame), btnAlign, FALSE, FALSE, 4);

    gtk_box_pack_start(GTK_BOX(mainBox), inputFrame, FALSE, FALSE, 0);

    /* ====== RESULTS SECTION ====== */
    GtkWidget* resultFrame = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    add_css_class(resultFrame, "section-frame");

    gtk_box_pack_start(GTK_BOX(resultFrame),
        make_label("📊  Results", "section-label"), FALSE, FALSE, 0);

    /* Paned: left = results text, right = sequence preview */
    GtkWidget* paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_paned_set_position(GTK_PANED(paned), 480);

    /* -- Results text view -- */
    GtkWidget* resScroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(resScroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    app->resultsView = gtk_text_view_new();
    app->resultsBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(app->resultsView));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(app->resultsView), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(app->resultsView), FALSE);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(app->resultsView), 8);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(app->resultsView), 8);
    add_css_class(app->resultsView, "results-text");
    gtk_text_buffer_set_text(app->resultsBuffer,
        "  Results will appear here after searching.\n\n"
        "  1. Browse for a FASTA or DNA text file\n"
        "  2. Enter a pattern (e.g. GATTACA)\n"
        "  3. Click \"Search Pattern\"\n", -1);
    gtk_container_add(GTK_CONTAINER(resScroll), app->resultsView);
    gtk_paned_pack1(GTK_PANED(paned), resScroll, TRUE, TRUE);

    /* -- Sequence preview -- */
    GtkWidget* seqScroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(seqScroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    GtkWidget* seqLabel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_box_pack_start(GTK_BOX(seqLabel),
        make_label("Sequence Preview", "field-label"), FALSE, FALSE, 2);

    app->sequenceView = gtk_text_view_new();
    app->sequenceBuffer = gtk_text_view_get_buffer(
                              GTK_TEXT_VIEW(app->sequenceView));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(app->sequenceView), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(app->sequenceView), FALSE);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(app->sequenceView), 8);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(app->sequenceView), 8);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(app->sequenceView),
                                GTK_WRAP_CHAR);
    add_css_class(app->sequenceView, "results-text");
    gtk_text_buffer_set_text(app->sequenceBuffer,
        "Sequence data will appear here\nafter loading a file.", -1);
    gtk_container_add(GTK_CONTAINER(seqScroll), app->sequenceView);

    gtk_box_pack_start(GTK_BOX(seqLabel), seqScroll, TRUE, TRUE, 0);
    gtk_paned_pack2(GTK_PANED(paned), seqLabel, TRUE, TRUE);

    gtk_box_pack_start(GTK_BOX(resultFrame), paned, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(mainBox), resultFrame, TRUE, TRUE, 0);

    /* ====== STATUS BAR ====== */
    app->statusLabel = gtk_label_new("Ready — load a DNA file to begin");
    gtk_widget_set_halign(app->statusLabel, GTK_ALIGN_START);
    add_css_class(app->statusLabel, "status-bar");
    gtk_box_pack_start(GTK_BOX(mainBox), app->statusLabel, FALSE, FALSE, 0);
}
