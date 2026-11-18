#include "EaseOfAccessPage.h"
#include "Commands.h"
#include "LinkLabel.h"
#include "IconHelper.h"
#include "Win7Ui.h"

#include <QScrollArea>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QFont>

// The KDE modules the tools and settings hand off to.
static const QStringList kAccess      = { "kcmshell6", "kcm_access" };
static const QStringList kMagnifier   = { "kcmshell6", "kcm_kwin_effects" };
static const QStringList kHighContrast = { "kcmshell6", "kcm_colors" };
static const QStringList kMouse       = { "kcmshell6", "kcm_mouse" };
static const QStringList kCursorTheme = { "kcmshell6", "kcm_cursortheme" };

// Sidebar
QList<SidebarLink> EaseOfAccessPage::sidebarLinks()
{
    return {};
}

QList<SidebarLink> EaseOfAccessPage::sidebarSeeAlso()
{
    return {
        Nav::to("Personalization", PageId::Personalization),
        Nav::command("Display", kcm("kcm_kscreen")),
    };
}

void EaseOfAccessPage::addSettingLink(QVBoxLayout *into, const QString &iconName,
                                      const QString &text, const QStringList &cmd)
{
    auto *row = new QHBoxLayout;
    row->setContentsMargins(0, 0, 0, 0);
    row->setSpacing(10);

    auto *icon = new QLabel;
    icon->setFixedSize(24, 24);
    icon->setPixmap(themeIcon({iconName.toUtf8().constData(),
                               "preferences-desktop-accessibility"}).pixmap(24, 24));
    icon->setStyleSheet("background: transparent;");
    row->addWidget(icon, 0, Qt::AlignVCenter);

    auto *link = new LinkLabel(text);
    QObject::connect(link, &LinkLabel::clicked, this, [this, cmd]() {
        launchDetached(this, cmd);
    });
    row->addWidget(link, 0, Qt::AlignVCenter);
    row->addStretch(1);

    into->addLayout(row);
    into->addSpacing(10);
}

// Page
EaseOfAccessPage::EaseOfAccessPage(QScrollArea *sidebar, QWidget *parent)
    : QWidget(parent)
{
    // Windows 7 lays the content out at a fixed width and leaves the rest of
    // the window blank on the right rather than stretching to fill it.
    auto *contentV = Win7::pageScaffold(this, sidebar, /*bottomMargin=*/20,
                                        /*fixedWidth=*/700);

    // Page title.
    contentV->addWidget(Win7::pageTitle("使计算机更易于使用"));
    contentV->addSpacing(16);

    auto addHeading = [&](const QString &text) {
        contentV->addLayout(
            Win7::sectionHeading(text, nullptr, nullptr, "#000000"));
        contentV->addSpacing(10);
    };

    // Quick access to common tools
    addHeading("常用工具的快速访问");

    auto *intro = Win7::label(
        "你可以使用此部分中的工具来帮助你入门。",
        9, "#333333");
    intro->setWordWrap(true);
    contentV->addWidget(intro);
    contentV->addSpacing(12);

    // 2x2 grid of tool tiles.
    auto *grid = new QGridLayout;
    grid->setContentsMargins(6, 0, 0, 0);
    grid->setHorizontalSpacing(28);
    grid->setVerticalSpacing(14);

    auto makeTile = [&](const QString &iconName, const QString &text,
                        const QStringList &cmd) {
        auto *tile = new QHBoxLayout;
        tile->setContentsMargins(0, 0, 0, 0);
        tile->setSpacing(10);

        auto *icon = new QLabel;
        icon->setFixedSize(32, 32);
        icon->setPixmap(themeIcon({iconName.toUtf8().constData(),
                                   "preferences-desktop-accessibility"}).pixmap(32, 32));
        icon->setStyleSheet("background: transparent;");
        tile->addWidget(icon, 0, Qt::AlignVCenter);

        auto *link = new LinkLabel(text);
        QObject::connect(link, &LinkLabel::clicked, this, [this, cmd]() {
            launchDetached(this, cmd);
        });
        tile->addWidget(link, 0, Qt::AlignVCenter);
        tile->addStretch(1);
        return tile;
    };

    grid->addLayout(makeTile("zoom-in", "启动放大镜", kMagnifier), 0, 0);
    grid->addLayout(makeTile("preferences-desktop-keyboard",
                             "启动屏幕键盘", kAccess), 0, 1);
    grid->addLayout(makeTile("audio-volume-high", "启动讲述人", kAccess), 1, 0);
    grid->addLayout(makeTile("preferences-desktop-theme",
                             "设置高对比度", kHighContrast), 1, 1);
    grid->setColumnStretch(0, 1);
    grid->setColumnStretch(1, 1);

    contentV->addLayout(grid);
    contentV->addSpacing(20);

    // Explore all settings
    addHeading("浏览所有设置");

    auto *explore = Win7::label(
        "选择这些设置后，它们将在你每次登录时自动启动。", 9, "#333333");
    explore->setWordWrap(true);
    contentV->addWidget(explore);
    contentV->addSpacing(12);

    auto *list = new QVBoxLayout;
    list->setContentsMargins(6, 0, 0, 0);
    list->setSpacing(0);

    addSettingLink(list, "preferences-desktop-accessibility",
                   "不使用显示器使用计算机", kAccess);
    addSettingLink(list, "zoom-in",
                   "使计算机更易于查看", kMagnifier);
    addSettingLink(list, "input-mouse",
                   "不使用鼠标或键盘使用计算机", kAccess);
    addSettingLink(list, "input-mouse",
                   "使鼠标更易于使用", kMouse);
    addSettingLink(list, "input-keyboard",
                   "使键盘更易于使用", kAccess);
    addSettingLink(list, "audio-volume-high",
                   "使用文本或视觉替代声音", kAccess);
    addSettingLink(list, "preferences-desktop-cursors",
                   "更改鼠标指针的大小", kCursorTheme);

    contentV->addLayout(list);
    contentV->addStretch(1);
}
