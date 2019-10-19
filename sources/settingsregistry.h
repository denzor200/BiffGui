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
};

#endif // SETTINGSREGISTRY_H
