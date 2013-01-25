function Component()
{
    // constructor
    installer.setDefaultPageVisible(QInstaller.ComponentSelection, false);

    var programFiles = installer.environmentVariable("ProgramFiles(x86)");
    if (programFiles == "")
        programFiles = installer.environmentVariable("ProgramFiles");
    if (programFiles == "")
        programFiles = installer.value("rootDir") + "/" + "Program Files";

    installer.setValue("TargetDir", programFiles + "/andromeda");
}

Component.prototype.isDefault = function()
{
    // select the component by default
    return true;
}

Component.prototype.createOperations = function()
{
    try {
        // call the base create operations function
        component.createOperations();
        if ( installer.value("os") == "win" ) {
            component.addOperation( "CreateShortcut",
                                   "@TargetDir@\\bin\\andromeda.exe",
                                   "@StartMenuDir@/Andromeda.lnk",
                                   "workingDirectory=@homeDir@" );
            component.addOperation( "CreateShortcut",
                                   "@TargetDir@\\uninstall.exe",
                                   "@StartMenuDir@/Uninstall.lnk",
                                   "workingDirectory=@homeDir@" );
        }
    } catch (e) {
        print(e);
    }
}
