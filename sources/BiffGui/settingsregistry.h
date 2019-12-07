#ifndef SETTINGSREGISTRY_H
#define SETTINGSREGISTRY_H

#include <QString>

namespace SettingsRegistry {
    static const QString DOC_OUTDIR("doc_outdir");

    static const QString PERSONS_INDIR("persons_indir");
    static const QString PERSONS_OUTDIR("persons_outdir");

    static const QString TABLE_INDIR("table_indir");
    static const QString TABLE_OUTDIR("table_outdir");

    static const QString SUBBTITLES_INDIR("subbtitles_indir");
    static const QString SUBBTITLES_OUTDIR("subbtitles_outdir");

    static const QString SUBBTITLES_INDIVIDUAL_OUTDIR("subbtitles_individual_outdir");

    static const QString MAKE_SHARED_FLAG("make_shared_flag");
    static const QString MAKE_INDIVIDUAL_FLAG("make_individual_flag");
    static const QString MAKE_CLEAN_FLAG("make_clean_flag");

    // Configured by user params
    static const QString BUSY_ROLES_FONT("busy_roles_font");
    static const QString BUSY_ROLES_SIZE("busy_roles_size");
    static const QString BUSY_ROLES_COLOR("busy_roles_color");
    static const QString BUSY_ROLES_HIGHLIGHT("busy_roles_highlight");
    static const QString BUSY_ROLES_BOLD("busy_roles_bold");
    static const QString BUSY_ROLES_ITALIC("busy_roles_italic");
    static const QString BUSY_ROLES_UNDERLINE("busy_roles_underline");

};

#endif // SETTINGSREGISTRY_H
