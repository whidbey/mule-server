

namespace cpp active911
namespace perl active911

exception ServiceUnavailable {

	1: i32 error_code,
	2: string error_message
}


struct AuthDeviceResult {
	
	1: bool valid,

}

service Interface {
	
	
	AuthDeviceResult authenticate_device(2:string username, 3:string password) throws (1:ServiceUnavailable sunav)
}







