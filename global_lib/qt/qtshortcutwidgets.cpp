#include "qtshortcutwidgets.h"

int myShortcut::partToKey(QString part) {
    QString p=part.toLower();
    if (p=="a") return Qt::Key_A;
    if (p=="b") return Qt::Key_B;
    if (p=="c") return Qt::Key_C;
    if (p=="d") return Qt::Key_D;
    if (p=="e") return Qt::Key_E;
    if (p=="f") return Qt::Key_F;
    if (p=="g") return Qt::Key_G;
    if (p=="h") return Qt::Key_H;
    if (p=="i") return Qt::Key_I;
    if (p=="j") return Qt::Key_J;
    if (p=="k") return Qt::Key_K;
    if (p=="l") return Qt::Key_L;
    if (p=="m") return Qt::Key_M;
    if (p=="n") return Qt::Key_N;
    if (p=="o") return Qt::Key_O;
    if (p=="p") return Qt::Key_P;
    if (p=="q") return Qt::Key_Q;
    if (p=="r") return Qt::Key_R;
    if (p=="s") return Qt::Key_S;
    if (p=="t") return Qt::Key_T;
    if (p=="u") return Qt::Key_U;
    if (p=="v") return Qt::Key_V;
    if (p=="w") return Qt::Key_W;
    if (p=="x") return Qt::Key_X;
    if (p=="y") return Qt::Key_Y;
    if (p=="z") return Qt::Key_Z;
    if (p=="0") return Qt::Key_0;
    if (p=="1") return Qt::Key_1;
    if (p=="2") return Qt::Key_2;
    if (p=="3") return Qt::Key_3;
    if (p=="4") return Qt::Key_4;
    if (p=="5") return Qt::Key_5;
    if (p=="6") return Qt::Key_6;
    if (p=="7") return Qt::Key_7;
    if (p=="8") return Qt::Key_8;
    if (p=="9") return Qt::Key_9;
    if (p=="f1") return Qt::Key_F1;
    if (p=="f2") return Qt::Key_F2;
    if (p=="f3") return Qt::Key_F3;
    if (p=="f4") return Qt::Key_F4;
    if (p=="f5") return Qt::Key_F5;
    if (p=="f6") return Qt::Key_F6;
    if (p=="f7") return Qt::Key_F7;
    if (p=="f8") return Qt::Key_F8;
    if (p=="f9") return Qt::Key_F9;
    if (p=="f10") return Qt::Key_F10;
    if (p=="f11") return Qt::Key_F11;
    if (p=="f12") return Qt::Key_F12;
    if (p=="f13") return Qt::Key_F13;
    if (p=="f14") return Qt::Key_F14;
    if (p=="f15") return Qt::Key_F15;
    if (p=="f16") return Qt::Key_F16;
    if (p=="f17") return Qt::Key_F17;
    if (p=="f18") return Qt::Key_F18;
    if (p=="f19") return Qt::Key_F19;
    if (p=="f20") return Qt::Key_F20;
    if (p=="esc") return Qt::Key_Escape;
    if (p=="escape") return Qt::Key_Escape;
    if (p=="tab") return Qt::Key_Tab;
    if (p=="backtab") return Qt::Key_Backtab;
    if (p=="backspace") return Qt::Key_Backspace;
    if (p=="return") return Qt::Key_Return;
    if (p=="enter") return Qt::Key_Enter;
    if (p=="insert") return Qt::Key_Insert;
    if (p=="ins") return Qt::Key_Insert;
    if (p=="einfg") return Qt::Key_Insert;
    if (p=="del") return Qt::Key_Delete;
    if (p=="delete") return Qt::Key_Delete;
    if (p=="entf") return Qt::Key_Delete;
    if (p=="home") return Qt::Key_Home;
    if (p=="pos1") return Qt::Key_Home;
    if (p=="end") return Qt::Key_End;
    if (p=="ende") return Qt::Key_End;
    if (p=="right") return Qt::Key_Right;
    if (p=="rechts") return Qt::Key_Right;
    if (p=="left") return Qt::Key_Left;
    if (p=="links") return Qt::Key_Left;
    if (p=="up") return Qt::Key_Up;
    if (p=="hoch") return Qt::Key_Up;
    if (p=="oben") return Qt::Key_Up;
    if (p=="down") return Qt::Key_Down;
    if (p=="runter") return Qt::Key_Down;
    if (p=="unten") return Qt::Key_Down;
    if (p=="pgup") return Qt::Key_PageUp;
    if (p=="pageup") return Qt::Key_PageUp;
    if (p=="pgdown") return Qt::Key_PageDown;
    if (p=="pagedown") return Qt::Key_PageDown;
    if (p=="space") return Qt::Key_Space;
    if (p=="!") return Qt::Key_Exclam;
    if (p=="\"") return Qt::Key_QuoteDbl;
    if (p=="§") return Qt::Key_Dollar;
    if (p=="%") return Qt::Key_Percent;
    if (p=="&") return Qt::Key_Ampersand;
    if (p=="/") return Qt::Key_Slash;
    if (p=="(") return Qt::Key_ParenLeft;
    if (p==")") return Qt::Key_ParenRight;
    if (p=="=") return Qt::Key_Equal;
    if (p=="?") return Qt::Key_Question;
    if (p=="\'") return Qt::Key_Apostrophe;
    if (p=="+") return Qt::Key_Plus;
    if (p=="-") return Qt::Key_Minus;
    if (p==",") return Qt::Key_Comma;
    if (p==".") return Qt::Key_Period;
    if (p==";") return Qt::Key_Semicolon;
    if (p=="<") return Qt::Key_Less;
    if (p==">") return Qt::Key_Greater;
    if (p==":") return Qt::Key_Colon;
    if (p=="@") return Qt::Key_At;
    if (p=="*") return Qt::Key_Asterisk;
    if (p=="~") return Qt::Key_AsciiTilde;
    if (p=="{") return Qt::Key_BraceLeft;
    if (p=="}") return Qt::Key_BraceRight;
    if (p=="[") return Qt::Key_BracketLeft;
    if (p=="]") return Qt::Key_BracketRight;
    if (p=="_") return Qt::Key_Underscore;
    if (p=="°") return Qt::Key_AsciiCircum;
    if (p=="#") return Qt::Key_ssharp;
    //if (p=="") return Qt::Key_;
    return Qt::Key_unknown;
}

