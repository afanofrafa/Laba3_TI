#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFile>
#include <QByteArray>
#include <QFileDialog>
#include <QImageReader>
#include <QTextStream>
#include <QMessageBox>
#include <random>
ull last_primitive = 2;
ull P = 0;
ull G = 0;
ull X = 0;
ull Y = 0;
vector<ull> v_K;
QString filePath;
QByteArray file_text;
int gcd(int a, int b) {
    if (b == 0)
        return a;
    return gcd(b, a % b);
}
int get_random_int(int min, int max) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(min, max);
    return dis(gen);
}
ull _pow_mod(ull value, ull degree, ull n_mod) {
    ull result = 1;
    while (degree) {
        if (degree & 0x01)
            result = (result * value) % n_mod;
        value = (value * value) % n_mod;
        degree >>= 1;
    }
    return result;
}

double _pow(double value, ull degree) {
    double result = 1;
    while (degree) {
        if (degree & 0x01)
            result *= value;
        value *= value;
        degree >>= 1;
    }
    return result;
}

bool fermat_test(const ull& n, int k) {
    for (int i = 0; i < k; i++) {
        // Генерируем случайное число a в интервале [2, n-1]
        ull a;
        if (n > 2)
            a = 2 + rand() % (n - 2);
        else {
            a = n;
            if (a == 2)
                return true;
        }
        // Вычисляем a^(n-1) по модулю n
        ull res = _pow_mod(a, n - 1, n);

        // Если результат не равен 1, то n составное
        if (res != 1)
            return false;
    }
    // Если число прошло все проверки, то оно вероятно простое
    return true;
}
// Функция для факторизации числа
vector<ull> factorize(ull n) {
    vector<ull> factors;
    for (ull i = 2; i * i <= n; i++) {
        while (n % i == 0) {
            factors.push_back(i);
            n /= i;
        }
    }
    if (n > 1)
        factors.push_back(n);
    return factors;
}
// Функция для проверки, является ли a первообразным корнем по модулю p
bool is_primitive_root(ull a, ull p) {
    // Факторизуем p-1
    vector<ull> factors = factorize(p - 1);

    // Проверяем, что a^((p-1)/q) != 1 для всех простых делителей q числа p-1
    for (ull q : factors) {
        if (_pow_mod(a, (p - 1) / q, p) == 1) {
            return false; // Если условие не выполняется, возвращаем false
        }
    }
    return true; // Если условие выполняется для всех q, возвращаем true
}
vector<ull> MainWindow::find_primitive_root(ull p) {
    // Проверяем простоту числа p
    vector<ull> v_q;
    if (p > 2) {
        if (!fermat_test(p, 40)) {
            qDebug() << "Error: p is not a prime number.";
            return v_q;
        }

        // Перебираем все возможные значения a в интервале [2, p-1]
        ull a;
        if (ui->radioButton_show_all->isChecked())
            a = 2;
        else {
            int k = (int)(p / 100);
            if (p - k > 2)
                a = p - k;
            else
                a = 2;
        }
        for ( ; a < p; a++) {
            if (is_primitive_root(a, p)) {
                v_q.push_back(a); // Если a является первообразным корнем, добавляем его
            }
        }

    }
    return v_q;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    on_pushButton_show_new_p_clicked();
    ui->progressBar->setVisible(false);
    ui->progressBar->setEnabled(false);
    ui->pushButton_choose_file->setEnabled(false);
    ui->pushButton_decypher->setEnabled(false);
    ui->pushButton_encypher->setEnabled(false);
    ui->label_file_path->setText("");
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    delete ui;
}

MainWindow::~MainWindow() {}

void MainWindow::on_pushButton_close_clicked()
{
    close();
}

void MainWindow::on_pushButton_show_new_p_clicked()
{
    ull it = 0;
    if (last_primitive + 1 < INT_MAX)
        for (ull i = last_primitive; it == 0 || it % 100000 != 0; i++) {
            if (fermat_test(i, 40)) {
                QString str = QString::number(ui->comboBox_choose_p->count() + 1) + ": " + QString::number(i);
                ui->comboBox_choose_p->addItem(str);
                it++;
            }
            if (last_primitive + 1 < INT_MAX)
                last_primitive = i + 1;
        }
}


