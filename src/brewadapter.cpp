#include "brewadapter.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

using yas::CliAction;
using yas::CliCommand;
using yas::Package;

namespace {
const QString kCask = QStringLiteral("cask");
const QString kFormula = QStringLiteral("formula");
} // namespace

QStringList BrewAdapter::cliSearchPaths() const
{
    // GUI apps do not inherit the shell PATH; brew lives here on arm64/intel.
    return {QStringLiteral("/opt/homebrew/bin"), QStringLiteral("/usr/local/bin")};
}

QStringList BrewAdapter::kindArgs(const QString &kind)
{
    return kind == kCask ? QStringList{QStringLiteral("--cask")} : QStringList{};
}

CliCommand BrewAdapter::searchCommand(const QString &query) const
{
    return {QStringLiteral("brew"), {QStringLiteral("search"), query}};
}

CliCommand BrewAdapter::infoCommand(const QString &packageId, const QString &) const
{
    return {QStringLiteral("brew"),
            {QStringLiteral("info"), QStringLiteral("--json=v2"), packageId}};
}

CliCommand BrewAdapter::listInstalledCommand() const
{
    return {QStringLiteral("brew"),
            {QStringLiteral("info"), QStringLiteral("--json=v2"), QStringLiteral("--installed")}};
}

CliCommand BrewAdapter::listOutdatedCommand() const
{
    return {QStringLiteral("brew"),
            {QStringLiteral("outdated"), QStringLiteral("--json=v2")}};
}

CliCommand BrewAdapter::installCommand(const QString &packageId, const QString &kind) const
{
    return {QStringLiteral("brew"),
            QStringList{QStringLiteral("install")} + kindArgs(kind) + QStringList{packageId}};
}

CliCommand BrewAdapter::uninstallCommand(const QString &packageId, const QString &kind) const
{
    return {QStringLiteral("brew"),
            QStringList{QStringLiteral("uninstall")} + kindArgs(kind) + QStringList{packageId}};
}

CliCommand BrewAdapter::upgradeCommand(const QString &packageId, const QString &kind) const
{
    return {QStringLiteral("brew"),
            QStringList{QStringLiteral("upgrade")} + kindArgs(kind) + QStringList{packageId}};
}

CliCommand BrewAdapter::upgradeAllCommand() const
{
    return {QStringLiteral("brew"), {QStringLiteral("upgrade")}};
}

CliCommand BrewAdapter::pinCommand(const QString &packageId, const QString &kind) const
{
    if (kind == kCask)
        return {}; // casks cannot be pinned
    return {QStringLiteral("brew"), {QStringLiteral("pin"), packageId}};
}

CliCommand BrewAdapter::unpinCommand(const QString &packageId, const QString &kind) const
{
    if (kind == kCask)
        return {};
    return {QStringLiteral("brew"), {QStringLiteral("unpin"), packageId}};
}

QList<Package> BrewAdapter::parseSearch(const QString &stdOut) const
{
    // Plain-text sections:
    //   ==> Formulae
    //   name1 name2 ... (one per line in non-tty mode)
    //   ==> Casks
    QList<Package> result;
    QString kind = kFormula;
    const QStringList lines = stdOut.split(QLatin1Char('\n'), Qt::SkipEmptyParts);
    for (const QString &raw : lines) {
        const QString line = raw.trimmed();
        if (line.startsWith(QStringLiteral("==>"))) {
            kind = line.contains(QStringLiteral("Cask"), Qt::CaseInsensitive) ? kCask
                                                                              : kFormula;
            continue;
        }
        if (line.contains(QStringLiteral("Error:")))
            continue;
        Package p;
        p.id = line;
        p.name = line;
        p.kind = kind;
        result.append(p);
    }
    return result;
}

