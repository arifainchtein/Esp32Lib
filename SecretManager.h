/*
  SecretManager.h - Library Managing the TOTP
  Created by Ari Fainchtein, Feb 13, 2019.
  Released into the public domain.
*/
#ifndef SecretManager_h
#define SecretManager_h
#include "Arduino.h"
#include <TimeManager.h>


class SecretManager{
	protected:
	TimeManager & timeManager;



	public:
		SecretManager(TimeManager & t) ;
		virtual void saveSecret(String secret, int numberDigits, int periodSeconds )=0;
		String void readSecret()=0;
		virtual long generateCode()=0;
		boolean checkCode(long userCode);
		static constexpr int SHARED_SECRET_LENGTH=27;

	private:

};

#endif
