#include "lexer.h"
#include "lexer_p.h"
#include "interfaces/lexemtype.h"
#include "errormessages/errormessages.h"


using namespace Shared;

namespace KumirAnalizer {

Lexer::Lexer(QObject *parent) :
    QObject(parent)
{
    d = new LexerPrivate;
    d->q = this;
}

int Lexer::splitIntoStatements(const QString &text
                                , int baseLineNo
                                , QList<Statement> &statements
                                ) const
{
    int errorsCount = 0;
    const QStringList lines = text.split("\n");
    for (int i=0; i<lines.size(); i++) {
        const QString line = lines[i];
        QList<Lexem*> lexems;
        d->splitLineIntoLexems(line, lexems);
        QList<Statement> sts;
        d->groupLexemsByStatements(lexems, sts);
        for (int j=0; j<sts.size(); j++) {
            for (int k=0; k<sts[j].data.size(); k++)
                sts[j].data[k]->lineNo = baseLineNo + i;
        }
        statements << sts;
    }
    return errorsCount;
}

QStringList allVariants(const QString & value) {
    QStringList variants = value.split("|");
    QStringList result;
    foreach (QString variant, variants) {
        variant.remove("\\s+");
        variant.remove("\\s*");
        variant.remove("\\s");
        variant.remove("_");
        result << variant;
    }
    return result;
}

void addToMap(QHash<QString,LexemType> & map,
              const QString &value, const LexemType type)
{
    QStringList variants = allVariants(value);
    foreach (QString variant, variants) {
        map[variant] = type;
    }
}

QString Lexer::classNameByBaseType(const AST::VariableBaseType &type) const
{
    QString result;
    for (int i=0; i<d->baseTypes.keys().size(); i++) {
        if (d->baseTypes[d->baseTypes.keys()[i]]==type) {
            result = d->baseTypes.keys()[i];
            break;
        }
    }
    return result;
}

void Lexer::setLanguage(const QLocale::Language &language)
{
    const QString langName = QLocale::languageToString(language);
    const QString resourcesRoot = qApp->property("sharePath").toString()+"/kumiranalizer/";
    const QString fileName = resourcesRoot+langName.toLower()+".keywords";
    LexerPrivate::initNormalizator(fileName);
}

void LexerPrivate::initNormalizator(const QString &fileName)
{
    QFile kwdFile(fileName);
    if (kwdFile.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QTextStream ts(&kwdFile);
        ts.setCodec("UTF-8");
        const QStringList lines = ts.readAll().split("\n",QString::SkipEmptyParts);
        kwdFile.close();
        foreach (const QString &line, lines) {
            if (line.startsWith("#"))
                continue;
            const QStringList pair = line.split("=");
            if (pair.size()>=2) {
                const QString context = pair.at(0).simplified();
                const QString value = pair.at(1).simplified();
                if (context=="begin module") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxPriModule);
                }
                else if (context=="end module") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxPriEndModule);
                }
                else if (context=="include text") {
//                    keyWords << value;
//                    addToMap(kwdMap, value, KS_INCLUDE);
                }
                else if (context=="use module") {
                    keyWords << value;
//                    useKwd = new QString(value);
                    addToMap(kwdMap, value, LxPriImport);
                }
                else if (context=="function return value") {
                    retvalKeyword = value;
                }
                else if (context=="algorhitm header") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxPriAlgHeader);
                }
                else if (context=="begin algorhitm implementation") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxPriAlgBegin);
                }
                else if (context=="end algorhitm implementation") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxPriAlgEnd);
                }
                else if (context=="algorhitm pre-condition") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxPriPre);
                }
                else if (context=="algorhitm post-condition") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxPriPost);
                }
                else if (context=="assertion") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxPriAssert);
                }
                else if (context=="begin loop") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxPriLoop);
                }
                else if (context=="end loop") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxPriEndLoop);
                }
                else if (context=="conditional end loop") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxPriEndLoop);
                }
                else if (context=="'for' loop type") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxSecFor);
                }
                else if (context=="'while' loop type") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxSecWhile);
                }
                else if (context=="'times' loop type") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxSecTimes);
                }
                else if (context=="for loop 'from'") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxSecFrom);
                }
                else if (context=="for loop 'to'") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxSecTo);
                }
                else if (context=="for loop 'step'") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxSecStep);
                }
                else if (context=="loop break and algorhitm return") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxPriExit);
                }
                else if (context=="if") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxPriIf);
                }
                else if (context=="then") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxPriThen);
                }
                else if (context=="else") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxPriElse);
                }
                else if (context=="end of 'if' or 'switch' statement") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxPriFi);
                }
                else if (context=="switch") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxPriSwitch);
                }
                else if (context=="case") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxPriCase);
                }
                else if (context=="terminal input") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxPriInput);
                }
                else if (context=="terminal output") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxPriOutput);
                }
                else if (context=="file input") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxPriFinput);
                }
                else if (context=="file output") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxPriFoutput);
                }
                else if (context=="new line symbol") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxSecNewline);
                }
                else if (context=="logical 'not'") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxSecNot);
                }
                else if (context=="logical 'and'") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxSecAnd);
                }
                else if (context=="logical 'or'") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxSecOr);
                }
                else if (context=="'write only' parameter modifier") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxSecOut);
                }
                else if (context=="'read only' parameter modifier") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxSecIn);
                }
                else if (context=="'read and write' parameter modifier") {
                    keyWords << value;
                    addToMap(kwdMap, value, LxSecInout);
                }
                else if (context=="'true' constant value") {
                    constNames << value;
                    boolConstantValues.insert(value, true);
                    addToMap(kwdMap, value, LxConstBoolTrue);
                }
                else if (context=="'false' constant value") {
                    constNames << value;
                    boolConstantValues.insert(value, false);
                    addToMap(kwdMap, value, LxConstBoolFalse);
                }
                else if (context=="integer type name") {
                    typeNames << value;
                    baseTypes.insert(value, AST::TypeInteger);
                    addToMap(kwdMap, value, LxNameClass);
                }
                else if (context=="floating point type name") {
                    typeNames << value;
                    baseTypes.insert(value, AST::TypeReal);
                    addToMap(kwdMap, value, LxNameClass);
                }
                else if (context=="character type name") {
                    typeNames << value;
                    baseTypes.insert(value, AST::TypeCharect);
                    addToMap(kwdMap, value, LxNameClass);
                }
                else if (context=="string type name") {
                    typeNames << value;
                    baseTypes.insert(value, AST::TypeString);
                    addToMap(kwdMap, value, LxNameClass);
                }
                else if (context=="boolean type name") {
                    typeNames << value;
                    baseTypes.insert(value, AST::TypeBoolean);
                    addToMap(kwdMap, value, LxNameClass);
                }
                else if (context=="integer array type name") {
                    QStringList variants = allVariants(value);
                    foreach (const QString & variant, variants) {
                        typeNames << variant;
                        baseTypes.insert(variant, AST::TypeInteger);
                        arrayTypes.insert(variant);
                    }
                    addToMap(kwdMap, value, LxNameClass);
                }
                else if (context=="floating point array type name") {
                    QStringList variants = allVariants(value);
                    foreach (const QString & variant, variants) {
                        typeNames << variant;
                        baseTypes.insert(variant, AST::TypeReal);
                        arrayTypes.insert(variant);
                    }
                    addToMap(kwdMap, value, LxNameClass);
                }
                else if (context=="character array type name") {
                    QStringList variants = allVariants(value);
                    foreach (const QString & variant, variants) {
                        typeNames << variant;
                        baseTypes.insert(variant, AST::TypeCharect);
                        arrayTypes.insert(variant);
                    }
                    addToMap(kwdMap, value, LxNameClass);
                }
                else if (context=="string array type name") {
                    QStringList variants = allVariants(value);
                    foreach (const QString & variant, variants) {
                        typeNames << variant;
                        baseTypes.insert(variant, AST::TypeString);
                        arrayTypes.insert(variant);
                    }
                    addToMap(kwdMap, value, LxNameClass);
                }
                else if (context=="boolean array type name") {
                    QStringList variants = allVariants(value);
                    foreach (const QString & variant, variants) {
                        typeNames << variant;
                        baseTypes.insert(variant, AST::TypeBoolean);
                        arrayTypes.insert(variant);
                    }
                    addToMap(kwdMap, value, LxNameClass);
                }

            }
        }
    }

    operators << "\\+";
    kwdMap["+"] = LxOperPlus;

    operators << "-";
    kwdMap["-"] = LxOperMinus;

    operators << "\\*\\*";
    kwdMap["**"] = LxOperPower;

    operators << "\\(";
    kwdMap["("] = LxOperLeftBr;

    operators << "\\)";
    kwdMap[")"] = LxOperRightBr;

    operators << "\\[";
    kwdMap["["] = LxOperLeftSqBr;

    operators << "\\]";
    kwdMap["]"] = LxOperRightSqBr;

    operators << ",";
    kwdMap[","] = LxOperComa;

    operators << ":";
    kwdMap[":"] = LxOperColon;

    operators << "\\*";
    kwdMap["*"] = LxOperAsterisk;

    operators << ">=";
    kwdMap[">="] = LxOperGreaterOrEqual;

    operators << "<=";
    kwdMap["<="] = LxOperLessOrEqual;

    operators << "<>";
    kwdMap["<>"] = LxOperNotEqual;

    operators << "=";
    kwdMap["="] = LxOperEqual;

    operators << "<";
    kwdMap["<"] = LxOperLess;

    operators << ">";
    kwdMap[">"] = LxOperGreater;

    operators << "#";


    operators << ":=";
    kwdMap[":="] = LxPriAssign;

