import qbs.base 1.0

Project {
    SubProject {
        filePath: "plugins/parts/parts.qbs"
    }

    references: [
        "app/app.qbs",
        "app/translations/translations.qbs"
    ]
}
