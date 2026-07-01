#pragma once

#include "packagemanageradapter.h"

// Homebrew adapter. Prefers `--json=v2` everywhere; text parsing only for
// `brew search`, which has no JSON output.
class BrewAdapter : public yas::PackageManagerAdapter {
    Q_OBJECT
public:
    using yas::PackageManagerAdapter::PackageManagerAdapter;

    QString displayName() const override { return QStringLiteral("Homebrew"); }
    QString cliProgram() const override { return QStringLiteral("brew"); }
    QStringList cliSearchPaths() const override;

    yas::CliCommand searchCommand(const QString &query) const override;
    yas::CliCommand infoCommand(const QString &packageId, const QString &kind) const override;
    yas::CliCommand listInstalledCommand() const override;
    yas::CliCommand listOutdatedCommand() const override;
    yas::CliCommand installCommand(const QString &packageId, const QString &kind) const override;
    yas::CliCommand uninstallCommand(const QString &packageId, const QString &kind) const override;
    yas::CliCommand upgradeCommand(const QString &packageId, const QString &kind) const override;
    yas::CliCommand upgradeAllCommand() const override;
    yas::CliCommand pinCommand(const QString &packageId, const QString &kind) const override;
    yas::CliCommand unpinCommand(const QString &packageId, const QString &kind) const override;

    QList<yas::Package> parseSearch(const QString &stdOut) const override;
    QList<yas::Package> parseInfo(const QString &stdOut) const override;
    QList<yas::Package> parseInstalled(const QString &stdOut) const override;
    QList<yas::Package> parseOutdated(const QString &stdOut) const override;

    QList<yas::CliAction> actionCatalog() const override;

private:
    static QStringList kindArgs(const QString &kind);
    static QList<yas::Package> parseInfoJson(const QString &stdOut);
};
