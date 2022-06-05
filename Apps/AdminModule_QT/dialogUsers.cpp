#include "dialogUsers.h"
#include "ui_dialogUsers.h"

#include "dialogNewPass.h"


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

dialogUsers::~dialogUsers()
{
    delete ui;
}

std::optional<MUsers> dialogUsers::exec(
    const MUsers& src_, int left_, int top_)
{
    tmp_users = src_;

    for (auto& user: tmp_users)
    {
        const auto row_index = ui->tableWidgetUsers->rowCount();
        ui->tableWidgetUsers->setRowCount(row_index + 1);
        ui->tableWidgetUsers->setItem(row_index, 0, new QTableWidgetItem);
        ui->tableWidgetUsers->setItem(row_index, 1, new QTableWidgetItem);
        set_tableWidgetUsers_line(row_index, &user);
    }

    this->move(left_, top_);
    set_edit(false);

    BasicRand.event();

    std::optional <MUsers> result;

    if (static_cast<QDialog*>(this)->exec() == QDialog::Accepted)
        result = std::move(tmp_users);

    return result;
}

void dialogUsers::on_buttonPassword_clicked()
{
    dialogNewPass dialog;
    dialog.exec();
}

void dialogUsers::on_tableWidgetUsers_itemSelectionChanged()
{
    auto items = ui->tableWidgetUsers->selectedItems();

    if (items.count() != ui->tableWidgetUsers->columnCount())
    {
        set_edit(false);
    }
    else
    {
        set_edit(true);

        auto data = items.first()->data(Qt::UserRole);
        auto* user = reinterpret_cast<MUsersItem*>(data.value<void*>());

        ui->lineEditLogin->setText(QString::fromStdWString(user->Login));
        ui->lineEditName->setText(QString::fromStdWString(user->Name));
    }
}

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
    set_tableWidgetUsers_line(item->row(), user);
}

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
    set_tableWidgetUsers_line(item->row(), user);
}

void dialogUsers::on_buttonActive_clicked()
{
    update_active(true);
}

void dialogUsers::on_buttonInactive_clicked()
{
    update_active(false);
}

void dialogUsers::on_buttonAdd_clicked()
{
    if (tmp_users.gCount() >= MAX_Users)
    {
        ResMessageBox((HWND)this->windowHandle(), 0, 23, MB_APPLMODAL|MB_OK|MB_ICONINFORMATION);
        return;
    }

    MUsersItem& new_user = tmp_users.Add();
    new_user.Login = L"NewUser";
    new_user.Name = L"Новый пользователь";

    const auto row_index = ui->tableWidgetUsers->rowCount();
    ui->tableWidgetUsers->setRowCount(row_index + 1);
    ui->tableWidgetUsers->setItem(row_index, 0, new QTableWidgetItem);
    ui->tableWidgetUsers->setItem(row_index, 1, new QTableWidgetItem);
    set_tableWidgetUsers_line(row_index, &new_user);
}

void dialogUsers::on_buttonDel_clicked()
{
    auto items = ui->tableWidgetUsers->selectedItems();

    for (auto i_item = items.rbegin(); i_item != items.rend(); ++i_item)
    {
        if ((*i_item)->column() != 0)
            continue;

        auto data = (*i_item)->data(Qt::UserRole);
        auto* user = reinterpret_cast<MUsersItem*>(data.value<void*>());
        ui->tableWidgetUsers->removeRow((*i_item)->row());
        tmp_users.Del(MUsers::const_iterator(user));
    }
}

void dialogUsers::update_active(bool active_)
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
        user->Active = active_;
        set_tableWidgetUsers_line(item->row(), user);
    }
}

void dialogUsers::set_tableWidgetUsers_line(
    int row_index_, MUsersItem* user_)
{
    auto ptr_to_user = QVariant::fromValue(reinterpret_cast<void*>(user_));

    auto* item1 = ui->tableWidgetUsers->item(row_index_, 0);
    item1->setData(Qt::UserRole, ptr_to_user);
    item1->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    item1->setText(QString::fromStdWString(user_->Login));

    QIcon icon_active = user_->Active ?
        QIcon(":/graphics/IconUserActive.png"):
        QIcon(":/graphics/IconUserInactive.png");
    item1->setIcon(icon_active);

    auto* item2 = ui->tableWidgetUsers->item(row_index_, 1);
    item2->setData(Qt::UserRole, ptr_to_user);
    item2->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    item2->setText(QString::fromStdWString(user_->Name));
}

void dialogUsers::set_edit(bool edit_)
{
    ui->labelLogin->setEnabled(edit_);
    ui->lineEditLogin->setEnabled(edit_);
    ui->labelFullName->setEnabled(edit_);
    ui->lineEditName->setEnabled(edit_);
    ui->buttonPassword->setEnabled(edit_);
}
