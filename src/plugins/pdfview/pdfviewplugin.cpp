/*
 * Copyright (C) 2012, Glad Deschrijver <glad.deschrijver@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include "pdfviewplugin.h"

#include <QtCore/QtPlugin>
#include <GuiSystem/DocumentManager>
#include <GuiSystem/EditorManager>

#include "pdfviewdocument.h"
#include "pdfvieweditor.h"

using namespace GuiSystem;
//using namespace PdfView;

PdfViewPlugin::PdfViewPlugin() :
    ExtensionSystem::IPlugin()
{
}

bool PdfViewPlugin::initialize()
{
    DocumentManager::instance()->addFactory(new PdfViewDocumentFactory(this));
    EditorManager::instance()->addFactory(new PdfViewEditorFactory(this));

    return true;
}

Q_EXPORT_PLUGIN(PdfViewPlugin)
