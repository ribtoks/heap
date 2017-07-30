# ministaller
Lightweight installer/updater for desktop Qt application

![alt tag](https://raw.githubusercontent.com/Ribtoks/ministaller/master/scrot.png)

Windows build status: [![Build status](https://ci.appveyor.com/api/projects/status/0r932de83dibgao6/branch/master?svg=true)](https://ci.appveyor.com/project/Ribtoks/ministaller/branch/master)

There're two tools available: **minipkgen** and **ministaller**.

### minipkgen

This is a command-line tool used for creating a 'diff' package config.

    Options:
      -b, --base-package <directory>  Path to the base package dir
      -n, --new-package <directory>   Path to the new package dir
      -o, --output <filepath>         Path to the result package file
      -f, --force-update              Don't skip same files
      -k, --keep-missing              Do not remove missing files in new package
      -v, --verbose                   Be verbose

New package is the update or newer version of same software and base package is the currently available version of software.

Typical usage is either offline usage or server-side when client request an update from version X to version Y and server generates an update package from only items needed for upgrade.

`./minipkgen --verbose -b ../test_base -n ../test_new` will create a json file with three self-explanatory lists _add_, _remove_, _update_ with objects containing _path_ and _sha1_ keys. Paths are relative either to `base dir` or to the `new dir` dependingly on the context (are they files to add, remove or update). Example of the config:

    {
        "add": [
            {
                "path": "dir2/Makefile",
                "sha1": "f4217be269bec91780fe27cca6f207c15051d007"
            }
        ],
        "remove": [
            {
                "path": "dir1/diffgenerator.h",
                "sha1": "42d268d568da9cacb5e8cef4a4354a39502841c6"
            }
        ],
        "update": [
        ]
    }

### ministaller

This is the main installer UI application which executes an update config created by **minipkgen** or generates virtual config on the fly

    Options:
      -u, --update-config <filepath>  Path to the update config if available
      -i, --install-path <directory>  Path to the installed package
      -p, --package-path <filepath>   Path to the package to install
      -w, --wait-pid <number>         Pid of the process to wait for
      -f, --force-update              Don't skip same files
      -k, --keep-missing              Do not remove missing files in new package
      -x, --launch-exe <filepath>     Relative path to executable to launch after installation
      -s, --skip-dirs                 Drill down through 1 dir directories
      -?, -h, --help                  Displays this help.

This software is capable of waiting for an application to exit (the one which has launched the installer) and launching an updated application on successful exit. Ministaller can gradually fail restoring all the previous files in case something went wrong on install.

Also there's _skip-dirs_ parameter which is helpful in case you used archiever which generated directory with only one directory with your software. In this case _ministaller_ will find first useful directory if this option is specified.

To update the updater you will need to invent some logic from your main application.