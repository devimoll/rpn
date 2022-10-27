#ifndef RPN_H
#define RPN_H

#include <QWidget>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QLabel>
#include <QString>
#include <QVector>
#include <QtMath>
#include <QMessageBox>
#include <time.h>

class rpn : public QWidget
{
    Q_OBJECT

public:
    rpn(QWidget *parent = nullptr);
    ~rpn();

private:
    QVBoxLayout *layout;
    QTextEdit *input;
    QLabel *result;
    QVector<double> stack;
    QVector<double> t; // term
    int base;

    double pop();
    void push(double);
    bool set(int);
    void input_changed();
};
#endif // RPN_H
