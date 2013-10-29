

namespace cpp active911
namespace perl active911

exception ServiceUnavailable {

	1: i32 error_code,
	2: string error_message
}

enum AuthType {
	
	DEVICE = 1,
	USER = 2,
}

struct Membership {
	
	1: i32 id,
	2: i32 agency_id,
	3: string page_type,
	4: string sound,
	5: i32 shift_id,
	6: i32 prev_shift_id,
	7: i32 shift_start,
	8: i32 shift_duration_seconds,
	9: string shift_daysofweek,
	10: string shift_on_filter,
	11: string shift_off_filter,
	12: set<i32> pagegroups,
	13: string icon_color,
	14: i32 icon_id,
	15: string capabilities,
}


struct Device {

	1: i32 id,
	2: string did,
	3: string email,
	4: string name,
	5: string c2dm_registration_id,
	6: string active911_apns_token,
	7: string active911_gcm_registration_id,
	8: string cadpage_apns_token,
	9: string web_registration_code,
	10: string device_type,
	11: double position_lat,
	12: double position_lon,
	13: i32 position_accuracy,
	14: i32 position_stamp,
	15: string response_action,
	16: i32 response_stamp,
	17: i32 response_specific_id,
	18: i32 subscription_id,
	19: string local_data,
	20: string client_version,
	21: i32 free_service_until,
	22: i32 last_registration,
	23: i32 last_ping,
	24: string device_key,
	25: list<Membership> memberships,
}



struct AuthDeviceResult {
	
	1: bool valid,
	2: AuthType type,
	3: Device device

}

service Auth {
	

	void ping(),
	
	AuthDeviceResult authenticate_device(2:string username, 3:string password) throws (1:ServiceUnavailable sunav)
}