void MainWindow::on_pushButton_choose_p_clicked()
{
    QString value = ui->comboBox_choose_p->currentText();
    for (int i = 0; i < value.size(); i++) {
        if (value.at(i) == ' ') {
            i++;
            value.erase(value.begin(), value.begin() + i);
            ui->spinBox_p->setValue(value.toUInt());
            break;
        }
    }
}

void MainWindow::on_spinBox_p_valueChanged(int arg1)
{
    ull p = ui->spinBox_p->value();
    if (fermat_test(p, 40)) {
        ui->spinBox_p->setStyleSheet("QSpinBox { background-color: white; }");
        vector<ull> vect = find_primitive_root(p);
        ui->comboBox_choose_g->clear();
        if (ui->radioButton_show_all->isChecked()) {
            for (int i = 0; i < vect.size(); i++) {
                QString str = QString::number(ui->comboBox_choose_g->count() + 1) + ": " + QString::number(vect.at(i));
                ui->comboBox_choose_g->addItem(str);
            }
        }
        else {
            QString str = QString::number(  vect.at( get_random_int(0, vect.size() - 1) )  );
            ui->comboBox_choose_g->addItem(str);
        }
        int v = ui->spinBox_p->value();
        if (v > 2)
            ui->spinBox_x->setMaximum(v - 1);
        else
            ui->spinBox_x->clear();
    }
    else {
        ui->spinBox_p->setStyleSheet("QSpinBox { background-color: #ff6f71; }");
        ui->comboBox_choose_g->clear();
    }
}


void MainWindow::on_pushButton_confirm_clicked()
{
    if (ui->comboBox_choose_g->count()) {
        QString str = "(";
        ull p = ui->spinBox_p->value();
        P = p;
        v_K.clear();
        bool fl = true;
        for (ull i = 2; i < P; i++) {
            if (gcd(i, p - 1) == 1) {
                if (ui->radioButton_show_all->isChecked() || fl) {
                    ui->comboBox_K->addItem(QString::number(i));
                    fl = false;
                }
                v_K.push_back(i);
            }
        }
        if (v_K.size() == 0) {
            ui->label_Ko->setText("");
            ui->pushButton_choose_file->setEnabled(false);
            ui->pushButton_decypher->setEnabled(false);
            ui->pushButton_encypher->setEnabled(false);
            return;
        }
        str += QString::number(p) + ", ";
        QString g = ui->comboBox_choose_g->currentText();
        for (int i = 0; i < g.size(); i++) {
            if (g.at(i) == ' ') {
                i++;
                g.erase(g.begin(), g.begin() + i);
                break;
            }
        }
        G = g.toUInt();
        str += g + ", ";
        ull x = ui->spinBox_x->value();
        X = x;
        ull y = _pow_mod(g.toUInt(), x, ui->spinBox_p->value());
        Y = y;
        str += QString::number(y) + ")";
        ui->label_Ko->setText(str);
        ui->pushButton_choose_file->setEnabled(true);
        ui->pushButton_decypher->setEnabled(true);
        ui->pushButton_encypher->setEnabled(true);
    }
    else {
        ui->label_Ko->setText("");
        ui->pushButton_choose_file->setEnabled(false);
        ui->pushButton_decypher->setEnabled(false);
        ui->pushButton_encypher->setEnabled(false);
    }
}


