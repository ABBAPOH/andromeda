import qbs.base 1.0

Project {
    references: [
        "bineditorpart/bineditorpart.qbs",
        "bookmarkspart/bookmarkspart.qbs",
        "filemanagerpart/filemanagerpart.qbs",
        "helloworldpart/helloworldpart.qbs",
        "imageviewerpart/imageviewerpart.qbs",
        "texteditorpart/texteditorpart.qbs",
        "webviewpart/webviewpart.qbs"
    ]

    Product {
        name: "parts-translations"
        type: "qm"
        Depends { name: "Qt.core" }
        files: [
            "bineditorpart/translations/*.ts",
            "bookmarkspart/translations/*.ts",
            "filemanagerpart/translations/*.ts",
            "helloworldpart/translations/*.ts",
            "imageviewerpart/translations/*.ts",
            "texteditorpart/translations/*.ts",
            "webviewpart/translations/*.ts"
        ]

        Group {
            fileTagsFilter: product.type
            qbs.install: true
            qbs.installDir: install_data_path + "/translations"
        }
    }
}