//    operators << "\\~";
//    kwdMap["~"] = QString(QChar(KS_TILDA));

//    operators << "&";
//    kwdMap["&"] = QString(QChar(KS_PAR));

//    operators << "%";
//    kwdMap["%"] = QString(QChar(KS_PERCENT));

    operators << "\"";
    operators << "'";
//    operators << QString::fromUtf8("”");
    operators << "\\|";

    compounds += keyWords;
    compounds += typeNames;
    compounds += constNames;

//    keyWords << tr("value");
    for (int i=0; i<compounds.size(); i++) {
        compounds[i] = "\\b("+compounds[i]+")\\b";
    }

//    QString operatorsPattern = operators.join("|");

    QString compoundsPattern = compounds.join("|");

    QString keywordsPattern = "\\b"+keyWords.join("|")+"\\b";
    keywordsPattern.replace("|","\\b|\\b");

    QString typesPattern = "\\b"+typeNames.join("|")+"\\b";
    typesPattern.replace("|","\\b|\\b");

    QString constsPattern = "\\b"+constNames.join("|")+"\\b";
    constsPattern.replace("|","\\b|\\b");

    rxCompound = QRegExp(operators.join("|")+ "|"+
                             compoundsPattern);
    rxTypes =  QRegExp(typesPattern);
    rxConst = QRegExp(constsPattern);
    rxKeyWords = QRegExp(keywordsPattern);

    rxKeyWords.setMinimal(false);
    rxCompound.setMinimal(false);
}

