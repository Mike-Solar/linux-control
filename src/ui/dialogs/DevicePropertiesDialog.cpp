#include "DevicePropertiesDialog.h"
#include "IconHelper.h"
#include "UiText.h"

#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QFrame>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QTreeWidget>
#include <QHeaderView>
#include <QTextEdit>
#include <QShowEvent>

namespace {

// Win7 group boxes on the property sheet: a light rounded frame with the title
// sitting just above the top-left of the border.
const char *kGroupStyle =
    "QGroupBox {"
    "  border: 1px solid #D9D9D9; border-radius: 3px;"
    "  margin-top: 9px; background: transparent; }"
    "QGroupBox::title {"
    "  subcontrol-origin: margin; subcontrol-position: top left;"
    "  left: 8px; padding: 0 3px; color: #000000; }";

QLabel *plain(const QString &text) {
    auto *l = new QLabel(text);
    l->setTextFormat(Qt::PlainText);
    l->setTextInteractionFlags(Qt::TextSelectableByMouse);
    return l;
}

QString valueOr(const QString &v, const QString &fallback = QStringLiteral("不可用")) {
    QString t = v.trimmed();
    if (t.isEmpty() || t.startsWith('('))   // "(Standard monitor types)" etc.
        return fallback;
    return t;
}

} // namespace

DevicePropertiesDialog::DevicePropertiesDialog(const ShellDevice &dev,
                                               QWidget *parent)
    : QDialog(parent), m_dev(dev) {
    setWindowTitle(dev.name + " Properties");
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint
                   | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    // Grey dialog body; the tab pages themselves are white (set per-page).
    setStyleSheet("DevicePropertiesDialog { background: #F0F0F0; }");
    setFixedWidth(400);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 8);
    layout->setSpacing(8);

    auto *tabs = new QTabWidget(this);
    tabs->addTab(buildGeneralTab(), "常规");
    tabs->addTab(buildHardwareTab(), "硬件");
    layout->addWidget(tabs, 1);

    auto *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel
        | QDialogButtonBox::Apply, this);
    if (auto *apply = buttons->button(QDialogButtonBox::Apply))
        apply->setEnabled(false);   // nothing editable, mirroring the reference
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}

void DevicePropertiesDialog::showEvent(QShowEvent *e) {
    QDialog::showEvent(e);
    if (!m_sizeAdjusted) {
        m_sizeAdjusted = true;
        adjustSize();
        setFixedSize(size());
    }
}

QWidget *DevicePropertiesDialog::buildGeneralTab() {
    auto *w = new QWidget;
    // ID-scoped: a declaration-only sheet would act as `* { ... }` and drag
    // the tab's tree/text scroll bars into non-native stylesheet rendering.
    w->setObjectName("devPropTab");
    w->setStyleSheet("#devPropTab { background: #FFFFFF; }");
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(12, 12, 12, 12);
    v->setSpacing(10);

    // ---- Device header (icon + name) -------------------------------------
    auto *header = new QHBoxLayout;
    header->setSpacing(10);
    auto *icon = new QLabel;
    icon->setPixmap(resolveIcon(m_dev.iconName).pixmap(32, 32));
    icon->setFixedSize(36, 36);
    icon->setAlignment(Qt::AlignTop);
    header->addWidget(icon, 0, Qt::AlignTop);
    auto *nameLabel = plain(m_dev.name);
    nameLabel->setWordWrap(true);
    nameLabel->setAlignment(Qt::AlignVCenter);
    header->addWidget(nameLabel, 1);
    v->addLayout(header);

    // ---- Device Information ----------------------------------------------
    auto *infoBox = new QGroupBox("设备信息");
    infoBox->setStyleSheet(kGroupStyle);
    auto *grid = new QGridLayout(infoBox);
    grid->setContentsMargins(12, 12, 12, 12);
    grid->setHorizontalSpacing(14);
    grid->setVerticalSpacing(7);
    grid->setColumnStretch(1, 1);

    int row = 0;
    auto addRow = [&](const QString &key, const QString &value) {
        auto *k = new QLabel(key);
        k->setStyleSheet("color: #555555;");
        auto *val = plain(value);
        val->setWordWrap(true);
        grid->addWidget(k,   row, 0, Qt::AlignTop | Qt::AlignLeft);
        grid->addWidget(val, row, 1, Qt::AlignTop | Qt::AlignLeft);
        ++row;
    };

    addRow("制造商：", valueOr(m_dev.manufacturer));
    addRow("型号：",        valueOr(m_dev.model.isEmpty() ? m_dev.name : m_dev.model));
    addRow("型号编号：", valueOr(m_dev.modelNumber));
    addRow("类别：",   valueOr(m_dev.category, QStringLiteral("未知")));
    addRow("描述：",  valueOr(m_dev.description));
    v->addWidget(infoBox);

    // ---- Device Tasks ----------------------------------------------------
    auto *taskBox = new QGroupBox("设备任务");
    taskBox->setStyleSheet(kGroupStyle);
    auto *taskV = new QVBoxLayout(taskBox);
    taskV->setContentsMargins(12, 12, 12, 12);
    auto *taskText = new QLabel(
        "要查看此设备的任务，请右键单击“设备和打印机”中的设备图标。");
    taskText->setWordWrap(true);
    taskV->addWidget(taskText);
    v->addWidget(taskBox, 1);

    v->addStretch();
    return w;
}

