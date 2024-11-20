#include "ashe/config.h"
#include "ashe/win/schedule_task.h"
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // !WIN32_LEAN_AND_MEAN
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif  // !_WINSOCKAPI_
#include <Windows.h>
#endif
#include <tchar.h>
#include <comdef.h>
#include <taskschd.h>
#include <strsafe.h>
#include <assert.h>

namespace ashe {
namespace win {

class ScopedBSTR {
   public:
    ScopedBSTR(const wchar_t* v) {
        if (v)
            s_ = SysAllocString(v);
    }
    ~ScopedBSTR() {
        if (s_) {
            SysFreeString(s_);
        }
    }

    operator BSTR() const { return s_; }

    BSTR s_ = nullptr;
};

class ScheduleTask::Private {
   public:
    ITaskService* m_lpITS = NULL;
    ITaskFolder* m_lpRootFolder = NULL;
};

ScheduleTask::ScheduleTask() :
    p_(new ScheduleTask::Private()) {
    HRESULT hr = ::CoInitialize(NULL);
    assert(SUCCEEDED(hr));

    hr = ::CoCreateInstance(CLSID_TaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskService,
                            (LPVOID*)(&p_->m_lpITS));
    if (SUCCEEDED(hr)) {
        hr = p_->m_lpITS->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
        assert(SUCCEEDED(hr));

        hr = p_->m_lpITS->GetFolder(_bstr_t(TEXT("\\")), &p_->m_lpRootFolder);
        assert(SUCCEEDED(hr));
    }
}

ScheduleTask::~ScheduleTask() {
    if (p_->m_lpITS) {
        p_->m_lpITS->Release();
    }
    if (p_->m_lpRootFolder) {
        p_->m_lpRootFolder->Release();
    }
    ::CoUninitialize();

    delete p_;
    p_ = nullptr;
}

bool ScheduleTask::deleteTask(const wchar_t* pszTaskName) {
    if (NULL == p_->m_lpRootFolder) {
        return false;
    }
    ScopedBSTR variantTaskName(pszTaskName);
    HRESULT hr = p_->m_lpRootFolder->DeleteTask(variantTaskName, 0);
    if (FAILED(hr)) {
        return false;
    }

    return true;
}

bool ScheduleTask::deleteFolder(const wchar_t* pszFolderName) {
    if (NULL == p_->m_lpRootFolder) {
        return false;
    }
    ScopedBSTR variantFolderName(pszFolderName);
    HRESULT hr = p_->m_lpRootFolder->DeleteFolder(variantFolderName, 0);
    if (FAILED(hr)) {
        return false;
    }

    return true;
}

// ITrigger type is TASK_TRIGGER_LOGON
// Action number is 1
//
bool ScheduleTask::createLoginTriggerTask(const wchar_t* pszTaskName,
                                          const wchar_t* pszProgramPath,
                                          const wchar_t* pszParameters,
                                          const wchar_t* pszDescription,
                                          const wchar_t* pszAuthor) {
    if (NULL == p_->m_lpRootFolder) {
        return false;
    }

    deleteTask(pszTaskName);

    ITaskDefinition* pTaskDefinition = NULL;
    HRESULT hr = p_->m_lpITS->NewTask(0, &pTaskDefinition);
    if (FAILED(hr)) {
        return false;
    }

    IRegistrationInfo* pRegInfo = NULL;
    ScopedBSTR variantAuthor(pszAuthor);
    ScopedBSTR variantDescription(pszDescription);
    hr = pTaskDefinition->get_RegistrationInfo(&pRegInfo);
    if (FAILED(hr)) {
        return false;
    }

    hr = pRegInfo->put_Author(variantAuthor);
    hr = pRegInfo->put_Description(variantDescription);
    pRegInfo->Release();

    IPrincipal* pPrincipal = NULL;
    hr = pTaskDefinition->get_Principal(&pPrincipal);
    if (FAILED(hr)) {
        return false;
    }

    hr = pPrincipal->put_LogonType(TASK_LOGON_INTERACTIVE_TOKEN);
    hr = pPrincipal->put_RunLevel(TASK_RUNLEVEL_HIGHEST);
    pPrincipal->Release();

    ITaskSettings* pSettting = NULL;
    hr = pTaskDefinition->get_Settings(&pSettting);
    if (FAILED(hr)) {
        return false;
    }

    hr = pSettting->put_StopIfGoingOnBatteries(VARIANT_FALSE);
    hr = pSettting->put_DisallowStartIfOnBatteries(VARIANT_FALSE);
    hr = pSettting->put_AllowDemandStart(VARIANT_TRUE);
    hr = pSettting->put_StartWhenAvailable(VARIANT_FALSE);
    hr = pSettting->put_MultipleInstances(TASK_INSTANCES_PARALLEL);
    pSettting->Release();

    IActionCollection* pActionCollect = NULL;
    hr = pTaskDefinition->get_Actions(&pActionCollect);
    if (FAILED(hr)) {
        return false;
    }

    // add program one or more
    {
        IAction* pAction = NULL;
        hr = pActionCollect->Create(TASK_ACTION_EXEC, &pAction);

        ScopedBSTR variantProgramPath(pszProgramPath);
        ScopedBSTR variantParameters(pszParameters);

        IExecAction* pExecAction = NULL;
        hr = pAction->QueryInterface(IID_IExecAction, (PVOID*)(&pExecAction));
        if (FAILED(hr)) {
            pAction->Release();
            return false;
        }
        pAction->Release();

        pExecAction->put_Path(variantProgramPath);
        pExecAction->put_Arguments(variantParameters);
        pExecAction->Release();
    }

    pActionCollect->Release();

    ITriggerCollection* pTriggers = NULL;
    hr = pTaskDefinition->get_Triggers(&pTriggers);
    if (FAILED(hr)) {
        return false;
    }

    ITrigger* pTrigger = NULL;
    hr = pTriggers->Create(TASK_TRIGGER_LOGON, &pTrigger);
    if (FAILED(hr)) {
        return false;
    }

    IRegisteredTask* pRegisteredTask = NULL;
    ScopedBSTR variantTaskName(pszTaskName);
    hr = p_->m_lpRootFolder->RegisterTaskDefinition(
        variantTaskName, pTaskDefinition, TASK_CREATE_OR_UPDATE, _variant_t(), _variant_t(),
        TASK_LOGON_INTERACTIVE_TOKEN, _variant_t(""), &pRegisteredTask);
    if (FAILED(hr)) {
        pTaskDefinition->Release();
        return false;
    }
    pTaskDefinition->Release();
    pRegisteredTask->Release();

    return true;
}

bool ScheduleTask::isExist(const wchar_t* pszTaskName) {
    if (NULL == p_->m_lpRootFolder) {
        return false;
    }
    HRESULT hr = S_OK;
    ScopedBSTR variantTaskName(pszTaskName);
    IRegisteredTask* pRegisteredTask = NULL;

    hr = p_->m_lpRootFolder->GetTask(variantTaskName, &pRegisteredTask);
    if (FAILED(hr) || (NULL == pRegisteredTask)) {
        return false;
    }
    pRegisteredTask->Release();

    return true;
}

bool ScheduleTask::isTaskValid(const wchar_t* pszTaskName) {
    if (NULL == p_->m_lpRootFolder) {
        return false;
    }
    HRESULT hr = S_OK;
    ScopedBSTR variantTaskName(pszTaskName);
    IRegisteredTask* pRegisteredTask = NULL;

    hr = p_->m_lpRootFolder->GetTask(variantTaskName, &pRegisteredTask);
    if (FAILED(hr) || (NULL == pRegisteredTask)) {
        return false;
    }

    TASK_STATE taskState;
    hr = pRegisteredTask->get_State(&taskState);
    if (FAILED(hr)) {
        pRegisteredTask->Release();
        return false;
    }
    pRegisteredTask->Release();

    if (TASK_STATE_DISABLED == taskState) {
        return false;
    }

    return true;
}

bool ScheduleTask::run(const wchar_t* pszTaskName, const wchar_t* pszParam) {
    if (NULL == p_->m_lpRootFolder) {
        return false;
    }
    HRESULT hr = S_OK;
    ScopedBSTR variantTaskName(pszTaskName);

    IRegisteredTask* pRegisteredTask = NULL;
    hr = p_->m_lpRootFolder->GetTask(variantTaskName, &pRegisteredTask);
    if (FAILED(hr) || (NULL == pRegisteredTask)) {
        return false;
    }

    VARIANT var;
    var.vt = VT_LPWSTR;
    var.bstrVal = (BSTR)pszParam;
    hr = pRegisteredTask->Run(var, NULL);
    if (FAILED(hr)) {
        pRegisteredTask->Release();
        return false;
    }
    pRegisteredTask->Release();

    return true;
}

bool ScheduleTask::isEnabled(const wchar_t* pszTaskName) {
    if (NULL == p_->m_lpRootFolder) {
        return false;
    }
    HRESULT hr = S_OK;
    ScopedBSTR variantTaskName(pszTaskName);
    IRegisteredTask* pRegisteredTask = NULL;

    hr = p_->m_lpRootFolder->GetTask(variantTaskName, &pRegisteredTask);
    if (FAILED(hr) || (NULL == pRegisteredTask)) {
        return false;
    }

    VARIANT_BOOL enabled = VARIANT_FALSE;
    pRegisteredTask->get_Enabled(&enabled);
    pRegisteredTask->Release();

    return (enabled == VARIANT_TRUE);
}

bool ScheduleTask::setEnabled(const wchar_t* pszTaskName, bool bEnable) {
    if (NULL == p_->m_lpRootFolder) {
        return false;
    }
    HRESULT hr = S_OK;
    ScopedBSTR variantTaskName(pszTaskName);
    IRegisteredTask* pRegisteredTask = NULL;

    hr = p_->m_lpRootFolder->GetTask(variantTaskName, &pRegisteredTask);
    if (FAILED(hr) || (NULL == pRegisteredTask)) {
        return false;
    }

    pRegisteredTask->put_Enabled(bEnable ? VARIANT_TRUE : VARIANT_FALSE);
    pRegisteredTask->Release();

    return true;
}

bool ScheduleTask::getProgramPath(const wchar_t* pszTaskName, long lActionIndex, std::wstring& programPath) {
    if (NULL == p_->m_lpRootFolder)
        return false;

    HRESULT hr = S_OK;
    ScopedBSTR variantTaskName(pszTaskName);
    IRegisteredTask* pRegisteredTask = NULL;
    ITaskDefinition* pTaskDefinition = NULL;
    IActionCollection* pActionColl = NULL;
    IAction* pAction = NULL;
    IExecAction* pExecAction = NULL;

    bool bRet = false;
    do {
        hr = p_->m_lpRootFolder->GetTask(variantTaskName, &pRegisteredTask);
        if (FAILED(hr) || (NULL == pRegisteredTask))
            break;

        hr = pRegisteredTask->get_Definition(&pTaskDefinition);
        if (FAILED(hr) || !pTaskDefinition)
            break;

        hr = pTaskDefinition->get_Actions(&pActionColl);
        if (FAILED(hr) || !pActionColl)
            break;

        hr = pActionColl->get_Item(lActionIndex, &pAction);
        if (FAILED(hr) || !pAction)
            break;

        hr = pAction->QueryInterface(IID_IExecAction, (PVOID*)(&pExecAction));
        if (FAILED(hr) || !pExecAction)
            break;

        BSTR path = NULL;
        hr = pExecAction->get_Path(&path);
        if (FAILED(hr))
            break;

        if (path)
            programPath = path;

        bRet = true;
    } while (false);

    if (pExecAction)
        pExecAction->Release();

    if (pAction)
        pAction->Release();

    if (pActionColl)
        pActionColl->Release();

    if (pTaskDefinition)
        pTaskDefinition->Release();

    if (pRegisteredTask)
        pRegisteredTask->Release();

    return bRet;
}

bool ScheduleTask::getParameters(const wchar_t* pszTaskName, long lActionIndex, std::wstring& parameters) {
    if (NULL == p_->m_lpRootFolder)
        return false;

    HRESULT hr = S_OK;
    ScopedBSTR variantTaskName(pszTaskName);
    IRegisteredTask* pRegisteredTask = NULL;
    ITaskDefinition* pTaskDefinition = NULL;
    IActionCollection* pActionColl = NULL;
    IAction* pAction = NULL;
    IExecAction* pExecAction = NULL;

    bool bRet = false;
    do {
        hr = p_->m_lpRootFolder->GetTask(variantTaskName, &pRegisteredTask);
        if (FAILED(hr) || (NULL == pRegisteredTask))
            break;

        hr = pRegisteredTask->get_Definition(&pTaskDefinition);
        if (FAILED(hr) || !pTaskDefinition)
            break;

        hr = pTaskDefinition->get_Actions(&pActionColl);
        if (FAILED(hr) || !pActionColl)
            break;

        hr = pActionColl->get_Item(lActionIndex, &pAction);
        if (FAILED(hr) || !pAction)
            break;

        hr = pAction->QueryInterface(IID_IExecAction, (PVOID*)(&pExecAction));
        if (FAILED(hr) || !pExecAction)
            break;

        BSTR arg = NULL;
        hr = pExecAction->get_Arguments(&arg);
        if (FAILED(hr))
            break;

        if (arg) {
            parameters = arg;
        }
        bRet = true;
    } while (false);

    if (pExecAction)
        pExecAction->Release();

    if (pAction)
        pAction->Release();

    if (pActionColl)
        pActionColl->Release();

    if (pTaskDefinition)
        pTaskDefinition->Release();

    if (pRegisteredTask)
        pRegisteredTask->Release();

    return bRet;
}
}  // namespace win
}  // namespace ashe