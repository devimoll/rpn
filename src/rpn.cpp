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
    QString input_text = input->toPlainText();
    QString shiki(input_text);
    shiki.replace("\n", " ");
    QStringList l = shiki.split(" ");
    l.removeAll("");
    qDebug() << base << l;
    for (const QString s : l) {
        bool ok;
        double x;
        if (base == 10) {
            x = s.toDouble(&ok);
        }
        else {
            x = s.toInt(&ok, base);
        }

        if (ok) {
            push(x);
        }
        else {
            if (s == "+") {
                if (!set(2)) return;
                push(t[1] + t[0]);
            }
            else if (s == "-") {
                if (!set(2)) return;
                push(t[1] - t[0]);
            }
            else if (s == "*") {
                if (!set(2)) return;
                push(t[1] * t[0]);
            }
            else if (s == "/") {
                if (!set(2)) return;
                push(t[1] / t[0]);
            }
            else if (s == "%" || s == "//") {
                if (!set(2)) return;
                push(fmod(t[1], t[0]));
            }
            else if (s == "sqrt") {
                if (!set(1)) return;
                push(qSqrt(t[0]));
            }
            else if (s == "ln") {
                if (!set(1)) return;
                push(qLn(t[0]));
            }
            else if (s == "~" || s == "not") {
                if (!set(1)) return;
                if (std::floor(t[0]) == t[0]) {
                    push(~(int)t[0]);
                }
                else {
                    result->setText("The operand of ~ is not integer.");
                    return;
                }
            }/*
            else if (s == "&" || s == "and") {
                set(2);
                //push(t[1] ^ t[0]);
            }
            else if (s == "^" || s == "xor") {
                set(2);
            }
            else if (s == "<<") {
                set(2);
            }*/
            else if (s == "**" || s == "pow") {
                if (!set(2)) return;
                push(qPow(t[1], t[0]));
            }
            else if (s == "pi") {
                push(M_PI);
            }
            else if (s == "unix") {
                push(time(NULL));
            }
            else if (s == "bin") {
                base = 2;
                input->setText("");
                return;
            }
            else if (s == "oct") {
                base = 8;
                input->setText("");
                return;
            }
            // dec dato 16sinsuuno suuji dakara 16sinsuu kara kirikaerarenai.
            else if (s == "deci") {
                base = 10;
                input->setText("");
                return;
            }
            else if (s == "hex") {
                int base_old = base;
                bool okok;
                base = 16;
                QString str("");
                /* listを1個ずつ読む
                 * 数値なら前のbaseで変数にtointしてqstring::numberで新たなbaseを指定してテキストボックスに追加
                 * 演算子ならそのままテキストボックスに追加
                 * 之で良い気がする*/
                for (int i = 0; i < l.size() - 1; i++) {
                    if (base_old == 10) {
                        x = l[i].toDouble(&okok);
                    }
                    else {
                        x = l[i].toInt(&okok, base_old);
                    }
                    if (okok) {
                        if (base == 10) {
                            str += QString::number(x);
                        }
                        else {
                            str += QString::number((int)x, base);
                        }
                    }
                    else {
                        str += l[i];
                    }
                    str += " ";
                }

                input->setText(str);
                return;
            }
            else if (s == "help" || s == "hh") {
                input_text.remove("help");
                input_text.remove("hh");
                qDebug() << "help";
                QTextCursor tc = input->textCursor();
                // よくわからないが1を引かないとエラーが出る
                tc.setPosition(input->document()->characterCount() - 1);
                input->setText(input_text);
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

