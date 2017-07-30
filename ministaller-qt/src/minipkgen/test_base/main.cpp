/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#include <iostream>
#include <QDir>
#include <QCommandLineParser>
#include <QFile>
#include <QJsonDocument>
#include "diffgenerator.h"

#define DEFAULT_OUTPUT_NAME QLatin1String("pkg.json")

enum CommandLineParseResult {
    CommandLineOk,
    CommandLineError,
    CommandLineHelpRequested
};

struct ParsedOptions {
    QString m_BaseDir;
    QString m_NewDir;
    QString m_JsonPath;
};

CommandLineParseResult parseCommandLine(QCommandLineParser &parser, const QStringList &arguments, ParsedOptions &options) {
    QCommandLineOption basePackageOption(QStringList() << "b" << "base-package",
                                        "Path to the base package dir",
                                        "directory");
    parser.addOption(basePackageOption);

    QCommandLineOption newPackageOption(QStringList() << "n" << "new-package",
                                        "Path to the new package dir",
                                        "directory");
    parser.addOption(newPackageOption);

    QCommandLineOption outputOption(QStringList() << "o" << "output",
                                        "Path to the result package file",
                                        "filepath");
    parser.addOption(outputOption);

    const QCommandLineOption helpOption = parser.addHelpOption();

    CommandLineParseResult result = CommandLineError;

    do {
        if (!parser.parse(arguments)) {
            std::cerr << parser.errorText().toStdString();
            break;
        }

        if (parser.isSet(helpOption)) {
            result = CommandLineHelpRequested;
            break;
        }

        if (parser.isSet(basePackageOption)) {
            const QString baseDir = parser.value(basePackageOption);
            if (!QDir(baseDir).exists()) {
                std::cerr << "Directory not found: " << baseDir.toStdString();
                break;
            } else {
                options.m_BaseDir = baseDir;
            }
        } else {
            std::cerr << "Base package path is missing" << std::endl;
            break;
        }

        if (parser.isSet(newPackageOption)) {
            const QString newDir = parser.value(newPackageOption);
            if (!QDir(newDir).exists()) {
                std::cerr << "Directory not found: " << newDir.toStdString();
                break;
            } else {
                options.m_NewDir = newDir;
            }
        } else {
            std::cerr << "New package path is missing" << std::endl;
            break;
        }

        if (parser.isSet(outputOption)) {
            options.m_JsonPath = parser.value(outputOption);
        } else {
            options.m_JsonPath = DEFAULT_OUTPUT_NAME;
        }
    } while (false);

    return result;
}

bool saveJson(const QString &filename, const QJsonDocument &document) {
    bool success = false;
    QFile outputFile(filename);

    if (outputFile.open(QIODevice::WriteOnly)) {
        outputFile.write(document.toJson(QJsonDocument::Indented));
        success = true;
    }

    return success;
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    QCommandLineParser optionsParser;
    optionsParser.setApplicationDescription("Tool for generating pkg file for ministaller");
    ParsedOptions options;

    switch (parseCommandLine(optionsParser, app.arguments(), options)) {
    case CommandLineOk:
        break;
    case CommandLineError:
        std::cout << optionsParser.helpText().toStdString() << std::endl;
        return 1;
    case CommandLineHelpRequested:
        std::cout << optionsParser.helpText().toStdString() << std::endl;
        return 0;
    }

    DiffGenerator diffGenerator(options.m_BaseDir, options.m_NewDir);

    diffGenerator.generateDiffs();

    std::cout << "Done" << std::endl;

    auto json = diffGenerator.generateJson();

    if (!saveJson(options.m_JsonPath, json)) {
        if (!saveJson(DEFAULT_OUTPUT_NAME, json)) {
            std::cerr << "Failed to save json to file" << std::endl;
            return 1;
        }
    }

    return 0;
}
