#include "LinverConfigDialog.h"
#include "Branding.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QIcon>

LinverConfigDialog::LinverConfigDialog(const QString &distroName, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("控制面板设置"));
    setWindowIcon(Branding::windowsLogo());
    setModal(true);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(10);

    // ---- Branding style row ----------------------------------------------
    auto *styleRow = new QHBoxLayout;
    styleRow->setSpacing(10);
    styleRow->addWidget(new QLabel(QStringLiteral("使用的品牌样式：")));

    m_styleCombo = new QComboBox;
    // Index order mirrors Branding::Style: 0 = distro, 1 = Windows 7.
    m_styleCombo->addItem(distroName.isEmpty() ? QStringLiteral("Linux")
                                               : distroName);
    m_styleCombo->addItem(QStringLiteral("Windows 7"));
    m_styleCombo->setCurrentIndex(
        Branding::style() == Branding::Style::Windows7 ? 1 : 0);
    styleRow->addWidget(m_styleCombo, 1);
    root->addLayout(styleRow);

    // ---- Toggles ----------------------------------------------------------
    // Choosing the Windows 7 style already swaps in the fake edition/logo, so no
    // separate "fake version" toggle is needed. This switch renames every
    // "Linux X" feature mention to "Windows X" and is independent of the style.
    m_windowsNames = new QCheckBox(
        QStringLiteral("使用 Windows 名称表示系统功能（更新、防火墙等）"));
    m_windowsNames->setChecked(Branding::useWindowsNames());
    root->addWidget(m_windowsNames);

    // ---- Customize hint + (stub) button ----------------------------------
    auto *custRow = new QHBoxLayout;
    custRow->setSpacing(10);
    auto *hint = new QLabel(
        QStringLiteral("要自定义文本、品牌图像和其他选项，请单击“自定义”。"));
    custRow->addWidget(hint, 1);

    auto *customize = new QPushButton(QStringLiteral("自定义..."));
    customize->setEnabled(false);   // reserved for a future customization panel
    custRow->addWidget(customize, 0, Qt::AlignBottom);
    root->addLayout(custRow);

    // ---- OK / Cancel ------------------------------------------------------
    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok
                                         | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
        applyToSettings();
        accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    root->addWidget(buttons);

    setSizeGripEnabled(false);
    setFixedSize(sizeHint().expandedTo(QSize(420, 0)));
}

void LinverConfigDialog::applyToSettings()
{
    Branding::setStyle(m_styleCombo->currentIndex() == 1
                           ? Branding::Style::Windows7
                           : Branding::Style::Distro);
    Branding::setUseWindowsNames(m_windowsNames->isChecked());
}
