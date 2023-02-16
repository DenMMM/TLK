#ifndef DIALOGNEWPASS_H
#define DIALOGNEWPASS_H
//---------------------------------------------------------------------------
#include <QDialog>

#include <string>
#include <optional>
//---------------------------------------------------------------------------
QT_BEGIN_NAMESPACE
namespace Ui {
class dialogNewPass;
}
QT_END_NAMESPACE
//---------------------------------------------------------------------------
class dialogNewPass : public QDialog
{
    Q_OBJECT
public:
    dialogNewPass(QWidget* parent = 0);
    ~dialogNewPass();

    std::optional <std::wstring> exec(
        int minLength, int maxLength,
        int left, int top, bool leftTop);

private slots:
    void on_buttonNew_clicked();

private:
    Ui::dialogNewPass *ui;

    std::wstring tmpPass;
    int minLength;

    void setCoord(int left, int top, bool leftTop);
};
//---------------------------------------------------------------------------
#endif // DIALOGNEWPASS_H
