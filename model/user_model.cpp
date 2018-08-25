//
// Created by zzzhr on 2018/8/23.
//

#include "user_model.h"
#include "chrono"

//
// W

/*
MyWorkerThread::MyWorkerThread(std::vector<wxWindow*> & event_tables)
        : wxThread()
{
    this->event_tables = &event_tables;
}

void MyWorkerThread::OnExit()
{
}
wxThread::ExitCode MyWorkerThread::Entry()
{

    return nullptr;
}
 */
//
//

///
/// \param value
/// \param password
pplx::task<web::json::value> UserModel::check_login(std::string &value, std::string& password) {
    using namespace web;
    web::json::value json_v;
	std::string cc = "";
    json_v[U("value")] = web::json::value::string(utility::conversions::to_string_t(value));
    json_v[U("password")] = web::json::value::string(utility::conversions::to_string_t(password));
    // new thread ?
    // std::cout << value << std::endl;
    return this->common_api.post_data("/v1/user/login",json_v,false);

}

void UserModel::terminate() {
    /*
    if(thread->IsRunning()){
        thread->Pause();
        thread->Kill();
    }
     */
    this->timer.Expire();
}


void UserModel::start_user_check_loop(wxWindow* handler) {
    // start a thread

    /*



    if ( thread->Create() != wxTHREAD_NO_ERROR )
    {
        wxLogError(wxT("Can't create thread!"));
        return;
    }
    //thread->
    thread->Run();
     */
    // int cc = 0;
    auto x = [&](wxWindow* handler){
        if(!this->current_token.empty()){
            this->start_check_request(handler);
        }
    };
    timer.StartTimer(1000, std::bind(x,handler));

}



void UserModel::start_check_request(wxWindow* handler) {
    wxThreadEvent event( wxEVT_THREAD, handler->GetId());
    // event.SetInt( 50 );
    // using namespace chrono;
    auto ts = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    event.SetTimestamp(ts);
    // event.Clone();
    /*
    for(wxWindow* m_frame : event_tables){
        wxQueueEvent( m_frame, event.Clone());
    }*/
    wxQueueEvent( handler, event.Clone());
    // wxLogMessage("Start checking User ");
}

UserModel &UserModel::instance() {
    static UserModel c;
    return c;
}

void UserModel::check_login(wxWindow& handler, std::string &value, std::string &password) {
    auto task = this->check_login(value,password);
    task.then([&](web::json::value v){
        auto success = v[U("success")].as_bool();
        wxThreadEvent event( wxEVT_THREAD, handler.GetId());
        event.SetInt(success ? 1 : 0);
        event.SetPayload(v);
        wxQueueEvent( &handler, event.Clone());
    });
}

void UserModel::on_user_login(web::json::value &user_json) {
    // std::cout << "Token:" << user_json[U("token")] << std::endl;
    this->current_token = utility::conversions::to_utf8string(user_json[U("token")].as_string());
};

UserModel::~UserModel() = default;

/*
void UserModel::add_event_frame(wxWindow *frame) {
    this->event_tables.push_back(frame);
}
*/