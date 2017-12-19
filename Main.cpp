/*

Copyright (c) 2017 Д.С. Андреев

Этот файл — часть jam.

jam - свободная программа: вы можете перераспространять ее и/или
изменять ее на условиях Стандартной общественной лицензии GNU в том виде,
в каком она была опубликована Фондом свободного программного обеспечения;
либо версии 3 лицензии, либо (по вашему выбору) любой более поздней
версии.

Письмуй распространяется в надежде, что она будет полезной,
но БЕЗО ВСЯКИХ ГАРАНТИЙ; даже без неявной гарантии ТОВАРНОГО ВИДА
или ПРИГОДНОСТИ ДЛЯ ОПРЕДЕЛЕННЫХ ЦЕЛЕЙ. Подробнее см. в Стандартной
общественной лицензии GNU.

Вы должны были получить копию Стандартной общественной лицензии GNU
вместе с этой программой. Если это не так, см.
<http://www.gnu.org/licenses/>.)


This file is part of jam.

jam is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Composer is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Composer.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <QtCore>
#include <QtConcurrent>

typedef QList<QStringList> StringListList;

bool executeCommand(const QStringList & in) {
    if(in.isEmpty() == true) { return false; }

    QStringList arguments = in;
    QString command = arguments.takeFirst();

    QProcess process;
    process.start(command,arguments);

    if(process.waitForStarted(-1) == false) { return false; }
    if(process.waitForFinished(-1) == false) { return false; }

    QByteArray out_err = process.readAllStandardError();
    if(out_err.isEmpty() == false) { fwrite(out_err.constData(),out_err.length(),1,stderr); }

    QByteArray out_std = process.readAllStandardOutput();
    if(out_std.isEmpty() == false) { fwrite(out_std.constData(),out_std.length(),1,stdout); }

    return true;
    }

StringListList separatedList(const QStringList & in) {
    StringListList out;
    QStringList cache;

    for(int i=0; i<in.count(); i++) {
        const QString & current = in.at(i);
        if(current == "---") { if(cache.isEmpty() == false) { out.append(cache); cache.clear(); } }
        else { if(current.isEmpty() == false) { cache.append(current); } }
        }

    if(cache.isEmpty() == false) { out.append(cache); }

    return out;
    }

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
#ifdef AM_USING_QT4
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#endif
    QStringList raw_arguments = app.arguments();
    raw_arguments.removeFirst();

    StringListList separated_arguments = separatedList(raw_arguments);
    if(separated_arguments.isEmpty() == true) {
        fprintf(stderr,"Usage: jam <cmd> [arg] ... [arg_n] [ --- <cmd> [arg] ... [arg_n] ]\r\n");
        return EXIT_FAILURE;
        }

    QFuture<void> future = QtConcurrent::map(separated_arguments,executeCommand);
    future.waitForFinished();

    return EXIT_SUCCESS;
    }