void MainWindow::on_pushButton_encypher_clicked()
{
    if (!file_text.isEmpty()) {
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            QMessageBox::critical(this, "Ошибка", "Не удалось создать файл для записи.");
            return;
        }
        float pr_bar_value = 0;
        ull size = file_text.size();
        float s_pt_stp = 1.0 * 90.0 / 100.0;
        QByteArray encypher;
        ui->textEdit_text_of_cypher->clear();
        ui->progressBar->setVisible(true);
        ui->progressBar->setEnabled(true);
        for (int i = 0; i < file_text.size(); i++) {
            QString str = "";
            ull K = v_K.at( get_random_int(0, v_K.size() - 1) );
            ull M = file_text.at(i) + 128;
            ull A = _pow_mod(G, K, P);
            ull B = ( (ull)_pow_mod(Y, K, P) * M ) % P;
            for (int j = 4; j >= 0; j--) {
                char ch_a = static_cast<char>(A & 0x7F);
                char ch_b = static_cast<char>(B & 0x7F);
                A >>= 7;
                B >>= 7;
                encypher.append(ch_a);
                encypher.append(ch_b);
                str.append(QString::number(ch_a) + ' ' + QString::number(ch_b) + ' ');
            }
            ui->textEdit_text_of_cypher->append(str);
            pr_bar_value += s_pt_stp;
            ui->progressBar->setValue(pr_bar_value / size * 100);
        }
        file.write(encypher);
        ui->progressBar->setValue(100);
        file.close();
        file_text.clear();
        file_text = encypher;
        ui->progressBar->setVisible(false);
        ui->progressBar->setValue(0);
        ui->progressBar->setEnabled(false);
    }
}


void MainWindow::on_pushButton_choose_file_clicked()
{
    filePath = QFileDialog::getOpenFileName(this, "Выберите файл", "C:/Users/Archie/Documents/ТИ_файлы_лаба2", "Все файлы (*.*)");
    if (filePath.isEmpty()) {
        ui->label_file_path->setText("");
        QMessageBox::warning(this, "Предупреждение", "Файл не выбран.");
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл для чтения.");
        return;
    }
    ui->label_file_path->setText("Текущий путь к файлу: " + filePath);
    file_text = file.readAll(); // Чтение всех байтов файла в QByteArray
    float pr_bar_value = 0;
    ull size = (ull)(file_text.size());
    ui->textEdit_text_of_cypher->clear();
    ui->progressBar->setVisible(true);
    ui->progressBar->setEnabled(true);
    for (int i = 0; i < file_text.size(); i++) {
        ui->textEdit_text_of_cypher->append(QString::number(file_text.at(i) + 128));
        pr_bar_value++;
        ui->progressBar->setValue(pr_bar_value / size * 100);
    }
    file.close();
    ui->progressBar->setVisible(false);
    ui->progressBar->setValue(0);
    ui->progressBar->setEnabled(false);
}


void MainWindow::on_pushButton_decypher_clicked()
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadWrite)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось создать файл для чтения и записи.");
        return;
    }
    file_text = file.readAll();
    float pr_bar_value = 0;
    ull size = (ull)(file_text.size() / 10);
    float s_pt_stp = 1.0 * 90.0 / 100.0;
    file.resize(0);
    file.seek(0);
    QByteArray decypher;
    ui->progressBar->setVisible(true);
    ui->progressBar->setEnabled(true);
    ui->textEdit_text_of_cypher->clear();
    for (int i = 0; i < file_text.size() - 9; i += 10) {
        ull A = 0;
        ull B = 0;
        int count = 0;
        for (int j = i; (j < file_text.size() - 1) && (j < i + 10); j += 2) {
            ull ch_a = file_text.at(j);
            ull ch_b = file_text.at(j + 1);
            for (int k = 0; k < count; k++) {
                ch_a <<= 7;
                ch_b <<= 7;
            }
            A |= ch_a;
            B |= ch_b;
            count++;
        }
        ull M = ( (ull)_pow_mod( A, P - 1 - X, P) * B ) % P;
        decypher.append(M - 128);
        ui->textEdit_text_of_cypher->append(QString::number(M));
        pr_bar_value += s_pt_stp;
        ui->progressBar->setValue(pr_bar_value / size * 100);
    }
    file.write(decypher);
    ui->progressBar->setValue(100);
    file.close();
    ui->progressBar->setVisible(false);
    ui->progressBar->setValue(0);
    ui->progressBar->setEnabled(false);
}

