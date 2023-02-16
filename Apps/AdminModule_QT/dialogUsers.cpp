//---------------------------------------------------------------------------
#include "dialogUsers.h"
#include "ui_dialogUsers.h"

#include "dialogUserPass.h"
//---------------------------------------------------------------------------
dialogUsers::dialogUsers(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialogUsers)
{
    ui->setupUi(this);

    ui->tableWidgetUsers->setColumnCount(2);
    ui->tableWidgetUsers->setColumnWidth(0, 150);
    ui->tableWidgetUsers->setColumnWidth(1, 300);
    ui->tableWidgetUsers->setHorizontalHeaderItem(0, new QTableWidgetItem(u8"Логин"));
    ui->tableWidgetUsers->setHorizontalHeaderItem(1, new QTableWidgetItem(u8"Полное имя"));

    ui->lineEditLogin->setMaxLength(MAX_UserLoginLen);
    ui->lineEditName->setMaxLength(MAX_UserNameLen);
}
//---------------------------------------------------------------------------
dialogUsers::~dialogUsers()
{
    delete ui;
}
//---------------------------------------------------------------------------
std::optional <MUsers> dialogUsers::exec(
    const MUsers& src, int left, int top)
{
    tmpUsers = src;

    for (auto& user: tmpUsers)
    {
        const auto rowIndex = ui->tableWidgetUsers->rowCount();
        ui->tableWidgetUsers->setRowCount(rowIndex + 1);
        ui->tableWidgetUsers->setItem(rowIndex, 0, new QTableWidgetItem);
        ui->tableWidgetUsers->setItem(rowIndex, 1, new QTableWidgetItem);
        setTableWidgetUsersLine(rowIndex, &user);
    }

    setEditable(false);
    ui->tableWidgetUsers->setFocus();
    this->move(left, top);

    BasicRand.event();

    std::optional <MUsers> result;

    if (QDialog::exec() == QDialog::Accepted)
        result = std::move(tmpUsers);

    return result;
}
//---------------------------------------------------------------------------
void dialogUsers::on_buttonPassword_clicked()
{
    auto items = ui->tableWidgetUsers->selectedItems();

    if (items.count() != ui->tableWidgetUsers->columnCount())
        return;

    auto* item = items.first();
    auto data = item->data(Qt::UserRole);
    auto* user = reinterpret_cast<MUsersItem*>(data.value<void*>());

    // Подготавливаем координаты
    const auto coord = ui->buttonPassword->mapToGlobal(QPoint(10, 10));

    // Открываем окно смены пароля
    dialogUserPass dialog;
    dialog.exec(*user, coord.x(), coord.y(), false);
}
//---------------------------------------------------------------------------
void dialogUsers::on_tableWidgetUsers_itemSelectionChanged()
{
    auto items = ui->tableWidgetUsers->selectedItems();

    if (items.count() != ui->tableWidgetUsers->columnCount())
    {
        setEditable(false);
    }
    else
    {
        setEditable(true);

        auto data = items.first()->data(Qt::UserRole);
        auto* user = reinterpret_cast<MUsersItem*>(data.value<void*>());

        ui->lineEditLogin->setText(QString::fromStdWString(user->Login));
        ui->lineEditName->setText(QString::fromStdWString(user->Name));
    }
}
//---------------------------------------------------------------------------
void dialogUsers::on_lineEditLogin_editingFinished()
{
    auto items = ui->tableWidgetUsers->selectedItems();

    if (items.count() != ui->tableWidgetUsers->columnCount())
        return;

    auto* item = items.first();
    auto data = item->data(Qt::UserRole);
    auto* user = reinterpret_cast<MUsersItem*>(data.value<void*>());

    ui->lineEditLogin->setText(ui->lineEditLogin->text().trimmed());
    user->Login = ui->lineEditLogin->text().toStdWString();
    setTableWidgetUsersLine(item->row(), user);
}
//---------------------------------------------------------------------------
void dialogUsers::on_lineEditName_editingFinished()
{
    auto items = ui->tableWidgetUsers->selectedItems();

    if (items.count() != ui->tableWidgetUsers->columnCount())
        return;

    auto* item = items.first();
    auto data = item->data(Qt::UserRole);
    auto* user = reinterpret_cast<MUsersItem*>(data.value<void*>());

    ui->lineEditName->setText(ui->lineEditName->text().trimmed());
    user->Name = ui->lineEditName->text().toStdWString();
    setTableWidgetUsersLine(item->row(), user);
}
//---------------------------------------------------------------------------
void dialogUsers::on_buttonActive_clicked()
{
    updateActive(true);
}
//---------------------------------------------------------------------------
void dialogUsers::on_buttonInactive_clicked()
{
    updateActive(false);
}
//---------------------------------------------------------------------------
void dialogUsers::on_buttonAdd_clicked()
{
    if (tmpUsers.gCount() >= MAX_Users)
    {
        ResMessageBox((HWND)this->windowHandle(), 0, 23, MB_APPLMODAL|MB_OK|MB_ICONINFORMATION);
        return;
    }

    MUsersItem& newUser = tmpUsers.Add();
    newUser.Login = L"NewUser";
    newUser.Name = L"Новый пользователь";

    const auto rowIndex = ui->tableWidgetUsers->rowCount();
    ui->tableWidgetUsers->setRowCount(rowIndex + 1);
    ui->tableWidgetUsers->setItem(rowIndex, 0, new QTableWidgetItem);
    ui->tableWidgetUsers->setItem(rowIndex, 1, new QTableWidgetItem);
    setTableWidgetUsersLine(rowIndex, &newUser);

    ui->tableWidgetUsers->selectRow(rowIndex);
    ui->lineEditLogin->setFocus();
}
//---------------------------------------------------------------------------
void dialogUsers::on_buttonDel_clicked()
{
    auto items = ui->tableWidgetUsers->selectedItems();

    for (auto iItem = items.rbegin(); iItem != items.rend(); ++iItem)
    {
        if ((*iItem)->column() != 0)
            continue;

        auto data = (*iItem)->data(Qt::UserRole);
        auto* user = reinterpret_cast<MUsersItem*>(data.value<void*>());
        ui->tableWidgetUsers->removeRow((*iItem)->row());
        tmpUsers.Del(MUsers::const_iterator(user));
    }
}
//---------------------------------------------------------------------------
void dialogUsers::updateActive(bool active)
{
    auto items = ui->tableWidgetUsers->selectedItems();

    if (items.empty())
        return;

    for (auto& item: items)
    {
        if (item->column() != 0)
            continue;

        auto data = item->data(Qt::UserRole);
        auto* user = reinterpret_cast<MUsersItem*>(data.value<void*>());
        user->Active = active;
        setTableWidgetUsersLine(item->row(), user);
    }
}
//---------------------------------------------------------------------------
void dialogUsers::setTableWidgetUsersLine(
    int rowIndex, MUsersItem* user)
{
    auto ptrToUser = QVariant::fromValue(reinterpret_cast<void*>(user));

    auto* item1 = ui->tableWidgetUsers->item(rowIndex, 0);
    item1->setData(Qt::UserRole, ptrToUser);
    item1->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    item1->setText(QString::fromStdWString(user->Login));

    QIcon iconActive = user->Active ?
        QIcon(":/graphics/IconUserActive.png"):
        QIcon(":/graphics/IconUserInactive.png");
    item1->setIcon(iconActive);

    auto* item2 = ui->tableWidgetUsers->item(rowIndex, 1);
    item2->setData(Qt::UserRole, ptrToUser);
    item2->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    item2->setText(QString::fromStdWString(user->Name));
}
//---------------------------------------------------------------------------
void dialogUsers::setEditable(bool mode)
{
    ui->labelLogin->setEnabled(mode);
    ui->lineEditLogin->setEnabled(mode);
    ui->labelFullName->setEnabled(mode);
    ui->lineEditName->setEnabled(mode);
    ui->buttonPassword->setEnabled(mode);
}
//---------------------------------------------------------------------------
