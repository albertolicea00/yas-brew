#include <QTest>

#include "brewadapter.h"

class TestBrewAdapter : public QObject {
    Q_OBJECT
private slots:
    void searchParsesFormulaeAndCasks()
    {
        BrewAdapter adapter;
        const auto packages = adapter.parseSearch(QStringLiteral(
            "==> Formulae\n"
            "wget\n"
            "wget2\n"
            "==> Casks\n"
            "wgestures\n"));
        QCOMPARE(packages.size(), 3);
        QCOMPARE(packages.at(0).id, QStringLiteral("wget"));
        QCOMPARE(packages.at(0).kind, QStringLiteral("formula"));
        QCOMPARE(packages.at(2).id, QStringLiteral("wgestures"));
        QCOMPARE(packages.at(2).kind, QStringLiteral("cask"));
    }

    void installedParsesJsonV2()
    {
        BrewAdapter adapter;
        const auto packages = adapter.parseInstalled(QStringLiteral(R"({
            "formulae": [{
                "name": "jq",
                "desc": "Lightweight JSON processor",
                "homepage": "https://jqlang.github.io/jq/",
                "tap": "homebrew/core",
                "pinned": true,
                "outdated": false,
                "versions": {"stable": "1.7.1"},
                "installed": [{"version": "1.7.1"}]
            }],
            "casks": [{
                "token": "firefox",
                "name": ["Mozilla Firefox"],
                "desc": "Web browser",
                "homepage": "https://www.mozilla.org/firefox/",
                "tap": "homebrew/cask",
                "version": "128.0",
                "outdated": true,
                "installed": "127.0"
            }]
        })"));
        QCOMPARE(packages.size(), 2);

        const auto &jq = packages.at(0);
        QCOMPARE(jq.id, QStringLiteral("jq"));
        QCOMPARE(jq.kind, QStringLiteral("formula"));
        QVERIFY(jq.pinned);
        QVERIFY(jq.installed());
        QVERIFY(!jq.outdated());

        const auto &firefox = packages.at(1);
        QCOMPARE(firefox.id, QStringLiteral("firefox"));
        QCOMPARE(firefox.name, QStringLiteral("Mozilla Firefox"));
        QCOMPARE(firefox.kind, QStringLiteral("cask"));
        QVERIFY(firefox.installed());
        QVERIFY(firefox.outdated());
    }

    void outdatedParsesJsonV2()
    {
        BrewAdapter adapter;
        const auto packages = adapter.parseOutdated(QStringLiteral(R"({
            "formulae": [{
                "name": "node",
                "installed_versions": ["21.0.0"],
                "current_version": "22.1.0",
                "pinned": false
            }],
            "casks": [{
                "name": "slack",
                "installed_versions": ["4.38"],
                "current_version": "4.39"
            }]
        })"));
        QCOMPARE(packages.size(), 2);
        QCOMPARE(packages.at(0).installedVersion, QStringLiteral("21.0.0"));
        QCOMPARE(packages.at(0).version, QStringLiteral("22.1.0"));
        QVERIFY(packages.at(0).outdated());
        QCOMPARE(packages.at(1).kind, QStringLiteral("cask"));
    }

    void caskCommandsGetCaskFlag()
    {
        BrewAdapter adapter;
        QCOMPARE(adapter.installCommand("firefox", "cask").arguments,
                 QStringList({"install", "--cask", "firefox"}));
        QCOMPARE(adapter.installCommand("jq", "formula").arguments,
                 QStringList({"install", "jq"}));
        QVERIFY(!adapter.pinCommand("firefox", "cask").isValid());
        QVERIFY(adapter.pinCommand("jq", "formula").isValid());
    }
};

QTEST_MAIN(TestBrewAdapter)
#include "tst_brewadapter.moc"