bool isDecimalIntegerConstant(const QString &s) {
    bool result = s.length()>0;
    for (int i=0; i<s.length(); i++) {
        result = result && s[i].isDigit();
        if (!result)
            break;
    }
    return result;
}

bool isDecimalRealConstant(const QString &s) {
    bool result = s.length()>0;
    bool dotFound = false;
    for (int i=0; i<s.length(); i++) {
        if (s[i]=='.') {
            if (!dotFound) {
                dotFound = true;
            }
            else {
                result = false;
                break;
            }
        }
        else if (!s[i].isDigit()) {
            result = false;
            break;
        }
    }
    return result;
}

static const QString expFormSymbols = QString::fromUtf8("eEеЕ01234567890");
static const QString hexFormSymbols = QString::fromAscii("0123456789ABCDEFabcdef");

bool isExpRealConstant(const QString &s) {
    bool result = s.length()>0 && s[0].isDigit();
    bool dotFound = false;
    for (int i=0; i<s.length(); i++) {
        if (s[i]=='.') {
            if (!dotFound) {
                dotFound = true;
            }
            else {
                result = false;
                break;
            }
        }
        else if (!expFormSymbols.contains(s[i])) {
            result = false;
            break;
        }
    }
    return result;
}

bool isHexIntegerConstant(const QString &s)  {
    bool result = s.length()>1 && s[0]=='$';
    for (int i=1; i<s.length(); i++) {
        result = result && hexFormSymbols.contains(s[i]);
        if (!result)
            break;
    }
    return result;
}


