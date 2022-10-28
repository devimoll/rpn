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
}

rpn::~rpn()
{
    delete layout;
    delete input;
    delete result;
}

double rpn::pop() {
    if (stack.isEmpty()) {
        qDebug() << "stack is empty but tried pop().";
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

void rpn::input_changed()
{
    stack.clear();
    stack.shrink_to_fit();
    const QString input_text = input->toPlainText();
    QString exprs(input_text);
    exprs.replace("\n", " ");
    QStringList elements = exprs.split(" ");
    elements.removeAll("");
    qDebug() << base << elements;
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
            else if (e == "sqrt") {
                if (!set(1)) return;
                push(qSqrt(t[0]));
            }
            else if (e == "ln") {
                if (!set(1)) return;
                push(qLn(t[0]));
            }
            else if (e == "~" || e == "not") {
                if (!set(1)) return;
                if (std::floor(t[0]) == t[0]) {
                    push(~(int)t[0]);
                }
                else {
                    result->setText("The operand of ~ is not integer.");
                    return;
                }
            }/*
            else if (e == "&" || e == "and") {
                set(2);
                //push(t[1] ^ t[0]);
            }
            else if (e == "^" || e == "xor") {
                set(2);
            }
            else if (e == "<<") {
                set(2);
            }*/
            else if (e == "**" || e == "pow") {
                if (!set(2)) return;
                push(qPow(t[1], t[0]));
            }
            else if (e == "pi") {
                push(M_PI);
            }
            else if (e == "unix") {
                push(time(NULL));
            }
            else if (e == "bin") {
                base = 2;
                input->setText("");
                return;
            }
            else if (e == "oct") {
                base = 8;
                input->setText("");
                return;
            }
            // dec dato 16sinsuuno suuji dakara 16sinsuu kara kirikaerarenai.
            else if (e == "deci") {
                base = 10;
                input->setText("");
                return;
            }
            else if (e == "hex") {
                int base_old = base;
                base = 16;
                exprs = "";
                /* listを1個ずつ読む
                 * 数値なら前のbaseで変数にtointしてqstring::numberで新たなbaseを指定してテキストボックスに追加
                 * 演算子ならそのままテキストボックスに追加
                 * 之で良い気がする*/
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

                input->setText(exprs);
                return;
            }
            else if (e == "help" || e == "hh") {
                exprs.remove("help");
                exprs.remove("hh");
                qDebug() << "help";
                QTextCursor tc = input->textCursor();
                // よくわからないが1を引かないとエラーが出る
                tc.setPosition(input->document()->characterCount() - 1);
                input->setText(exprs);
                // この行はinput->setText(input_text);より下に無いとinvaliedで上書きされる
                result->setText("+, -, *, /, sqrt, ln");
                input->setTextCursor(tc);
                // このreturnがないとinvalidで上書きされる
                return;
            }
            else {
                result->setText("Invalid Operator");
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
        result->setText(QString::number(stack.back()));
    }
    else {
        result->setText(QString::number((int)stack.back(), base));
    }
}

