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
#include "../common/common_event_ids.h"
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
    pplx::task<ResponseEntity> check_login(const std::string &value, const std::string &password);
    void check_login(wxWindow* handler,const std::string &value,const std::string &password);
    pplx::task<ResponseEntity> GetUserInfo();
    void start_user_check_loop(wxWindow* handler, const int& eventId);
	bool IsUserLogin();
    void on_user_login(const ResponseEntity &user_data);
	void UpdateToken(const utility::string_t& token);
	utility::string_t & GetToken();
    // void add_event_frame(wxWindow* frame);
    void terminate();
    void start_check_request(wxWindow* handler, const int &event_id);
private:
    UserModel():timer(){

    }
    ~UserModel();
    Timer timer;
	utility::string_t current_token = U("");
	web::json::value userInfo;
	long last_refresh_time = 0;
};


#endif //FUCK_USER_MODEL_H
