
#ifndef Py_PYDEBUG_H
#define Py_PYDEBUG_H
#ifdef __cplusplus
extern "C" {
#endif

extern DL_IMPORT(int) Py_DebugFlag;
extern DL_IMPORT(int) Py_VerboseFlag;
extern DL_IMPORT(int) Py_InteractiveFlag;
extern DL_IMPORT(int) Py_OptimizeFlag;
extern DL_IMPORT(int) Py_NoSiteFlag;
extern DL_IMPORT(int) Py_UseClassExceptionsFlag;
extern DL_IMPORT(int) Py_FrozenFlag;
extern DL_IMPORT(int) Py_TabcheckFlag;
extern DL_IMPORT(int) Py_UnicodeFlag;
extern DL_IMPORT(int) Py_IgnoreEnvironmentFlag;
extern DL_IMPORT(int) Py_DivisionWarningFlag;

/* this is a wrapper around getenv() that pays attention to
   Py_IgnoreEnvironmentFlag.  It should be used for getting variables like
   PYTHONPATH and PYTHONHOME from the environment */
#define Py_GETENV(s) (Py_IgnoreEnvironmentFlag ? NULL : getenv(s))

DL_IMPORT(void) Py_FatalError(char *message);

#ifdef __cplusplus
}
#endif
#endif /* !Py_PYDEBUG_H */