void searchNumericConstants(QList<Lexem*> & lexems) {
    QList<Lexem*>::iterator it = lexems.begin();
    QList<Lexem*>::iterator itt;
    while (it!=lexems.end()) {
        if ( (*it)->type==LxTypeName ) {
            itt = it + 1;
            const QString s = (*it)->data;
            if (isDecimalIntegerConstant(s) || isHexIntegerConstant(s)) {
                (*it)->type=LxConstInteger;
            }
            else if (isDecimalRealConstant(s)) {
                (*it)->type=LxConstReal;
            }
            else if (isExpRealConstant(s)) {
                (*it)->type=LxConstReal;
                (*it)->data.replace(QString::fromUtf8("е"), "E");
                (*it)->data.replace(QString::fromUtf8("Е"), "E");
                (*it)->data.replace("e", "E");
                if ( (*it)->data[(*it)->data.length()]=='E' ) {
                    if (itt!=lexems.end()) {
                        if ( (*itt)->type==LxOperPlus || (*itt)->type==LxOperMinus ) {
                            (*it)->data += (*itt)->data;
                            (*it)->length += (*itt)->length;
                            delete *itt;
                            itt = lexems.erase(itt);
                            if (itt!=lexems.end()) {
                                const QString ss = (*itt)->data;
                                if (isDecimalRealConstant(ss)) {
                                    (*it)->data += (*itt)->data;
                                    (*it)->length += (*itt)->length;
                                    delete *itt;
                                    lexems.erase(itt);
                                }
                            }
                        }
                    }
                }
                else {
                    if ( !(*it)->data.contains("+") && !(*it)->data.contains("-") ) {
                        (*it)->data.replace("E","E+");
                    }
                }
            }
        }
        it ++;
    }
}

void LexerPrivate::splitLineIntoLexems(const QString &text
                                       , QList<Lexem*> &lexems
                                       ) const
{
    lexems.clear();
    Q_ASSERT(rxCompound.isValid());
    bool inLit = false;
    QChar litSimb;
    int cur = 0;
    int prev = -1;
    if (text.isEmpty()) {
        return;
    }
    forever {
        cur = rxCompound.indexIn(text, qMax(0,prev));
        if (cur!=-1) {
            if ( (cur-prev>1&&prev==-1) || (cur-prev>0&&prev>=0) ) {
                if (inLit) {
                    const QString app = text.mid(prev, cur-prev);
                    lexems.last()->data += app;
                }
                else {
                    Lexem * lx = new Lexem;
                    lx->linePos = qMax(prev, 0);
                    lx->type = LxTypeName;
                    lx->data = text.mid(qMax(prev,0), prev>=0? cur-prev : cur-prev-1);
                    lexems << lx;
                }
            }
            QString symb = rxCompound.cap(0);
            if (inLit) {
                if ( (symb=="\"" || symb=="'") && symb[0]==litSimb) {
                    inLit = false;
                }
                else {
                    lexems.last()->data += symb;
                }

            }
            else {
                if (symb=="|") {
                    Lexem * lx = new Lexem;
                    lx->type = LxTypeComment;
                    lx->data = text.mid(cur+1);
                    lx->linePos = cur+1;
                    lexems << lx;
                    break;
                }
                else if (symb=="#") {
                    Lexem * lx = new Lexem;
                    lx->type = LxTypeDoc;
                    lx->data = text.mid(cur+1);
                    lx->linePos = cur+1;
                    lexems << lx;
                    break;
                }
                else if (symb=="\"" || symb=="'") {
                    litSimb = symb[0];
                    Lexem * lx = new Lexem;
                    lx->type = LxConstLiteral;
                    lx->linePos = cur;
                    inLit = true;
                    lexems << lx;
                }
                else {
                    QString simplifiedSymb = symb;
                    simplifiedSymb.remove(' ');
                    simplifiedSymb.remove('_');
                    Lexem * lx = new Lexem;
                    lx->type = kwdMap[simplifiedSymb];
                    lx->data = symb;
                    lx->linePos = cur;
                    lexems << lx;
                }
            }
            prev = cur + rxCompound.matchedLength();
        } // end if cur!=-1
        else {
            if (inLit) {
                lexems.last()->data += text.mid(prev+1);
                lexems.last()->error = _("Unpaired quote");
            }
            else {
                Lexem * lx = lexems.last();
                if (lx->type==LxTypeComment || lx->type==LxTypeDoc)
                    lexems.last()->data += text.mid(prev+1);
                else {
                    if (prev<text.length()) {
                        Lexem * llx = new Lexem;
                        llx->type = LxTypeName;
                        llx->linePos = qMax(prev, 0);
                        llx->data = text.mid(prev);
                        lexems << llx;
                    }
                }
            }
            break;
        }
    }
    for (int i=0; i<lexems.size(); i++) {
        lexems[i]->length = lexems[i]->data.size();
        if (lexems[i]->type!=LxConstLiteral) {
            lexems[i]->data = lexems[i]->data.simplified();
        }
        else {
            lexems[i]->length += 2;
        }
    }
    QList<Lexem*>::iterator it = lexems.begin();
    while (it!=lexems.end()) {
        Lexem * lx = (*it);
        if (lx->data.isEmpty()) {
            delete lx;
            it = lexems.erase(it);
        }
        else {
            it ++;
        }
    }
    searchNumericConstants(lexems);
}