myShortcut::myShortcut(QString keys, QString id) {
    QString part="";
    QList<QString> partList;
    for (int i=0; i<keys.size(); i++) {
        //std::cout<<"char="<<QString(keys[i]).toStdString()<<"    part="<<part.toStdString()<<std::endl;
        if (keys[i]==QChar('+')) {
            if (part.isEmpty()) {
                part=part+"+";
            } else {
                //std::cout<<"part = '"<<part.toStdString()<<"'\n";
                partList.append(part);
                part="";
            }
        } else {
            part=part+keys[i];
        }
    }
    if (!part.isEmpty()) partList.append(part);

    //std::cout<<partList.size()<<std::endl;
    if (partList.size()==0) return;
    if (partList.size()==1) {
        key=partToKey(partList[0]);
    } else {
        key=partToKey(partList[partList.size()-1]);
        modifiers=Qt::NoModifier;
        for (int i=0; i<partList.size()-1; i++) {
            QString p=partList[i].toLower();
            if (p=="shift") modifiers=modifiers|Qt::ShiftModifier;
            else if (p=="umschalt") modifiers=modifiers|Qt::ShiftModifier;
            else if (p=="alt") modifiers=modifiers|Qt::AltModifier;
            else if (p=="strg") modifiers=modifiers | Qt::ControlModifier;
            else if (p=="ctrl") modifiers=modifiers | Qt::ControlModifier;
        }
    }
    this->ID=id;
    if (id.isEmpty()) this->ID=keys;
}

bool myShortcut::matches(QKeyEvent* event) {
    //std::cout<<"matching "<<event->key()<<" =?= "<<key<<"   and   "<<event->modifiers()<<" =?= "<<modifiers<<std::endl;
    return (event->key()==key) && (event->modifiers()==modifiers);
}









void ShortcutQSpinBox::keyPressEvent (QKeyEvent* event) {
    bool found=false;
    for (int i=0; i<shortcuts.size(); i++) {
        if (shortcuts[i].matches(event)) {
            emit keyPressed(shortcuts[i].ID);
            //std::cout<<"spinbox emited "<<shortcuts[i].ID.toStdString()<<std::endl;
            found=true;
            break;
        }
    }
    if (!found) QSpinBox::keyPressEvent(event);
}



void ShortcutQDoubleSpinBox::keyPressEvent (QKeyEvent* event) {
    bool found=false;
    for (int i=0; i<shortcuts.size(); i++) {
        if (shortcuts[i].matches(event)) {
            emit keyPressed(shortcuts[i].ID);
            //std::cout<<"doublespinbox emited "<<shortcuts[i].ID.toStdString()<<std::endl;
            found=true;
            break;
        }
    }
    if (!found) QDoubleSpinBox::keyPressEvent(event);
}


void ShortcutNumberEdit::keyPressEvent (QKeyEvent* event) {
    bool found=false;
    for (int i=0; i<shortcuts.size(); i++) {
        if (shortcuts[i].matches(event)) {
            emit keyPressed(shortcuts[i].ID);
            //std::cout<<"doubleedit emited "<<shortcuts[i].ID.toStdString()<<std::endl;
            found=true;
            break;
        }
    }
    if (!found) NumberEdit::keyPressEvent(event);
}



void ShortcutQComboBox::keyPressEvent (QKeyEvent* event) {
    bool found=false;
    for (int i=0; i<shortcuts.size(); i++) {
        if (shortcuts[i].matches(event)) {
            emit keyPressed(shortcuts[i].ID);
            //std::cout<<"combobox emited "<<shortcuts[i].ID.toStdString()<<std::endl;
            found=true;
            break;
        }
    }
    if (!found) QComboBox::keyPressEvent(event);
}


void ShortcutQLineEdit::keyPressEvent (QKeyEvent* event) {
    bool found=false;
    for (int i=0; i<shortcuts.size(); i++) {
        if (shortcuts[i].matches(event)) {
            emit keyPressed(shortcuts[i].ID);
            //std::cout<<"combobox emited "<<shortcuts[i].ID.toStdString()<<std::endl;
            found=true;
            break;
        }
    }
    if (!found) QLineEdit::keyPressEvent(event);
}

