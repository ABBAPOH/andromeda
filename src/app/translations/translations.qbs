import qbs.base 1.0

Product {
    name: "app-translations"
    type: "qm"
    Depends { name: "Qt.core" }
    files: [
        "*.ts"
    ]

    Group {
        fileTagsFilter: product.type
        qbs.install: true
        qbs.installDir: install_data_path + "/translations"
    }
}