void popFirstStatement(QList<Lexem*> & lexems, Statement & result );
void popFirstStatementByKeyword(QList<Lexem*> & lexems, Statement & result );

void LexerPrivate::groupLexemsByStatements(
    const QList<Lexem*> &lexems
    , QList<Statement> &statements
    ) const
{
    QList<Lexem*> lexemsCopy = lexems;
    while (lexemsCopy.size()>0) {
        Statement statement;
        popFirstStatement(lexemsCopy, statement);
        if (statement.data.size()>0)
            statements << statement;
    }
}

void popFirstStatement(QList<Lexem*> & lexems, Statement & result )
{
    if (lexems.isEmpty())
        return;
    if (lexems[0]->type==LxOperSemicolon) {
        // Semicolons are delemiters. Just skip them.
        lexems.pop_front();
    }
    else if (lexems[0]->type==LxTypeComment || lexems[0]->type==LxTypeDoc) {
        // Comments and docstrings are newline-endian lexems
        result.data << lexems[0];
        result.type = lexems[0]->type;
        lexems.pop_front();
    }
    else if (lexems[0]->type & LxTypePrimaryKwd || lexems[0]->type==LxNameClass) {
        // Statement type can be determined by primary keyword
        popFirstStatementByKeyword(lexems, result);
    }
    else {
        // Generic assignment (or algothitm call) statement
        result.type = LxPriAssign;
        while (lexems.size()>0) {
            Lexem * lx = lexems[0];
            if (lx->type==LxOperSemicolon
                    || ( lx->type & LxTypePrimaryKwd && lx->type!=LxPriAssign )
                    || lexems[0]->type==LxNameClass) {
                // end of statement
                break;
            }
            lexems.pop_front();
            result.data << lx;
        }
    }
}

void popModuleStatement(QList<Lexem*> & lexems, Statement &result);
void popEndModuleStatement(QList<Lexem*> & lexems, Statement &result);
void popAlgHeaderStatement(QList<Lexem*> & lexems, Statement &result);
void popAlgBeginStatement(QList<Lexem*> & lexems, Statement &result);
void popAlgEndStatement(QList<Lexem*> & lexems, Statement &result);
void popPreStatement(QList<Lexem*> & lexems, Statement &result);
void popPostStatement(QList<Lexem*> & lexems, Statement &result);
void popIfStatement(QList<Lexem*> & lexems, Statement &result);
void popThenStatement(QList<Lexem*> & lexems, Statement &result);
void popElseStatement(QList<Lexem*> & lexems, Statement &result);
void popFiStatement(QList<Lexem*> & lexems, Statement &result);
void popSwitchStatement(QList<Lexem*> & lexems, Statement &result);
void popCaseStatement(QList<Lexem*> & lexems, Statement &result);
void popLoopStatement(QList<Lexem*> & lexems, Statement &result);
void popEndLoopStatement(QList<Lexem*> & lexems, Statement &result);
void popInputStatement(QList<Lexem*> & lexems, Statement &result);
void popOutputStatement(QList<Lexem*> & lexems, Statement &result);
void popFinputStatement(QList<Lexem*> & lexems, Statement &result);
void popFoutputStatement(QList<Lexem*> & lexems, Statement &result);
void popAssertStatement(QList<Lexem*> & lexems, Statement &result);
void popImportStatement(QList<Lexem*> & lexems, Statement &result);
void popExitStatement(QList<Lexem*> & lexems, Statement &result);
void popVarDeclStatement(QList<Lexem*> & lexems, Statement &result);