QWidget *DevicePropertiesDialog::buildHardwareTab() {
    auto *w = new QWidget;
    w->setObjectName("devPropTab");
    w->setStyleSheet("#devPropTab { background: #FFFFFF; }");
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(12, 12, 12, 12);
    v->setSpacing(8);

    // Header, mirroring the General tab.
    auto *header = new QHBoxLayout;
    header->setSpacing(10);
    auto *icon = new QLabel;
    icon->setPixmap(resolveIcon(m_dev.iconName).pixmap(32, 32));
    icon->setFixedSize(36, 36);
    icon->setAlignment(Qt::AlignTop);
    header->addWidget(icon, 0, Qt::AlignTop);
    auto *nameLabel = plain(m_dev.name);
    nameLabel->setWordWrap(true);
    header->addWidget(nameLabel, 1);
    v->addLayout(header);

    v->addWidget(new QLabel("设备功能："));

    auto *tree = new QTreeWidget;
    tree->setColumnCount(2);
    tree->setHeaderLabels({"名称", "类型"});
    tree->setRootIsDecorated(false);
    tree->setSelectionMode(QAbstractItemView::SingleSelection);
    tree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    tree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    tree->setFixedHeight(96);

    auto *fn = new QTreeWidgetItem(tree);
    fn->setIcon(0, resolveIcon(m_dev.iconName));
    fn->setText(0, m_dev.name);
    fn->setText(1, valueOr(m_dev.category, QStringLiteral("设备")));
    tree->setCurrentItem(fn);
    v->addWidget(tree);

    // ---- Device Function Summary -----------------------------------------
    auto *summaryBox = new QGroupBox("设备功能摘要");
    summaryBox->setStyleSheet(kGroupStyle);
    auto *grid = new QGridLayout(summaryBox);
    grid->setContentsMargins(12, 12, 12, 12);
    grid->setHorizontalSpacing(14);
    grid->setVerticalSpacing(6);
    grid->setColumnStretch(1, 1);

    int row = 0;
    auto addRow = [&](const QString &key, const QString &value) {
        auto *k = new QLabel(key);
        k->setStyleSheet("color: #555555;");
        auto *val = plain(value);
        val->setWordWrap(true);
        grid->addWidget(k,   row, 0, Qt::AlignTop | Qt::AlignLeft);
        grid->addWidget(val, row, 1, Qt::AlignTop | Qt::AlignLeft);
        ++row;
    };
    addRow("制造商：",  valueOr(m_dev.manufacturer));
    addRow("位置：",      UiText::localize(valueOr(m_dev.location, QStringLiteral("未知"))));
    addRow("设备状态：", m_dev.status.isEmpty()
               ? QStringLiteral("此设备工作正常。")
               : UiText::localize(m_dev.status));
    v->addWidget(summaryBox);

    auto *btnRow = new QHBoxLayout;
    btnRow->addStretch(1);
    auto *propsBtn = new QPushButton("属性");
    connect(propsBtn, &QPushButton::clicked, this,
            &DevicePropertiesDialog::showFunctionProperties);
    btnRow->addWidget(propsBtn);
    v->addLayout(btnRow);

    v->addStretch();
    return w;
}

