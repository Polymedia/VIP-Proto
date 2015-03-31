#include "rconsole.h"

#include <Rinternals.h>
#include <Rembedded.h>

#include <R_ext/RStartup.h>
#include <R_ext/Parse.h>

#include <iostream>

static RConsole *R = nullptr;

static int ReadConsole(const char *prompt, char *buf, int len, int addtohistory)
{
    fputs(prompt, stdout);
    fflush(stdout);
    if(fgets(buf, len, stdin)) return 1; else return 0;
}

static void WriteConsole(const char *buf, int len)
{
    //printf("%s", buf);
    R->write(buf);
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
}

RConsole::~RConsole()
{
    R_RunExitFinalizers();
    R_CleanTempDir();
    Rf_endEmbeddedR(0);

    R = nullptr;
}

bool RConsole::execute(const QString &code)
{
    ParseStatus status;
    SEXP ans, cmdSexp, cmdexpr = R_NilValue;

    PROTECT(cmdSexp = Rf_allocVector(STRSXP, 1));
    SET_STRING_ELT(cmdSexp, 0, Rf_mkChar(code.toLocal8Bit().constData()));

    cmdexpr = PROTECT(R_ParseVector(cmdSexp, -1, &status, R_NilValue));

    std::cout << "1" << std::endl;

    switch (status)
    {
    case PARSE_OK:
        // Loop is needed here as EXPSEXP might be of length > 1
        for(int i = 0; i < Rf_length(cmdexpr); i++) {
            int errorOccurred;
            ans = R_tryEval(VECTOR_ELT(cmdexpr, i), R_GlobalEnv, &errorOccurred);
            std::cout << "2" << std::endl;
            if (errorOccurred) {
                std::cout << "3" << std::endl;
                if (m_verbose) {
                    QString evalError = R_curErrorBuf();
                    emit error(evalError);
                }

                UNPROTECT(2);
                return false;
            }
            std::cout << "4" << std::endl;
            if (m_verbose)
                emit value(RProxy(ans));
        }
        break;
    case PARSE_INCOMPLETE:
        // need to read another line
        break;
    case PARSE_NULL:
        if (m_verbose)
            emit error(QString("ParseStatus is null (%1)").arg(status));
        UNPROTECT(2);
        return false;
        break;
    case PARSE_ERROR:
        if (m_verbose)
            emit error(QString("Parse Error: \"%1\"").arg(code));
        UNPROTECT(2);
        return false;
        break;
    case PARSE_EOF:
        if (m_verbose)
            emit error(QString("ParseStatus is eof"));
        break;
    default:
        if (m_verbose)
             emit error(QString("ParseStatus is not documented"));
        UNPROTECT(2);
        return false;
        break;
    }

    UNPROTECT(2);
    return true;
}