void popFirstStatementByKeyword(QList<Lexem*> &lexems, Statement &result)
{
    Q_ASSERT(!lexems.isEmpty());
    if (lexems[0]->type==LxPriModule) {
        popModuleStatement(lexems, result);
    }
    else if (lexems[0]->type==LxPriEndModule) {
        popEndModuleStatement(lexems, result);
    }
    else if (lexems[0]->type==LxPriAlgHeader) {
        popAlgHeaderStatement(lexems, result);
    }
    else if (lexems[0]->type==LxPriAlgBegin) {
        popAlgBeginStatement(lexems, result);
    }
    else if (lexems[0]->type==LxPriAlgEnd) {
        popAlgEndStatement(lexems, result);
    }
    else if (lexems[0]->type==LxPriPre) {
        popPreStatement(lexems, result);
    }
    else if (lexems[0]->type==LxPriPost) {
        popPostStatement(lexems, result);
    }
    else if (lexems[0]->type==LxPriIf) {
        popIfStatement(lexems, result);
    }
    else if (lexems[0]->type==LxPriThen) {
        popThenStatement(lexems, result);
    }
    else if (lexems[0]->type==LxPriElse) {
        popElseStatement(lexems, result);
    }
    else if (lexems[0]->type==LxPriFi) {
        popFiStatement(lexems, result);
    }
    else if (lexems[0]->type==LxPriSwitch) {
        popSwitchStatement(lexems, result);
    }
    else if (lexems[0]->type==LxPriCase) {
        popCaseStatement(lexems, result);
    }
    else if (lexems[0]->type==LxPriLoop) {
        popLoopStatement(lexems, result);
    }
    else if (lexems[0]->type==LxPriEndLoop) {
        popEndLoopStatement(lexems, result);
    }
    else if (lexems[0]->type==LxPriInput) {
        popInputStatement(lexems, result);
    }
    else if (lexems[0]->type==LxPriOutput) {
        popOutputStatement(lexems, result);
    }
    else if (lexems[0]->type==LxPriFinput) {
        popFinputStatement(lexems, result);
    }
    else if (lexems[0]->type==LxPriFoutput) {
        popFoutputStatement(lexems, result);
    }
    else if (lexems[0]->type==LxPriAssert) {
        popAssertStatement(lexems, result);
    }
    else if (lexems[0]->type==LxPriImport) {
        popImportStatement(lexems, result);
    }
    else if (lexems[0]->type==LxPriExit) {
        popExitStatement(lexems, result);
    }
    else if (lexems[0]->type==LxNameClass) {
        popVarDeclStatement(lexems, result);
    }
    else {
        qFatal("Wrong first statement primary keyword");
    }
}

void popLexemsUntilPrimaryKeyword(QList<Lexem*> & lexems, Statement &result)
{
    while (lexems.size()>0) {
        Lexem * lx = lexems[0];
        if (lx->type==LxOperSemicolon || lx->type & LxTypePrimaryKwd)
            break;
        lexems.pop_front();
        result.data << lx;
    }
}

void popLexemsUntilPrimaryKeywordOrVarDecl(QList<Lexem*> &lexems, Statement &result)
{
    while (lexems.size()>0) {
        Lexem * lx = lexems[0];
        if (lx->type==LxOperSemicolon || lx->type & LxTypePrimaryKwd || lx->type==LxNameClass)
            break;
        lexems.pop_front();
        result.data << lx;
    }
}

void popModuleStatement(QList<Lexem*> & lexems, Statement &result)
{
    result.type = lexems[0]->type;
    result.data << lexems[0];
    lexems.pop_front();
    popLexemsUntilPrimaryKeywordOrVarDecl(lexems, result);
}

void popEndModuleStatement(QList<Lexem*> &lexems, Statement &result)
{
    result.type = lexems[0]->type;
    result.data << lexems[0];
    lexems.pop_front();
    popLexemsUntilPrimaryKeywordOrVarDecl(lexems, result);
}

void popAlgHeaderStatement(QList<Lexem*> &lexems, Statement &result)
{
    result.type = lexems[0]->type;
    result.data << lexems[0];
    lexems.pop_front();
    popLexemsUntilPrimaryKeyword(lexems, result);
}

void popAlgBeginStatement(QList<Lexem*> &lexems, Statement &result)
{
    result.type = lexems[0]->type;
    result.data << lexems[0];
    lexems.pop_front();
    popLexemsUntilPrimaryKeywordOrVarDecl(lexems, result);
}

void popAlgEndStatement(QList<Lexem*> &lexems, Statement &result)
{
    result.type = lexems[0]->type;
    result.data << lexems[0];
    lexems.pop_front();
    popLexemsUntilPrimaryKeywordOrVarDecl(lexems, result);
}

void popExitStatement(QList<Lexem*> &lexems, Statement &result)
{
    result.type = lexems[0]->type;
    result.data << lexems[0];
    lexems.pop_front();
    popLexemsUntilPrimaryKeywordOrVarDecl(lexems, result);
}

