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
	// new thread ?
	// std::cout << value << std::endl;
	return CommonApi::instance().post_data("/v1/user/login", json_v);

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
		if (!this->current_token.empty()) {
			this->start_check_request(handler, eventId);
		}
	};
	timer.StartTimer(1000, std::bind(x, handler));

}


bool UserModel::IsUserLogin() {
	return !this->current_token.empty();
}



void UserModel::start_check_request(wxWindow* handler, const int &event_id) {
	wxThreadEvent event(wxEVT_THREAD);
	event.SetInt(event_id);
	// using namespace chrono;
	auto ts = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	event.SetTimestamp(ts);
	// event.Clone();
	/*
	for(wxWindow* m_frame : event_tables){
		wxQueueEvent( m_frame, event.Clone());
	}*/
	wxQueueEvent(handler, event.Clone());
	// wxLogMessage("Start checking User ");
}

UserModel &UserModel::instance() {
	static UserModel c;
	return c;
}

void UserModel::check_login(wxWindow& handler, const std::string &value, const std::string &password) {
	auto task = this->check_login(value, password);
	task.then([&](ResponseEntity v) {
		auto success = v.success;
		wxThreadEvent event(wxEVT_THREAD, handler.GetId());
		event.SetInt(success ? 1 : 0);
		// event.SetB
		event.SetPayload(v);
		wxQueueEvent(&handler, event.Clone());
	});
}

void UserModel::on_user_login(const ResponseEntity &user_data) {
	// std::cout << "Token:" << user_json[U("token")] << std::endl;
	// this->current_token = user_json[U("token")].as_string();
	this->userInfo = user_data.result;
};

void UserModel::UpdateToken(utility::string_t token) {
	this->current_token = token;
};

utility::string_t UserModel::GetToken() {
	return this->current_token;
};

UserModel::~UserModel() = default;

/*
void UserModel::add_event_frame(wxWindow *frame) {
	this->event_tables.push_back(frame);
}
*/