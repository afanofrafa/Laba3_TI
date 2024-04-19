#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#define ull unsigned long long
QT_BEGIN_NAMESPACE
using namespace std;
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);  
    ~MainWindow();

private slots:
    void on_pushButton_close_clicked();

    void on_pushButton_show_new_p_clicked();

    void on_pushButton_choose_p_clicked();

    void on_spinBox_p_valueChanged(int arg1);

    void on_pushButton_confirm_clicked();

    void on_pushButton_encypher_clicked();

    void on_pushButton_choose_file_clicked();

    void on_pushButton_decypher_clicked();

    vector<ull> find_primitive_root(ull p);

private:
    Ui::MainWindow *ui;
    void closeEvent(QCloseEvent *event) override;
};
#endif // MAINWINDOW_H