void popPreStatement(QList<Lexem*> &lexems, Statement &result)
{
    result.type = lexems[0]->type;
    result.data << lexems[0];
    lexems.pop_front();
    popLexemsUntilPrimaryKeyword(lexems, result);
}

void popPostStatement(QList<Lexem*> &lexems, Statement &result)
{
    result.type = lexems[0]->type;
    result.data << lexems[0];
    lexems.pop_front();
    popLexemsUntilPrimaryKeyword(lexems, result);
}

void popIfStatement(QList<Lexem*> &lexems, Statement &result)
{
    result.type = lexems[0]->type;
    result.data << lexems[0];
    lexems.pop_front();
    popLexemsUntilPrimaryKeywordOrVarDecl(lexems, result);
}

void popThenStatement(QList<Lexem*> &lexems, Statement &result)
{
    result.type = lexems[0]->type;
    result.data << lexems[0];
    lexems.pop_front();
    popLexemsUntilPrimaryKeywordOrVarDecl(lexems, result);
}

void popElseStatement(QList<Lexem*> &lexems, Statement &result)
{
    result.type = lexems[0]->type;
    result.data << lexems[0];
    lexems.pop_front();
    popLexemsUntilPrimaryKeywordOrVarDecl(lexems, result);
}

void popFiStatement(QList<Lexem*> &lexems, Statement &result)
{
    result.type = lexems[0]->type;
    result.data << lexems[0];
    lexems.pop_front();
    popLexemsUntilPrimaryKeywordOrVarDecl(lexems, result);
}

void popSwitchStatement(QList<Lexem*> &lexems, Statement &result)
{
    result.type = lexems[0]->type;
    result.data << lexems[0];
    lexems.pop_front();
    popLexemsUntilPrimaryKeyword(lexems, result);
}

void popCaseStatement(QList<Lexem*> &lexems, Statement &result)
{
    result.type = lexems[0]->type;
    result.data << lexems[0];
    lexems.pop_front();
    popLexemsUntilPrimaryKeyword(lexems, result);
}

void popLoopStatement(QList<Lexem*> &lexems, Statement &result)
{
    result.type = lexems[0]->type;
    result.data << lexems[0];
    lexems.pop_front();
    popLexemsUntilPrimaryKeyword(lexems, result);
}

void popEndLoopStatement(QList<Lexem*> &lexems, Statement &result)
{
    result.type = lexems[0]->type;
    result.data << lexems[0];
    lexems.pop_front();
    if (lexems.size()>0) {
        Lexem * lx = lexems[0];
        if (lx->type==LxPriCase) {
            lx->type = LxSecIf;
            result.data << lx;
            lexems.pop_front();
        }
    }
    popLexemsUntilPrimaryKeyword(lexems, result);
}

void popInputStatement(QList<Lexem*> &lexems, Statement &result)
{
    result.type = lexems[0]->type;
    result.data << lexems[0];
    lexems.pop_front();
    popLexemsUntilPrimaryKeywordOrVarDecl(lexems, result);
}

void popOutputStatement(QList<Lexem*> &lexems, Statement &result)
{
    result.type = lexems[0]->type;
    result.data << lexems[0];
    lexems.pop_front();
    popLexemsUntilPrimaryKeywordOrVarDecl(lexems, result);
}

void popFinputStatement(QList<Lexem*> &lexems, Statement &result)
{
    result.type = lexems[0]->type;
    result.data << lexems[0];
    lexems.pop_front();
    popLexemsUntilPrimaryKeywordOrVarDecl(lexems, result);
}

void popFoutputStatement(QList<Lexem*> &lexems, Statement &result)
{
    result.type = lexems[0]->type;
    result.data << lexems[0];
    lexems.pop_front();
    popLexemsUntilPrimaryKeywordOrVarDecl(lexems, result);
}

void popAssertStatement(QList<Lexem*> &lexems, Statement &result)
{
    result.type = lexems[0]->type;
    result.data << lexems[0];
    lexems.pop_front();
    popLexemsUntilPrimaryKeywordOrVarDecl(lexems, result);
}

void popImportStatement(QList<Lexem*> &lexems, Statement &result)
{
    result.type = lexems[0]->type;
    result.data << lexems[0];
    lexems.pop_front();
    popLexemsUntilPrimaryKeywordOrVarDecl(lexems, result);
}

void popVarDeclStatement(QList<Lexem*> &lexems, Statement &result)
{
    result.type = lexems[0]->type;
    result.data << lexems[0];
    lexems.pop_front();
    popLexemsUntilPrimaryKeyword(lexems, result);
}


