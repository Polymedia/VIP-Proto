#include "rconsole.h"

#include <Rinternals.h>
#include <Rembedded.h>

#include <R_ext/RStartup.h>
#include <R_ext/Parse.h>

static QMap<QString, SEXP> createShortcuts()
{
    QMap<QString, SEXP> s;
    s.insert("[[", R_Bracket2Symbol);
    s.insert("[", R_BracketSymbol);
    s.insert("{", R_BraceSymbol);
    s.insert("class", R_ClassSymbol);
    s.insert(".Device", R_DeviceSymbol);
    s.insert("dimnames", R_DimNamesSymbol);
    s.insert("dim", R_DimSymbol);
    s.insert("$", R_DollarSymbol);
    s.insert("...", R_DotsSymbol);
    s.insert("drop", R_DropSymbol);
    s.insert(".Last.value", R_LastvalueSymbol);
    s.insert("levels", R_LevelsSymbol);
    s.insert("mode", R_ModeSymbol);
    s.insert("name", R_NameSymbol);
    s.insert("names", R_NamesSymbol);
    s.insert("na.rm", R_NaRmSymbol);
    s.insert("package", R_PackageSymbol);
    s.insert("quote", R_QuoteSymbol);
    s.insert("row.names", R_RowNamesSymbol);
    s.insert(".Random.seed", R_SeedsSymbol);
    s.insert("source", R_SourceSymbol);
    s.insert("tsp", R_TspSymbol);
    s.insert(".defined", R_dot_defined);
    s.insert(".Method", R_dot_Method);
    s.insert(".target", R_dot_target);
    return s;
}

static RConsole *R = nullptr;

static int ReadConsole(const char *prompt, char *buf, int len, int /*addtohistory*/)
{
    fputs(prompt, stdout);
    fflush(stdout);
    if(fgets(buf, len, stdin)) return 1; else return 0;
}

static void WriteConsole(const char *buf, int /*len*/)
{
    //printf("%s", buf);
    R->write(QString::fromLocal8Bit(buf));
}

static void CallBack(void)
{
    /* called during i/o, eval, graphics in ProcessEvents */
    R->callback();
}

static void Busy(int which)
{
    /* set a busy cursor ... if which = 1, unset if which = 0 */
    R->busy(which);
}

RConsole::RConsole(bool verbose, QObject *parent) :
    QObject(parent),
    m_verbose(verbose)
{
    const char *R_argv[] = { "--gui=none", "--no-save",
                                "--no-readline", "--silent", "--vanilla", "--slave"};
    int R_argc = sizeof(R_argv) / sizeof(R_argv[0]);
    Rf_initEmbeddedR(R_argc, (char**)R_argv);

    //R_CStackLimit = -1;

    R_ReplDLLinit();

    structRstart Rst;
    R_DefParams(&Rst);
    Rst.R_Interactive = (Rboolean)true;
    Rst.rhome = get_R_HOME();
    Rst.home = getRUser();
    Rst.CharacterMode = LinkDLL;
    Rst.ReadConsole = ReadConsole;
    Rst.WriteConsole = WriteConsole;
    Rst.CallBack = CallBack;
    //Rst.ShowMessage = askok;
    //Rst.YesNoCancel = askyesnocancel;
    Rst.Busy = Busy;
    //Rst.WriteConsoleEx = myWriteConsoleEx;
    R_SetParams(&Rst);

    R = this;
    RObject::m_shortcuts = createShortcuts();
}

RConsole::~RConsole()
{
    R_RunExitFinalizers();
    R_CleanTempDir();
    Rf_endEmbeddedR(0);

    R = nullptr;
}

RBind RConsole::operator[](const QString &name)
{
    return RBind(name);
}

bool RConsole::execute(const QString &code)
{
    ParseStatus status;
    SEXP ans, cmdSexp, cmdexpr = R_NilValue;

    PROTECT(cmdSexp = Rf_allocVector(STRSXP, 1));
    SET_STRING_ELT(cmdSexp, 0, Rf_mkChar(code.toLocal8Bit().constData()));

    cmdexpr = PROTECT(R_ParseVector(cmdSexp, -1, &status, R_NilValue));

    switch (status)
    {
    case PARSE_OK:
        // Loop is needed here as EXPSEXP might be of length > 1
        for(int i = 0; i < Rf_length(cmdexpr); i++) {
            int errorOccurred;
            ans = R_tryEval(VECTOR_ELT(cmdexpr, i), R_GlobalEnv, &errorOccurred);
            if (errorOccurred) {
                emit error(QString::fromLocal8Bit(R_curErrorBuf()));

                UNPROTECT(2);
                return false;
            } else if (m_verbose)
                Rf_PrintValue(ans);
        }
        UNPROTECT(2);
        return true;
    case PARSE_INCOMPLETE:
        emit error(QString("Parse error (%1): parse is incomplete").arg(status));
        break;
    case PARSE_NULL:
        emit error(QString("Parse error (%1): parse is null").arg(status));
        break;
    case PARSE_ERROR:
        emit error(QString("Parse error (%1): \"%2\"").arg(status).arg(code));
        break;
    case PARSE_EOF:
        emit error(QString("Parse error (%1): end of file"));
        break;
    default:
        emit error(QString("Parse error (%1): not documented"));
        break;
    }

    UNPROTECT(2);
    return false;
}

