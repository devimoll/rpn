#include "rpn.h"

rpn::rpn(QWidget *parent)
    : QWidget(parent),
      layout{new QVBoxLayout()},
      input{new QTextEdit()},
      result{new QLabel()},
      stack{QVector<double>()},
      t{QVector<double>()},
      base{10}
{
    layout->addWidget(input);
    layout->addWidget(result);
    setLayout(layout);
    connect(input, &QTextEdit::textChanged,
            this, &rpn::input_changed);
    QString help = "+ - * / %/// acos acosh asin asinh atan2 atan atanh\
 ceil cos cosh dtr exp abs floor ln log lb **/pow rtd sin sinh sqrt tan\
 tanh gamma ~/not &/and |/or ^/xor << >> pi e unix bin oct deci hex help/hh";
    help_dialog.setParent(this);
    help_dialog.setWindowTitle("rpn - help");
    help_dialog.setText(help);
    help_dialog.setStandardButtons(QMessageBox::Close);
    help_dialog.setWindowModality(Qt::NonModal);
}

rpn::~rpn()
{
    delete layout;
    delete input;
    delete result;
}

double rpn::pop() {
    if (stack.isEmpty()) {
        qDebug() << "Quamquam acervus vacuus erat, pop() appellatus est.";
        QMessageBox::critical(this, "EXITIALE ERROR", "Quamquam acervus vacuus erat, pop() appellatus est.");
        exit(1);
    }
    double x = stack.back();
    stack.pop_back();
    return x;
}

void rpn::push(double x) {
    stack.append(x);
}

bool rpn::set(int n) {
    t.clear();
    t.shrink_to_fit();
    if (stack.size() < n) {
        result->setText("Too Few Arguments");
        return false;
    }
    for (int i = 0; i < n; i++) {
        t.append(pop());
    }
    return true;
}

bool rpn::is_integer(double n)
{
    return std::floor(n) == n;
}

// カーソル位置を保持したままテキストボックスに書き込む
void rpn::set_input_box_text(QString str) {
    QTextCursor tc = input->textCursor();
    // よくわからないが1を引かないとエラーが出る
    tc.setPosition(input->document()->characterCount() - 1);
    input->setText(str);
    input->setTextCursor(tc);
}

void rpn::convert_base(int base_old, int base, const QStringList &elements)
{
    double x;
    bool is_number;
    QString exprs = "";
    // 進数指定命令は不要だから-1
    for (int i = 0; i < elements.size() - 1; i++) {
        if (base_old == 10) {
            x = elements[i].toDouble(&is_number);
        }
        else {
            x = elements[i].toInt(&is_number, base_old);
        }
        if (is_number) {
            if (base == 10) {
                exprs += QString::number(x);
            }
            else {
                exprs += QString::number((int)x, base);
            }
        }
        else {
            exprs += elements[i];
        }
        exprs += " ";
    }
    set_input_box_text(exprs);
}