QList<Package> BrewAdapter::parseInfoJson(const QString &stdOut)
{
    QList<Package> result;
    const QJsonObject root = QJsonDocument::fromJson(stdOut.toUtf8()).object();

    const QJsonArray formulae = root.value(QStringLiteral("formulae")).toArray();
    for (const auto &value : formulae) {
        const QJsonObject f = value.toObject();
        Package p;
        p.id = f.value(QStringLiteral("name")).toString();
        p.name = p.id;
        p.kind = kFormula;
        p.description = f.value(QStringLiteral("desc")).toString();
        p.homepage = f.value(QStringLiteral("homepage")).toString();
        p.source = f.value(QStringLiteral("tap")).toString();
        p.pinned = f.value(QStringLiteral("pinned")).toBool();
        p.version = f.value(QStringLiteral("versions")).toObject()
                        .value(QStringLiteral("stable")).toString();
        const QJsonArray installed = f.value(QStringLiteral("installed")).toArray();
        if (!installed.isEmpty()) {
            p.installedVersion = installed.last().toObject()
                                     .value(QStringLiteral("version")).toString();
            // keg-only/HEAD builds append suffixes; trust brew's outdated flag
            if (!f.value(QStringLiteral("outdated")).toBool())
                p.version = p.installedVersion;
        }
        result.append(p);
    }

    const QJsonArray casks = root.value(QStringLiteral("casks")).toArray();
    for (const auto &value : casks) {
        const QJsonObject c = value.toObject();
        Package p;
        p.id = c.value(QStringLiteral("token")).toString();
        p.name = c.value(QStringLiteral("name")).toArray().isEmpty()
                     ? p.id
                     : c.value(QStringLiteral("name")).toArray().first().toString();
        p.kind = kCask;
        p.description = c.value(QStringLiteral("desc")).toString();
        p.homepage = c.value(QStringLiteral("homepage")).toString();
        p.source = c.value(QStringLiteral("tap")).toString();
        p.version = c.value(QStringLiteral("version")).toString();
        const QString installed = c.value(QStringLiteral("installed")).toString();
        if (!installed.isEmpty()) {
            p.installedVersion = installed;
            if (!c.value(QStringLiteral("outdated")).toBool())
                p.version = installed;
        }
        result.append(p);
    }
    return result;
}

QList<Package> BrewAdapter::parseInfo(const QString &stdOut) const
{
    return parseInfoJson(stdOut);
}

QList<Package> BrewAdapter::parseInstalled(const QString &stdOut) const
{
    return parseInfoJson(stdOut);
}

QList<Package> BrewAdapter::parseOutdated(const QString &stdOut) const
{
    QList<Package> result;
    const QJsonObject root = QJsonDocument::fromJson(stdOut.toUtf8()).object();

    const auto parseArray = [&result](const QJsonArray &array, const QString &kind) {
        for (const auto &value : array) {
            const QJsonObject o = value.toObject();
            Package p;
            p.id = o.value(QStringLiteral("name")).toString();
            p.name = p.id;
            p.kind = kind;
            p.version = o.value(QStringLiteral("current_version")).toString();
            p.pinned = o.value(QStringLiteral("pinned")).toBool();
            const QJsonArray versions = o.value(QStringLiteral("installed_versions")).toArray();
            if (!versions.isEmpty())
                p.installedVersion = versions.last().toString();
            result.append(p);
        }
    };
    parseArray(root.value(QStringLiteral("formulae")).toArray(), kFormula);
    parseArray(root.value(QStringLiteral("casks")).toArray(), kCask);
    return result;
}

QList<CliAction> BrewAdapter::actionCatalog() const
{
    const QString brew = QStringLiteral("brew");
    return {
        {QStringLiteral("update"), tr("Update metadata"),
         tr("Fetch the newest package lists from all taps"),
         {brew, {QStringLiteral("update")}}, false, false, true},
        {QStringLiteral("doctor"), tr("Run diagnostics"),
         tr("Check your system for potential problems"),
         {brew, {QStringLiteral("doctor")}}, false, false, false},
        {QStringLiteral("cleanup"), tr("Clean cache"),
         tr("Remove stale lock files, outdated downloads and old versions"),
         {brew, {QStringLiteral("cleanup"), QStringLiteral("--prune=all")}}, false, true, false},
        {QStringLiteral("autoremove"), tr("Remove unused dependencies"),
         tr("Uninstall formulae that were only installed as dependencies"),
         {brew, {QStringLiteral("autoremove")}}, false, true, true},
        {QStringLiteral("leaves"), tr("List leaves"),
         tr("Installed formulae that are not dependencies of another formula"),
         {brew, {QStringLiteral("leaves")}}, false, false, false},
        {QStringLiteral("missing"), tr("Check missing dependencies"),
         tr("List installed formulae with missing dependencies"),
         {brew, {QStringLiteral("missing")}}, false, false, false},
        {QStringLiteral("deps"), tr("Dependency tree"),
         tr("Show the dependency tree of a package"),
         {brew, {QStringLiteral("deps"), QStringLiteral("--tree")}}, true, false, false},
        {QStringLiteral("uses"), tr("Who depends on it"),
         tr("List installed formulae that depend on a package"),
         {brew, {QStringLiteral("uses"), QStringLiteral("--installed")}}, true, false, false},
        {QStringLiteral("tap"), tr("List taps"),
         tr("Show all configured third-party repositories"),
         {brew, {QStringLiteral("tap")}}, false, false, false},
        {QStringLiteral("services"), tr("List services"),
         tr("Status of formulae that provide background services"),
         {brew, {QStringLiteral("services"), QStringLiteral("list")}}, false, false, false},
        {QStringLiteral("config"), tr("Show configuration"),
         tr("Print brew's own environment and configuration"),
         {brew, {QStringLiteral("config")}}, false, false, false},
    };
}