QStringList LexerPrivate::keyWords = QStringList();
QStringList LexerPrivate::operators = QStringList();
QStringList LexerPrivate::typeNames = QStringList();
QStringList LexerPrivate::constNames = QStringList();
QStringList LexerPrivate::compounds = QStringList();
QHash<QString,LexemType> LexerPrivate::kwdMap = QHash<QString, LexemType>();
QRegExp LexerPrivate::rxCompound = QRegExp();
QRegExp LexerPrivate::rxKeyWords = QRegExp();
QRegExp LexerPrivate::rxConst = QRegExp();
QRegExp LexerPrivate::rxTypes = QRegExp();
QHash<QString,AST::VariableBaseType> LexerPrivate::baseTypes = QHash<QString, AST::VariableBaseType>();
QHash<QString,bool> LexerPrivate::boolConstantValues = QHash<QString,bool>();
QSet<QString> LexerPrivate::arrayTypes = QSet<QString>();
QString LexerPrivate::retvalKeyword = QString();

QString Lexer::testName(const QString &name)
{
    if ( name.isEmpty() )
        return 0;

    /*
        TN_BAD_NAME_1 "Плохой символ в имени"
        TN_BAD_NAME_1A "Плохой символ в имени"
        TN_BAD_NAME_2 "Имя не может нач. с цифры"
        TN_BAD_NAME_3 "Ключевое слово в имени"
        TN_BAD_NAME_4 "Непарная \""
        */

    QString pattern = QString::fromUtf8("[+\\-=:*&?/><#%()\\^$.,");
//    if (!m_allowHiddenNames)
//        pattern += "!";
    pattern += QString::fromUtf8("|№\\[\\]{}~`\\\\]");

    QRegExp rxSym = QRegExp (pattern);

    Q_ASSERT ( rxSym.isValid() );

    int st = 0;
    while  ( ( st < name.length() )&& ( name[st]==' ' || name[st] == '\t' ) ) {
        st ++ ;
    }

    if ( name[st].isDigit() )
    {
        return _("Name starts with digit");
    }

    if ( name.count ( "\"" ) % 2 )
    {
        return _("Name contains quotation symbol");
    }

    QRegExp rxKwd = QRegExp(trUtf8("\\bзнач\\b|\\bтаб\\b"));

    int ps; // позиция первого найденного неправильного символа
    int pks; // позиция первого найденного ксимвола

    ps = rxSym.indexIn ( name );
    QString found = rxSym.cap();
    QRegExp KS = QRegExp ( "[\\x3E8-\\x3EF]+|[\\x1100-\\x1200]+" );
    pks = KS.indexIn ( name );

    QString error = 0;

    if ( ps != -1 )
    {
        error = _("Bad symbol in name");
    }

    if ( pks != -1 )
    {
        QChar debug = name[pks];
        ushort debug_code = debug.unicode();
        Q_UNUSED(debug_code);
        if ( error > 0 )
        {
            if ( pks < ps )
            {
                error = _("Name contains keyword");
            }
        }
        else
        {
            error = _("Name contains keyword");
        }
    }


    pks = rxKwd.indexIn(name);

    if ( pks != -1 )
    {
        QChar debug = name[pks];
        ushort debug_code = debug.unicode();
        Q_UNUSED(debug_code);
        if ( error > 0 )
        {
            if ( pks < ps )
            {
                error = _("Name contains keyword");
            }
        }
        else
        {
            error = _("Name contains keyword");
        }
    }


    return error;

    if ( name.contains ( "\"" ) && !name.startsWith ( "\"" ) && ( name.count ( "\"" ) % 2 ) ==0 )
    {
        return _("Error contains unpaired quote");
    }
    return error;
}

AST::VariableBaseType Lexer::baseTypeByClassName(const QString &clazz) const
{
    if (d->baseTypes.contains(clazz)) {
        return d->baseTypes[clazz];
    }
    else {
        return AST::TypeNone;
    }
}

bool Lexer::isArrayClassName(const QString &clazz) const
{
    return d->arrayTypes.contains(clazz);
}

bool Lexer::boolConstantValue(const QString &val) const
{
    Q_ASSERT(d->boolConstantValues.contains(val));
    return d->boolConstantValues[val];
}

bool Lexer::isReturnVariable(const QString &name) const
{
    return name==d->retvalKeyword;
}



}
