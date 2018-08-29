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
pplx::task<ResponseEntity> UserModel::check_login(const std::string &value, const std::string& password) {
	using namespace web;
	web::json::value json_v;
	json_v[U("value")] = web::json::value::string(utility::conversions::to_string_t(value));
	json_v[U("password")] = web::json::value::string(utility::conversions::to_string_t(password));
	return CommonApi::instance().post_data(U("/v1/user/login"), json_v);

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


void UserModel::start_user_check_loop(wxWindow* handler, const int& eventId) {
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
	auto x = [&](wxWindow* handler) {
		long current =  std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		if (!this->current_token.empty()) {
			long time_diff = current - last_refresh_time;
			if(time_diff > 10){
				last_refresh_time = current;
				this->start_check_request(handler, eventId);
			}else{
				std::cout << time_diff << std::endl;
			}
		}
	};
	timer.StartTimer(1000 * 30, std::bind(x, handler)); // refresh user data every 30 sec.

}


bool UserModel::IsUserLogin() {
	return !this->current_token.empty();
}



void UserModel::start_check_request(wxWindow* handler, const int &event_id) {
	GetUserInfo().then([&,handler](ResponseEntity v){
		last_refresh_time = 0;
		if(v.success){

			wxThreadEvent event(wxEVT_THREAD);
			event.SetId(event_id);
			event.SetInt(USER_REFRESH_RESPONSE);
			// using namespace chrono;
			auto ts = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			event.SetTimestamp(ts);
			// event.Clone();
			/*
            for(wxWindow* m_frame : event_tables){
                wxQueueEvent( m_frame, event.Clone());
            }*/
			wxQueueEvent(handler, event.Clone());

		}else{
			// throw false
			if(v.status > 400 && v.status < 500){
				std::cout<<"Failed refresh, need re-login"<<std::endl;
				std::cout<< v.message <<std::endl;
			}

		}
	});

	// wxLogMessage("Start checking User ");
}

UserModel &UserModel::instance() {
	static UserModel c;
	return c;
}

void UserModel::check_login(wxWindow* handler, const std::string &value, const std::string &password) {
	auto task = this->check_login(value, password);
	task.then([&,handler](ResponseEntity v) {
		// auto success = v.success;
		SendCommonThreadEvent(handler, USER_LOGIN_RESPONSE, v);
		// event.SetB

	});
}



void UserModel::on_user_login(const ResponseEntity &user_data) {
	this->userInfo = user_data.result;
};

void UserModel::UpdateToken(const utility::string_t& token) {
	this->current_token = token;
};

utility::string_t & UserModel::GetToken() {
	return this->current_token;
}

pplx::task<ResponseEntity> UserModel::GetUserInfo() {
	using namespace web;
	return CommonApi::instance().post_data(U("/v1/user/info"), web::json::value::object(false));

};

UserModel::~UserModel() = default;

/*
void UserModel::add_event_frame(wxWindow *frame) {
	this->event_tables.push_back(frame);
}
*/