//
// Created by zzzhr on 2018/8/23.
//

#ifndef FUCK_USER_MODEL_H
#define FUCK_USER_MODEL_H

#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../util/common_api.h"
#include "cpprest/http_client.h"
#include "../util/simple_timer.h"
/*
class MyWorkerThread : public wxThread
{
public:
    MyWorkerThread(std::vector<wxWindow*> &event_tables);

    // thread execution starts here
    virtual void *Entry();

    // called when the thread exits - whether it terminates normally or is
    // stopped with Delete() (but not when it is Kill()ed!)
    virtual void OnExit();

private:
    std::vector<wxWindow*>* event_tables = nullptr;
};
*/
class UserModel {
public:
    static UserModel& instance();
    pplx::task<web::json::value> check_login(std::string &value, std::string &password);
    void check_login(wxWindow& handler, std::string &value, std::string &password);
    void start_user_check_loop(wxWindow* handler);
    void on_user_login(web::json::value &user_json);
    // void add_event_frame(wxWindow* frame);
    void terminate();
    void start_check_request(wxWindow* handler);
private:
    UserModel():timer(){

    }
    ~UserModel();
    CommonApi common_api = CommonApi::instance();
    // std::vector<wxWindow*> event_tables;
    // MyWorkerThread *thread = new MyWorkerThread(event_tables);
    // int timer_id;
    Timer timer;
    std::string current_token = "";
};


#endif //FUCK_USER_MODEL_H