void rpn::input_changed()
{
    stack.clear();
    stack.shrink_to_fit();
    const QString input_text = input->toPlainText();
    QString exprs(input_text);
    exprs.replace("\n", " ");
    QStringList elements = exprs.split(" ");
    elements.removeAll("");
    //qDebug() << base << elements;
    for (const QString e : elements) {
        bool is_number;
        double x;
        if (base == 10) {
            x = e.toDouble(&is_number);
        }
        else {
            x = e.toInt(&is_number, base);
        }

        if (is_number) {
            push(x);
        }
        else {
            if (e == "+") {
                if (!set(2)) return;
                push(t[1] + t[0]);
            }
            else if (e == "-") {
                if (!set(2)) return;
                push(t[1] - t[0]);
            }
            else if (e == "*") {
                if (!set(2)) return;
                push(t[1] * t[0]);
            }
            else if (e == "/") {
                if (!set(2)) return;
                push(t[1] / t[0]);
            }
            else if (e == "%" || e == "//") {
                if (!set(2)) return;
                push(fmod(t[1], t[0]));
            }
            else if (e == "acos") {
                if (!set(1)) return;
                push(qAcos(t[0]));
            }
            else if (e == "acosh") {
                if (!set(1)) return;
                push(std::acosh(t[0]));
            }
            else if (e == "asin") {
                if (!set(1)) return;
                push(qAsin(t[0]));
            }
            else if (e == "asinh") {
                if (!set(1)) return;
                push(std::asinh(t[0]));
            }
            else if (e == "atan2") {
                if (!set(2)) return;
                push(qAtan2(t[1], t[0]));
            }
            else if (e == "atan") {
                if (!set(1)) return;
                push(qAtan(t[0]));
            }
            else if (e == "atanh") {
                if (!set(1)) return;
                push(std::atanh(t[0]));
            }
            else if (e == "ceil") {
                if (!set(1)) return;
                push(qCeil(t[0]));
            }
            else if (e == "cos") {
                if (!set(1)) return;
                push(qCos(t[0]));
            }
            else if (e == "cosh") {
                if (!set(1)) return;
                push(std::cosh(t[0]));
            }
            else if (e == "dtr") {
                if (!set(1)) return;
                push(qDegreesToRadians(t[0]));
            }
            else if (e == "exp") {
                if (!set(1)) return;
                push(qExp(t[0]));
            }
            else if (e == "abs") {
                if (!set(1)) return;
                push(qFabs(t[0]));
            }
            else if (e == "floor") {
                if (!set(1)) return;
                push(qFloor(t[0]));
            }
            else if (e == "hypot") {
                if (!set(2)) return;
                push(std::hypot(t[1], t[0]));
            }
            else if (e == "ln") {
                if (!set(1)) return;
                push(qLn(t[0]));
            }
            else if (e == "log") {
                if (!set(1)) return;
                push(std::log10(t[0]));
            }
            else if (e == "lb") {
                if (!set(1)) return;
                push(std::log2(t[0]));
            }
            else if (e == "**" || e == "pow") {
                if (!set(2)) return;
                push(qPow(t[1], t[0]));
            }
            else if (e == "rtd") {
                if (!set(1)) return;
                push(qRadiansToDegrees(t[0]));
            }
            else if (e == "sin") {
                if (!set(1)) return;
                push(qSin(t[0]));
            }
            else if (e == "sinh") {
                if (!set(1)) return;
                push(std::sinh(t[0]));
            }
            else if (e == "sqrt") {
                if (!set(1)) return;
                push(qSqrt(t[0]));
            }
            else if (e == "tan") {
                if (!set(1)) return;
                push(qTan(t[0]));
            }
            else if (e == "tanh") {
                if (!set(1)) return;
                push(std::tanh(t[0]));
            }
            else if (e == "gamma") {
                if (!set(1)) return;
                push(std::tgamma(t[0]));
            }
            else if (e == "~" || e == "not") {
                if (!set(1)) return;
                if (is_integer(t[0])) {
                    push(~(int)t[0]);
                }
                else {
                    result->setText("The operand of ~/not is not integer.");
                    return;
                }
            }
            else if (e == "&" || e == "and") {
                if (!set(2)) return;
                if (is_integer(t[0])
                 && is_integer(t[1])) {
                    push((int)t[1] & (int)t[0]);
                }
                else {
                    result->setText("The operand of &/and is not integer.");
                    return;
                }
            }
            else if (e == "|" || e == "or") {
                if (!set(2)) return;
                if (is_integer(t[0])
                 && is_integer(t[1])) {
                    push((int)t[1] | (int)t[0]);
                }
                else {
                    result->setText("The operand of |/or is not integer.");
                    return;
                }
            }
            else if (e == "^" || e == "xor") {
                if (!set(2)) return;
                if (is_integer(t[0])
                 && is_integer(t[1])) {
                    push((int)t[1] ^ (int)t[0]);
                }
                else {
                    result->setText("The operand of ^/xor is not integer.");
                    return;
                }
            }
            else if (e == "<<") {
                if (!set(2)) return;
                if (is_integer(t[0])
                 && is_integer(t[1])) {
                    push((int)t[1] << (int)t[0]);
                }
                else {
                    result->setText("The operand of << is not integer.");
                    return;
                }
            }
            else if (e == ">>") {
                if (!set(2)) return;
                if (is_integer(t[0])
                 && is_integer(t[1])) {
                    push((int)t[1] >> (int)t[0]);
                }
                else {
                    result->setText("The operand of >> is not integer.");
                    return;
                }
            }
            else if (e == "pi") {
                push(M_PI);
            }
            else if (e == "e" && base == 10) {
                push(M_E);
            }
            else if (e == "unix") {
                push(time(NULL));
            }
            else if (e == "bin") {
                int base_old = base;
                base = 2;
                convert_base(base_old, base, elements);
                return;
            }
            else if (e == "oct") {
                int base_old = base;
                base = 8;
                convert_base(base_old, base, elements);
                return;
            }
            // decだと16進数の数字と判断されてしまい16進数モードから切り替えられない
            else if (e == "deci") {
                int base_old = base;
                base = 10;
                convert_base(base_old, base, elements);
                return;
            }
            else if (e == "hex") {
                int base_old = base;
                base = 16;
                convert_base(base_old, base, elements);
                return;
            }
            else if (e == "help" || e == "hh") {
                exprs.remove("help");
                exprs.remove("hh");
                set_input_box_text(exprs);
                // https://doc.qt.io/qt-6/application-windows.html#window-geometry
                help_dialog.setGeometry(
                    this->geometry().x(),
                    this->geometry().y() + this->frameGeometry().height(),
                    1,  // 無視されるからなんでもよい
                    1   // 無視されるからなんでもよい
                );
                // exec()だとモーダルウヰンドウになってしまう
                help_dialog.show();
                // このreturnがないとinvalidで上書きされる
                return;
            }
            else if (e == "aboutqt") {
                exprs.remove("aboutqt");
                set_input_box_text(exprs);
                QMessageBox msgbox;
                msgbox.aboutQt(this, "About Qt");
                return;
            }
            else {
                result->setText("Invalid Operator: " + e);
                return;
            }
        }
    }
    if (stack.isEmpty()) {
        push(0);
    }
    if (stack.size() != 1) {
        result->setText("");
        return;
    }
    if (base == 10) {
        result->setText("0d " + QString::number(stack.back()));
    }
    else if (base == 2) {
        result->setText("0b " + QString::number((int)stack.back(), base));
    }
    else if (base == 8) {
        result->setText("0o " + QString::number((int)stack.back(), base));
    }
    else {
        result->setText("0x " + QString::number((int)stack.back(), base));
    }
}