// A compact Device-Manager-style "General" sheet for the selected function,
// opened from the Hardware tab's Properties button.
void DevicePropertiesDialog::showFunctionProperties() {
    QDialog dlg(this);
    dlg.setWindowTitle(m_dev.name + " Properties");
    dlg.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint
                       | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    dlg.setStyleSheet("QDialog { background: #F0F0F0; }");
    dlg.setFixedWidth(360);

    auto *outer = new QVBoxLayout(&dlg);
    outer->setContentsMargins(10, 10, 10, 8);
    outer->setSpacing(8);

    auto *tabs = new QTabWidget(&dlg);
    auto *page = new QWidget;
    page->setObjectName("devPropTab");
    page->setStyleSheet("#devPropTab { background: #FFFFFF; }");
    auto *v = new QVBoxLayout(page);
    v->setContentsMargins(12, 12, 12, 12);
    v->setSpacing(8);

    auto *header = new QHBoxLayout;
    header->setSpacing(10);
    auto *icon = new QLabel;
    icon->setPixmap(resolveIcon(m_dev.iconName).pixmap(32, 32));
    icon->setFixedSize(36, 36);
    header->addWidget(icon, 0, Qt::AlignTop);
    auto *nameLabel = plain(m_dev.name);
    nameLabel->setWordWrap(true);
    header->addWidget(nameLabel, 1);
    v->addLayout(header);

    auto *sep = new QFrame;
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("color: #DDDDDD;");
    v->addWidget(sep);

    auto *grid = new QGridLayout;
    grid->setHorizontalSpacing(14);
    grid->setVerticalSpacing(6);
    grid->setColumnStretch(1, 1);
    int row = 0;
    auto addRow = [&](const QString &key, const QString &value) {
        auto *k = new QLabel(key);
        k->setStyleSheet("color: #555555;");
        auto *val = plain(value);
        val->setWordWrap(true);
        grid->addWidget(k,   row, 0, Qt::AlignTop | Qt::AlignLeft);
        grid->addWidget(val, row, 1, Qt::AlignTop | Qt::AlignLeft);
        ++row;
    };
    addRow("设备类型：",  valueOr(m_dev.category, QStringLiteral("硬件设备")));
    addRow("制造商：", valueOr(m_dev.manufacturer));
    addRow("位置：",     UiText::localize(valueOr(m_dev.location, QStringLiteral("未知"))));
    if (!m_dev.driver.isEmpty())
        addRow("驱动程序：", m_dev.driver
               + (m_dev.driverVersion.isEmpty() ? QString()
                                                : " (" + m_dev.driverVersion + ")"));
    v->addLayout(grid);

    auto *statusBox = new QGroupBox("设备状态");
    statusBox->setStyleSheet(kGroupStyle);
    auto *sv = new QVBoxLayout(statusBox);
    sv->setContentsMargins(10, 12, 10, 10);
    auto *status = new QTextEdit;
    status->setReadOnly(true);
    status->setFixedHeight(84);
    status->setPlainText(m_dev.status.isEmpty()
                             ? QStringLiteral("此设备工作正常。")
                             : UiText::localize(m_dev.status));
    sv->addWidget(status);
    v->addWidget(statusBox);
    v->addStretch();

    tabs->addTab(page, "常规");
    outer->addWidget(tabs, 1);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok, &dlg);
    QObject::connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    outer->addWidget(buttons);

    dlg.adjustSize();
    dlg.exec();
}
